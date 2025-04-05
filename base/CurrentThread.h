/**
  ******************************************************************************
  * @file           : CurrentThread.h
  * @author         : xy
  * @brief          : 
  * @attention      : 提供 Thread ID 的缓存机制，以提高性能，因为 syscall(SYS_gettid) 是系统调用，会从用户态进入内核态，再从内核态回到用户态
  * @date           : 2025/2/15
  ******************************************************************************
  */
#ifndef TINYNET_BASE_CURRENTTHREAD_H_
#define TINYNET_BASE_CURRENTTHREAD_H_

#include <unistd.h>
#include <sys/syscall.h>



namespace xy::CurrentThread
{

extern __thread int t_cachedTid;	// __thread ，每个线程都会有一个独立的副本，不会存在数据竞争问题

void cacheTid();

inline int tid() {
	if (__builtin_expect(t_cachedTid == 0, 0)) {
		cacheTid();
	}
	return t_cachedTid;
}
}

// xy

#endif //TINYNET_BASE_CURRENTTHREAD_H_
