/**
  ******************************************************************************
  * @file           : ${filename}
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/3
  ******************************************************************************
  */
#ifndef TINYNET_BASE_TIMESTAMP_H_
#define TINYNET_BASE_TIMESTAMP_H_
#include <iostream>
#include <string>

namespace xy {
class Timestamp {
 public:
  Timestamp ();
  explicit Timestamp (int64_t microSecondsSinceEpoch);
  static Timestamp now ();
  std::string toString () const;
 private:
  int64_t microSecondsSinceEpoch_;
};
}
#endif //TINYNET_BASE_TIMESTAMP_H_
