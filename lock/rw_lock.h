#ifndef __RW_LOCK_H__
#define __RW_LOCK_H__

#include <sys/types.h>
#include <pthread.h>

class RWLock
{
public:
	RWLock();
	~RWLock();
	void lockForRead();
	void lockForWrite();
	void unlock();
	
private:
	pthread_rwlock_t lock_;
};

#endif
