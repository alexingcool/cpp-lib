#include <cassert>
#include "motex_lock.h"

MutexLock::MutexLock()
{
	int ret = pthread_mutex_init(&mutex_, NULL);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_mutex_init(%p) failed(%s)", &mutex_, strerror(ret));
	}
}

MutexLock::~MutexLock()
{
	int ret = pthread_mutex_destroy(&mutex_);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_mutex_destroy(%p) failed(%s)", &mutex_, strerror(ret));
	}
}

MutexLock::lock()
{
	int ret = pthread_mutex_unlock(&mutex_);
	if (ret < 0)
	{
		fprintf(stderr, "pthread_mutex_unlock(%p) failed(%s)", &mutex_, strerror(ret));
	}
}

bool tryLock()
{
	int ret = pthread_mutex_trylock(&mutex_);
	if (ret < 0)
	{
		frpintf(stderr, "pthread_mutex_trylock(%p) failed(%s)", &mutex_, strerror(ret));
	}

	if (ret == EBUSY)
	{
		return false;
	}
	return true;
}

