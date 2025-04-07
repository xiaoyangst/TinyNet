/**
  ******************************************************************************
  * @file           : CallBacks.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#ifndef TINYNET_NET_CALLBACKS_H_
#define TINYNET_NET_CALLBACKS_H_

#include <memory>
#include <functional>

namespace xy {

class TcpConnection;
class Buffer;
class Timestamp;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;
using MessageCallback = std::function<void(const TcpConnectionPtr &,
										   Buffer *,
										   Timestamp)>;

}
#endif //TINYNET_NET_CALLBACKS_H_
