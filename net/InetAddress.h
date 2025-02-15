/**
  ******************************************************************************
  * @file           : InetAddress.h
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/7
  ******************************************************************************
  */
#ifndef TINYNET_NET_INETADDRESS_H_
#define TINYNET_NET_INETADDRESS_H_
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "base/Copyable.h"

namespace xy {

class InetAddress : public Copyable {
 public:
  InetAddress (const std::string &ip, uint16_t port);
  explicit InetAddress (const struct sockaddr_in &addr);
  std::string toIp () const;
  std::string toIpPort () const;
  uint16_t toPort () const;
  const sockaddr_in* getSockAddr() const;
  void setSockAddr(const struct sockaddr_in& addr);
 private:
  struct sockaddr_in addr_{};
};

} // xy

#endif //TINYNET_NET_INETADDRESS_H_
