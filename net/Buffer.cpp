/**
  ******************************************************************************
  * @file           : Buffer.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#include "Buffer.h"
#include <errno.h>
#include <sys/uio.h>
#include <execution>
namespace xy {

ssize_t Buffer::readFd(int fd, int *savedErrno) {
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writable = writableBytes();
	vec[0].iov_base = begin()+writerIndex_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;
	const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
	const ssize_t n = ::readv(fd, vec, iovcnt);
	if (n < 0)
	{
		*savedErrno = errno;
	}
	else if (static_cast<size_t>(n) <= writable)	// 从 fd 读取到的数据长度 ，Buffer的可写区域足够
	{
		writerIndex_ += n;
	}
	else	// 从 fd 读取到的数据长度 ，Buffer的可写区域不够，将 extrabuf 临时存储的数据写入到 Buffer 中，append 会自动考虑扩容
	{
		writerIndex_ = buffer_.size();
		append(extrabuf, n - writable);
	}
	return n;
}

ssize_t Buffer::writeFd(int fd, int *saveErrno) {
	ssize_t  n = ::write(fd,peek(),readableBytes());
	if (n < 0){
		*saveErrno = errno;
	}
	return n;
}

}