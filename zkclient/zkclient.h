#ifndef __ZKCLIENT_H__
#define __ZKCLIENT_H__

#include <zookeeper.h>
#include <string>
#include <vector>

class Client {
	public:
		static const int kMaxDataSize = 64 * 1024; // 64K.
		static const int kMaxPathLen = 1024;
		static const int kConnTimeout = 10 *1000; // 10 secs.
		enum {
			kOK = 0,
			kNotConnected,
			kConnectTimeout,
			kConnectFailed,
			kPathNotFound,
			kReadFailed,
			kVersionNotMatch,
			kWriteFailed,
			kCreateFailed,
			kCreateNodeExists,
			kExistsFailed,
			kRemoveFailed,
		};
		static const char* code_to_string(int code);
		Client(const char* server, int conn_timeout = kConnTimeout);
		~Client();
		int connect();
		void disconnect();
		bool isConnected();
		int read(const char* path, std::string* data, int* version = NULL, int size = kMaxDataSize);
		int readChildren(const char* path, std::vector< std::string >* children);
		int write(const char* path, const std::string& data, int version = -1);
		int exists(const char* path);
		int createNode(const char* path, bool temporary = false, std::string* vpath = NULL);
		int createTree(const char* path);
		int removeNode(const char* path, int version = -1);
		int removeTree(const char* path);
	private:
		std::string server_;
		int conn_timeout_;
		zhandle_t* handler_;
};

