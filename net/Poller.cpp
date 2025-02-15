#include "Poller.h"

namespace xy {
Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {
}

bool Poller::hasChannel(xy::Channel *channel) const {
	auto chs = channels_.find(channel->fd());
	return chs != channels_.end() && chs->second == channel;
}
} // xy