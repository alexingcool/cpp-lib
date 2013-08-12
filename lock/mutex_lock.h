#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__

#include <cassert>
#include "mutex_lock.h"

class MutexLock
{
public:
	MutexLock();
	~MutexLock();
	void lock();
	void unlock();
	bool tryLock();
private:
	pthread_mutex_t mutex_;
};

#endif
