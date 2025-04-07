/**
  ******************************************************************************
  * @file           : Buffer.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#ifndef TINYNET_NET_BUFFER_H_
#define TINYNET_NET_BUFFER_H_

#include <vector>
#include <string>

namespace xy {

class Buffer {
 public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;

  explicit Buffer(size_t initialSize = kInitialSize)
	  : buffer_(kCheapPrepend + initialSize), readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend) {}

  size_t readableBytes() const { return writerIndex_ - readerIndex_; }

  size_t writableBytes() const { return buffer_.size() - writerIndex_; }

  size_t prependableBytes() const { return readerIndex_; }

  // 可读数据起始位置
  const char *peek() const {
	  return begin() + readerIndex_;
  }

  std::string toString(size_t len) {
	  std::string result(peek(), len);
	  retrieve(len);    // 把数据读取出来，并转换为字符串，就得更新可读索引
	  return result;
  }

  // 读取指定长度之后，更新读索引
  void retrieve(size_t len) {
	  if (len < readableBytes()) {
		  readerIndex_ += len;
	  } else {
		  retrieveAll();
	  }
  }

  // 读取所有数据，更新读索引和写索引
  void retrieveAll() {
	  readerIndex_ = kCheapPrepend;
	  writerIndex_ = kCheapPrepend;
  }

  void ensureWritableBytes(size_t len){
	  if (writableBytes() < len){	// 可写数据长度小于 len，需要扩容
		  // 扩容
		  makeSpace(len);
	  }
  }

  void append(const char* data, size_t len){
	  // 确保可写数据长度大于 len
	  ensureWritableBytes(len);
	  std::copy(data, data + len, begin() + writerIndex_);	// 拷贝数据到缓冲区
	  writerIndex_ += len;	// 更新写索引
  }

  ssize_t readFd(int fd, int* savedErrno);
  ssize_t writeFd(int fd,int* saveErrno);

 private:
  void makeSpace(int len){
	  auto freeSpace = writableBytes() + prependableBytes();
	  if (freeSpace < len + kCheapPrepend){
		  buffer_.resize(writerIndex_ + len);
	  } else {
		  size_t readable = readableBytes();	// 可读区域长度
		  std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);	// 拷贝可读数据到缓冲区开头
		  readerIndex_ = kCheapPrepend;
		  writerIndex_ = readerIndex_ + readable;
	  }
  }

 private:
  char *begin() {
	  return buffer_.data();
  }

  const char *begin() const {
	  return buffer_.data();
  }

 private:
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;

};
}
#endif //TINYNET_NET_BUFFER_H_
