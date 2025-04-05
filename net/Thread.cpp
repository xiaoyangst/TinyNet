/**
  ******************************************************************************
  * @file           : Thread.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#include "Thread.h"

Thread::Thread(Thread::Func func, const std::string &name)
	: func_(func), name_(name) {}

void Thread::start() {
	if (func_) {
		thread_ = std::thread(func_);
	}
}

void Thread::stop() {
	if (thread_.joinable()) {
		thread_.join();
	}
}