//
// Created by Homin Su on 2021/10/2.
//

#include "task_base.h"

/**
 * @brief 设置内存池
 * @param _memory_resource 内存池
 */
void TaskBase::set_memory_resource(std::shared_ptr<std::pmr::memory_resource> _memory_resource) {
  memory_resource_ = std::move(_memory_resource);
}

/**
 * @brief 设置责任链的下一个节点
 * @param _next 责任链的下一个节点
 */
void TaskBase::set_next(std::shared_ptr<TaskBase> _next) {
  next_ = std::move(_next);
}

/**
 * @brief 给对象传递数据，线程安全
 * @param _data 数据块地址
 */
void TaskBase::PushBack(std::shared_ptr<Data> &_data) {
  // 独占锁
  std::unique_lock<std::shared_mutex> lock(mutex_);
  datas_.push_back(_data);
  // 考虑最大列表的问题
}

/**
 * @brief 弹出数据
 * @return 数据块所在地址
 */
std::shared_ptr<Data> TaskBase::PopFront() {
  // 独占锁
  std::unique_lock<std::shared_mutex> lock(mutex_);

  if (datas_.empty()) {
    return nullptr;
  }

  auto ret = datas_.front();
  datas_.pop_front();

  return ret;
}
