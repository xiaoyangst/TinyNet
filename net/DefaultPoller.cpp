#include "Poller.h"
#include "EpollPoller.h"

#include <cstdlib>

namespace xy {
Poller *Poller::newDefaultPoller(EventLoop *loop) {
	if (::getenv("MUDUO_USE_POLL")) {
		return nullptr; // 生成poll的实例
	} else {
		return new EpollPoller(loop); // 生成epoll的实例
	}
}
}
