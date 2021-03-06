#include <sstream>
#include "zkclient.h"

static const char* code_strings[] = 
{
	"OK",
	"not connected",
	"connect timeout",
	"connect failed",
	"path not found",
	"read failed",
	"version not match",
	"write failed",
	"create failed",
	"create node exists",
	"exists failed",
	"remove failed",
	NULL,
};

const char* Client::code_to_string(int code) 
{
	if((code < kOK) || (code > kRemoveFailed)) 
	{
		return "(nil)";
	}
	else
	{
		return code_strings[code];
	}
}

static const char* state_to_string(int state) 
{
	if(state == ZOO_EXPIRED_SESSION_STATE) 
	{
		return "expired session state";
	} 
	else if (state == ZOO_AUTH_FAILED_STATE) 
	{
		return "auth failed state";
	} 
	else if (state == ZOO_CONNECTING_STATE)
	{
		return "connecting state";
	}
	else if (state == ZOO_ASSOCIATING_STATE) 
	{
		return "associating state";
	}
	else if (state == ZOO_CONNECTED_STATE) 
	{
		return "connected state";
	}
	else if (state == 0)
	{
		return "closed state";
	} 
	else 
	{
		return "invalid state";
	}
}

Client::Client(const char* server, int conn_timeout) : server_(server), conn_timeout_(conn_timeout), handler_(NULL) 
{
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	if (conn_timeout_ < 0) 
	{
		fprintf(stderr, "conn_timeout=%d,reset to %d", conn_timeout_, kConnTimeout);
		conn_timeout_ = kConnTimeout;
	}
}

Client::~Client()
{
	disconnect();
}

void Client::disconnect() 
{
	if(handler_)
	{
		zookeeper_close(handler_);
		handler_ = NULL;
	}
}

bool Client::isConnected() 
{
	return handler_ && (zoo_state(handler_) == ZOO_CONNECTED_STATE);
}

int Client::isConnected()
{
	if (handler_ != NULL || (zoo_state(handler_) == ZOO_EXPIRED_SESSION_STATE))
	{
		if (handler_ == NULL)
		{
			disconnect();
		}
		zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);
		handler_ = zookeeper_init(server_.c_str(), NULL, conn_timeout_, NULL, NULL, 0);
		if (handler_ == NULL)
		{
			return kNotConnected;
		}
	}

	int timeout = conn_timeout_;
	while (zoo_state(handler_) != ZOO_CONNECTED_STATE)
	{
		if (is_unrecoverable(handler_) == ZINVALIDSTATE)
		{
			disconnect();
			return kConnectFailed;
		}
		if (timeout == 0)
		{
			disconnect();
			return kConnectTimeOut;
		}
		else
		{
			timeout--;
			sleep(1);
		}
	}

	return 0;
}

static inline char* string_as_array(std::string *str)
{
	return str->empty() ? NULL : &*str->begin();
}

int Client::read(const char *pth, std::string *data, int *version, int size)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	if (data != NULL)
	{
		data->resize(size);
	}

	struct Stat stat;
	int code = zoo_get(handler_, path, 0, data ? string_as_array(data) : NULL, &size, &stat);
	if (code == ZNONODE)
	{
		return kPathNotFound;
	}
	else if (code != ZOK)
	{
		fprintf(stderr, "zoo_get(%s) failed (%s)", path, zerror(code));
		return kReadFailed;
	}

	if (data != NULL)
	{
		data->resize(size);
	}
	if (version != nULL)
	{ 
		*version = stat.version;
	}

	return 0;
}

int Client::readChildren(const char *path, std::vector<std::string> *children)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	struct String_vector strings;
	int code = zoo_get_children(handler_, path, 0, &strings);
	if (code == ZNONODE)
	{
		return kPathNotFound;
	}
	else if (code != ZOK)
	{
		fprintf(stderr, "zoo_get_child(%s) failed(%s)", path, zerror(code));
		return kReadFailed;
	}

	children->clear();
	for (int i = 0; i < strings.count; i++)
	{
		children->push_back(strings.data[i]);
	}

	deallocate_String_vector(&strings);
	return 0;
}

int Client::write(const char *path, const std::string &data, int version)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	int code = zoo_set(handler_, path, data.data(), data.size(), version);
	if (code == ZNONODE)
	{
		return kPathNotFound;
	}
	else if (code = ZBADVERSION)
	{
		return kVersionNotMatch;
	}
	else if (code != ZOK)
	{
		fprintf(stderr, "zoo_get(%s, %d) failed(%s)", path, version, zerror(code));
		return kWriteFailed;
	}

	return 0;
}

int Client::exists(const char *path)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	int code = zoo_exists(handler_, path, 0, NULL);
	if (code = ZNONODE)
	{
		return kPathNotFound;
	}
	else if (code != ZOK)
	{
		fprintf(stderr, "zoo_exists(%s) failed(%s)", path, zerror(code));
		return kExistsFailed;
	}

	return 0;
}

int Client::createNode(const char *path, bool temporary, std::string *vpath)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	if (vpath != NULL)
	{
		vpath->resize(kMaxPathLen);
	}

	int code = zoo_create(handler_, path, NULL, 0, &ZOO_OPEN_ACL_UNSAFE, temporary ? ZOO_EPHEMERAL | ZOO_SEQUENCE : 0, vapth ? string_as_array(vpath) : NULL, vapth ? kMaxPathLen : - 1);
	if (code = ZNONODE)
	{
		return kPathNotFound;
	}
	else if (code == ZNODEEXISTS)
	{
		return kCreateNodeExists;
	}
	else if (code != ZOK)
	{
		fprintf(stderr, "zoo_create(%s) failed(%s)", path, zerror(code));
	}

	if (vpath)
	{
		int size = strlen(string_as_array(vpath));
		vpath->resize(size);
	}

	return 0;
}

int Client::createTree(const char *path)
{
	if (!isConnected())
	{
		return kNotConnected;
	}
	
	std::string s(path);
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == '/')
		{
			s[i] = ' ';
		}
	}

	std::istringstream iss(s);
	std::vector<std::string> ss;
	while (iss >> s)
	{
		if (!s.empty())
		{
			ss.push_back(s);
		}
	}

	s.clear();
	for (size_t i = 0; i < ss.size(); i++)
	{
		s += '/';
		s += ss[i];
		int code = exists(s.c_str());
		if (code != 0)
		{
			if (code == kPathNotFound)
			{
				code = createNode(s.c_str());
				if (code != 0)
				{
					fprintf(stderr, "createNode(%s) failed(%s)", s.c_str(), code_to_string(code));
					return code;
				}
			}
			else
			{
				fprintf(stderr, "exists(%s) failed(%s)", s.c_str(), code_to_string(code));
				return code;
			}
		}
	}

	return 0;
}

int Client::removeNode(const char *path, int version)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	int code = zoo_delete(handler_, path, version);
	if (code == ZNONODE)
	{
		return kPathNotFound;
	}
	else if (code = ZBADVERSION)
	{
		return kVersionNotMatch;
	}
	else if (code != ZOK)
	{
		fprintf(stderr, "zoo_delete(%s, %d) failed(%s)", path, version, zerror(code));
		return kRemoveFailed;
	}

	return 0;
}

int Client::removeTree(const char *path)
{
	if (!isConnected())
	{
		return kNotConnected;
	}

	std::vector<std::string> children;
	int code = readChildren(path, &children);
	if (code != 0)
	{
		return code;
	}

	for (size_t i = 0; i < children.size(); i++)
	{
		std::string tmp(path);
		tmp += '/';
		tmp += children[i];
		code = removeTree(tmp.c_str());
		if (code != 0)
		{
			return code;
		}
	}

	code = removeNode(path);
	if (code != 0)
	{
		return code;
	}

	return 0;
}


