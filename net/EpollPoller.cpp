#include "EpollPoller.h"
#include "base/Log.h"
namespace xy {
const int kNew = -1;	// 未添加
const int kAdded = 1;	// 已添加
const int kDeleted = 2;	// 已删除

EpollPoller::EpollPoller(EventLoop *loop)
	: Poller(loop),
	  epfd_(::epoll_create1(EPOLL_CLOEXEC)),
	  events_(kInitEventListSize) {
	if (epfd_ < 0) {
		LOG_FATAL("epoll_create1 error:%d \n", errno);
	}
}

EpollPoller::~EpollPoller() {
	::close(epfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelVec *activeChannels) {
	int numEvents = ::epoll_wait(epfd_, events_.data(), events_.size(), timeoutMs);
	Timestamp now(Timestamp::now());
	int savedErrno = errno;
	if (numEvents > 0) {
		// 把触发的事件，填充到传出参数 activeChannels
		fillActiveChannels(numEvents, activeChannels);
		// 检查是否需要扩容
		if (numEvents == events_.size()) {
			events_.resize(numEvents*2);
		}
	} else if (numEvents == 0) {
		// 没有任何事件发生
		LOG_DEBUG("noting happened \n");
	} else {
		// 遇到错误
		if (savedErrno != EINTR) {
			errno = savedErrno;
			LOG_ERROR("EpollPoller::poll");
		}
	}
	return now;
}

void EpollPoller::updateChannel(Channel *channel) {
	const int index = channel->index();    // 代表 channel 的状态
	int fd = channel->fd();
	if (index == kNew || index == kDeleted) {
		if (index == kNew) {	// channels_ 没有记录，得添加一份
			channels_[fd] = channel;	// 记录所有注册的 channel（unordered_map）
		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	} else {
		if (channel->isNoneEvent()){	// channel 已被设置为对任何事件都不感兴趣，意味着可以从 epoll 中删除
			update(EPOLL_CTL_DEL,channel);
			channel->set_index(kDeleted);
		} else{	// 还有值得关注的事件，这里更新一下即可
			update(EPOLL_CTL_MOD,channel);
		}
	}
}

// 从 channels_ 中删除 channel
// 意味着 channel 从此不再被服务器关心
void EpollPoller::removeChannel(Channel *channel) {
	int fd = channel->fd();
	int index = channel->index();
	channels_.erase(fd);
	if (index == kAdded){
		update(EPOLL_CTL_DEL,channel);
	}
	channel->set_index(kNew);
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelVec *activeChannels) const {
	activeChannels->reserve(numEvents);
	for (int i = 0; i < numEvents; ++i) {
		// channel 保存在 ptr 成员中的，这里取出来
		auto channel = static_cast<Channel *>(events_[i].data.ptr);
		// 在 channel 中记录发生的类型类型
		channel->set_revents(events_[i].events);
		// 添加到 activeChannels 中
		activeChannels->push_back(channel);
	}
}


void EpollPoller::update(int operation, Channel *channel) {
	struct epoll_event event;
	memset(&event,0,sizeof(event));
	int fd = channel->fd();
	event.events = channel->events();
	event.data.fd = fd;
	event.data.ptr = channel;
	if(::epoll_ctl(epfd_,operation,fd,&event) < 0){
		if (operation == EPOLL_CTL_DEL){
			LOG_ERROR("update EPOLL_CTL_DEL err：%d\n",errno);
		} else{
			LOG_FATAL("update EPOLL_CTL_ADD/MOD err：%d\n",errno);
		}
	}
}
} // xy