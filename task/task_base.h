//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_TASK_TASK_BASE_H_
#define XFILECRYPT_TASK_TASK_BASE_H_

#include <list>
#include <memory>
#include <memory_resource>

#include "../x_thread_pool/x_task.h"

class Data;

/**
 * @brief 任务基类
 */
class TaskBase : public XTask<int> {
 protected:
  size_t data_bytes_{}; ///< 处理的数据的字节数
  std::shared_ptr<std::pmr::memory_resource> memory_resource_;  ///< 内存池
  std::shared_ptr<TaskBase> next_; ///< 责任链

 private:
  std::list<std::shared_ptr<Data>> datas_;  ///< 数据块
  mutable std::shared_mutex mutex_;

 public:
  /**
   * @brief 设置内存池
   * @param _memory_resource 内存池
   */
  void set_memory_resource(std::shared_ptr<std::pmr::memory_resource> _memory_resource);

  /**
   * @brief 设置责任链的下一个节点
   * @param _next 责任链的下一个节点
   */
  void set_next(std::shared_ptr<TaskBase> _next);

  /**
   * @brief 给对象传递数据，线程安全
   * @param _data 数据块地址
   */
  void PushBack(std::shared_ptr<Data> &_data);

  /**
   * @brief 弹出数据
   * @return 数据块所在地址
   */
  std::shared_ptr<Data> PopFront();

 protected:
  /**
   * @brief 线程入口函数
   */
  void Main() override = 0;
};

#endif //XFILECRYPT_TASK_TASK_BASE_H_
