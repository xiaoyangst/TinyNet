#include "EventLoop.h"
#include "Poller.h"
#include <memory>

namespace xy {
__thread EventLoop *t_loopInThisThread = nullptr;    // 防止一个线程创建多个 EventLoop
const int kPollTimeMs = 10000;

int createEventfd() {
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		LOG_ERROR("Failed in eventfd\n");
		abort();
	}
	return evtfd;
}

EventLoop::EventLoop()
	: threadId_(CurrentThread::tid()),
	  looping_(false),
	  quit_(false),
	  callingPendingFunctors_(false),
	  poller_(Poller::newDefaultPoller(this)),
	  wakeupFd_(createEventfd()),
	  wakeupChannel_(new Channel(this, wakeupFd_)) {
	LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
	if (t_loopInThisThread) {
		LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
	} else {
		t_loopInThisThread = this;
	}

	// 设置wakeupfd的事件类型以及发生事件后的回调操作
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	// 每一个eventloop都将监听wakeupchannel的EPOLLIN读事件了
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread = nullptr;
}

// 存在阻塞
void EventLoop::loop()    // 开启事件循环
{
	looping_ = true;
	quit_ = false;

	while (!quit_) {
		activeChannels_.clear();
		// 监听 client_fd(客户端连接和通信达的 fd) 和 weak_up_fd（mainLoop 唤醒 subLoop 的 fd)
		pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);    // 执行 epoll_wait
		// poller_->poll 之后，activeChannels_ 就被填充记录发送事件的 channel 集合
		for (Channel *channel : activeChannels_) {
			channel->handleEvent(pollReturnTime_);    // 执行发生的事件
		}
		doPendingFunctors();
	}

	looping_ = false;
}

void EventLoop::doPendingFunctors() {
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;

	{
		std::lock_guard<std::mutex> mtx(pend_mtx_);
		functors.swap(pendingFunctors_);    // queueInLoop 会添加回调函数到 pendingFunctors_ 中
	}

	for (const Functor &functor : functors) {
		functor();
	}
	callingPendingFunctors_ = false;
}

bool EventLoop::isInLoopThread() const {
	return threadId_ == CurrentThread::tid();
}

void EventLoop::runInLoop(Functor cb) {
	if (isInLoopThread())    // 是在当前线程的当前 Loop 中
	{
		cb();
	} else {
		queueInLoop(std::move(cb));
	}
}

void EventLoop::queueInLoop(Functor cb) {
	{
		std::lock_guard<std::mutex> mtx(pend_mtx_);
		pendingFunctors_.push_back(std::move(cb));
	}

	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();    // 唤醒
	}
}


void EventLoop::quit() {    // 退出事件循环
	quit_ = true;
	if (!isInLoopThread())    // 不是在当前线程中被调用的，意味着 loop 还在阻塞中，因此执行唤醒操作，来接触 loop 中的阻塞
	{
		wakeup();    // eventfd --> write
	}
}

// epoll 监听 eventfd，如果 write，epoll_wait 就会解除阻塞
// 因此执行 wakeup 就会触发事件，loop 函数接触阻塞，执行 handleRead 回调
void EventLoop::wakeup() {	// eventfd --> write
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof one);	// 不会阻塞
	if (n != sizeof one) {
		LOG_ERROR("EventLoop::wakeup() writes %zd bytes instead of 8", n);
	}
}

void EventLoop::handleRead() {	// eventfd --> read
	uint64_t one = 1;
	ssize_t n = read(wakeupFd_, &one, sizeof one);	// 会阻塞，除非此前有 write
	if (n != sizeof one) {
		LOG_ERROR("EventLoop::handleRead() reads %zd bytes instead of 8\n", n);
	}
}


void EventLoop::updateChannel(Channel *channel) {
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
	poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
	return poller_->hasChannel(channel);
}

} // xy