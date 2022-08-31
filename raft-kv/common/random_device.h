/*
 * @Descripttion: 
 * @version: 
 * @Author: xuboluo
 * @Date: 2022-08-03 16:24:10
 * @LastEditors: xuboluo
 * @LastEditTime: 2022-08-05 21:34:19
 */
#pragma once
#include <random>

namespace kv {

class RandomDevice {
 public:
  explicit RandomDevice(uint32_t min, std::uint32_t max)
      : gen_(rd_()),
        distribution_(min, max) {

  }

  uint32_t gen();

 private:
  std::random_device rd_; 
  std::mt19937 gen_; // 随机数生层器
  std::uniform_int_distribution<> distribution_; // 生成随机数
};

}
