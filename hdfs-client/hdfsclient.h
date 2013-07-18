#ifndef __HDFS_CLIENT_H__
#define __HDFS_CLIENT_H__

class Client
{
public:
	Client(const char *hp_path, int retry_time);
	int testFile(const char *remote) const;
	int uploadFile(const char *local, const char *remote) const;
	int downloadFile(const char *remote, const char *local) const;
	int removeFile(const char *remote) const;
	int makeDir(const char *dir) const;
private:
	std::string hp_path_;
	int retry_time_;
}

#endif
