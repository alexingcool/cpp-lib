#include <sstream>
#include <string>
#include "hdfsclient.h"

static int getoutput(const char *cmd, std::string *out)
{
	if (cmd == NULL || out == NULL)
	{
		return -1;
	}

	FILE *fp = popen(cmd, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "popen(%s) failed(%s)", cmd, strerror);
		return -1;
	}

	static const int kReadBufferSize = 64 * (1 << 10);
	char buf[kReadBufferSize];
	out->clear();
	while (true)
	{
		int n = fread(buf, sizeof(char), kReadBufferSize, fp);
		out->append(buf, n);
		if (n < kReadBufferSize)
		{
			break;
		}
	}

	int n = pclose(fp);
	if (n == -1)
	{
		fprintf(stderr, "pclose(%p) failed(%s)", cmd, strerror);
		return -1;
	}

	return 0;
}

static int shell_call(const char *cmd, int retry, bool test = false)
{
	if (cmd == NULL)
	{
		return -1;
	}

	std::string out;
	while (retry)
	{
		retry--;
		if (getoutput(cmd, &out) == -1)
		{
			continue;
		}

		int retval = atoi(out.c_str());
		if (retval != 0)
		{
			if (test && retval == 1)
			{
				return 1;
			}
			fprintf(stderr, "execute '%s' failed retval = %d", cmd, retval);
			continue;
		}
		return 0;
	}

	return -1;
}

Client::Client(const char *hp_path, int retry_time) :
	hp_path_(hp_path), retry_time_(retry_time) {}

int Client::testFile(const char *remote) const 
{
	if (remote == NULL)
	{
		return -1;
	}

	std::ostringstream oss;
	oss << hp_path_ << " -test -e " << remote << " 2>/dev/null 1>/dev/null; echo $?";
	std::string cmd = oss.str();
	return shell_call(cmd.c_str(), retry_time_, true);
}

int Client::uploadFile(const char *local, const char *remote) const
{
	std::ostringstream oss;
	oss << hp_path_ << " -copyFromLocal " << local << " " << remote << " 2>/dev/null 1>/dev/null; echo $?";
	std::string cmd = oss.str();
	return shell_call(cmd.c_str(), retry_time_);
}

int Client::downloadFile(const char *remote, const char *local) const
{
	std::ostringstream oss;
	oss << hp_path_ << " -copyToLocal " << remote << " " << local << " 2>/dev/null 1 >/dev/null; echo $?";
	std::string cmd = oss.str();
	return shell_call(cmd.c_str(), retry_time_);
}

int Client::removeFile(const char *remote) const
{
	std::ostringstream oss;
	oss << hp_path_ << " -rmr " << remote << " 2>/dev/null 1>/dev/null; echo $?";
	std::string cmd = oss.str();
	return shell_call(cmd.c_str(), retry_time_);
}


