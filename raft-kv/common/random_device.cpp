/*
 * @Descripttion: 
 * @version: 
 * @Author: xuboluo
 * @Date: 2022-08-03 16:24:10
 * @LastEditors: xuboluo
 * @LastEditTime: 2022-08-05 21:30:11
 */
#include <raft-kv/common/random_device.h>

namespace kv {
uint32_t RandomDevice::gen() {
  return static_cast<uint32_t>(distribution_(gen_)); // 强制类型转换
}

}


