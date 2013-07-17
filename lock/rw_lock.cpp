#include <cassert>
#include "rw_lock.h"

RWLock::RwLock()
{
	int ret = pthread_rwlock_init(&lock_, NULL);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_rwlock_init(%p) failed(%s)", &lock_, SERRNO2(ret));
	}
}

RWLock::~RWLock()
{
	int ret = pthread_rwlock_destroy(&lock_);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_rwlock_destroy(%p) failed(%s)", &lock_, SERRNO2(ret));
	}
}

void RWLock::lockForRead()
{
	int ret = pthread_rwlock_rdlock(&lock_);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_rwlock_rdlock(%p) failed(%s)", &lock_, SERRNO2(ret));
	}
}

void RWLock::lockForWrite()
{
	int ret = pthread_rwlock_wrlock(&lock_);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_rwlock_wrlock(%p) failed(%s)", &lock_, SERRNO2(ret));
	}
}

void RWLock::unlock()
{
	int ret = pthread_rwlock_unlock(&lock_);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_rwlock_unlock(%p) failed(%s)", &lock_, SERRNO2(ret));
	}
}

