#include "InetAddress.h"

namespace xy {
InetAddress::InetAddress(const std::string &ip, uint16_t port) {
	bzero(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const sockaddr_in &addr)
	: addr_(addr) {}

std::string
InetAddress::toIp() const {
	return {inet_ntoa(addr_.sin_addr)};
}

uint16_t
InetAddress::toPort() const {
	return ntohs(addr_.sin_port);
}

std::string
InetAddress::toIpPort() const {
	std::string port = std::to_string(toPort());
	return toIp() + ":" + port;
}

const sockaddr_in *
InetAddress::getSockAddr() const {
	return &addr_;
}

void InetAddress::setSockAddr(const struct sockaddr_in &addr) { addr_ = addr; }
} // xy