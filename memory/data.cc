//
// Created by Homin Su on 2021/10/2.
//

#ifdef Debug
#include <iostream>
#elif Release
#include <stdexcept>
#endif

#include "data.h"

Data::Data() = default;

Data::~Data() {
  // 内存池未定义或没有数据就不用释放
  if (nullptr == memory_resource_ || nullptr == data_) {
    return;
  }

  // 释放分配的内存
  memory_resource_->deallocate(data_, memory_size_);
  data_ = nullptr;
  memory_size_ = 0;
  size_ = 0;
}

/**
 * @brief 创建 Data 对象
 * @param _memory_resource 内存池对象
 * @return Data 的智能指针对象
 */
std::shared_ptr<Data> Data::Make(std::shared_ptr<std::pmr::memory_resource> _memory_resource) {
  // 创建 Data 的智能指针对象
  std::shared_ptr<Data> data_ptr(new Data());
  data_ptr->memory_resource_ = std::move(_memory_resource);

  return data_ptr;
}

/**
 * @brief 创建内存空间
 * @param _memory_size 占用内存字节数
 * @return 创建的内存空间的指针，创建失败为空 nullptr
 */
void *Data::New(size_t _memory_size) {

  // 异常处理
  if (_memory_size <= 0) {
#ifdef Debug
    std::cerr << "Data::New _memory_size <= 0" << std::endl;
    return nullptr;
#elif Release
    throw std::runtime_error("Data::New _memory_size <= 0");
#endif
  }

  // 异常处理
  if (nullptr == memory_resource_) {
    return nullptr;
  }

  // 申请空间
  data_ = memory_resource_->allocate(_memory_size);

  memory_size_ = _memory_size;
  size_ = _memory_size;

  return data_;
}

/**
 * @brief 获取数据块的指针
 * @return 数据块的指针
 */
void *Data::data() const {
  return data_;
}

/**
 * @brief 获取实际数据的字节数
 * @return 实际数据的字节数
 */
size_t Data::size() const {
  return size_;
}

/**
 * @brief 设置实际数据字节数
 * @param size 实际数据字节数
 */
void Data::set_size(size_t _size) {
  size_ = _size;
}

/**
 * @brief 获取分配的内存大小
 * @return size_t 分配的内存大小
 */
size_t Data::memory_size() const {
  return memory_size_;
}

/**
 * @brief 是否是文件结尾
 * @return true or false
 */
bool Data::end() const {
  return end_;
}

/**
 * @brief 设置为文件结尾
 * @param _end true or false
 */
void Data::set_end(bool _end) {
  end_ = _end;
}
