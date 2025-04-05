/**
  ******************************************************************************
  * @file           : Socket.h
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/7
  ******************************************************************************
  */
#ifndef TINYNET_NET_SOCKET_H_
#define TINYNET_NET_SOCKET_H_
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include "base/Noncopyable.h"
#include "InetAddress.h"
#include "base/Log.h"
namespace xy {

class Socket : public Noncopyable {
 public:
  explicit Socket(int sockfd)
	  : sockfd_(sockfd) {}

  ~Socket();

  int fd() const { return sockfd_; }

  void bindAddress(const InetAddress &localaddr);
  void listen();
  int accept(InetAddress *peeraddr);

  void shutdownWrite();

  void setTcpNoDelay(bool on);
  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setKeepAlive(bool on);
 private:
  const int sockfd_;
};

} // xy

#endif //TINYNET_NET_SOCKET_H_
