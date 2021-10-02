//
// Created by Homin Su on 2021/10/2.
//

#include <iostream>
#include <sstream>
#include <memory>
#include <memory_resource>

#include "x_thread_pool/x_thread_pool.h"
#include "task/read/read_task.h"

int main() {
  XThreadPool::Get()->Init();

  // 创建线程安全的内存池
  auto memory_resource = std::make_shared<std::pmr::synchronized_pool_resource>();

  auto read_task_1 = std::make_shared<ReadTask>();
  read_task_1->Init("../test/src/WechatIMG1.jpeg");
  read_task_1->set_memory_resource(memory_resource);

  auto read_task_2 = std::make_shared<ReadTask>();
  read_task_2->Init("../test/src/test.txt");
  read_task_2->set_memory_resource(memory_resource);

  XThreadPool::Get()->AddTask(read_task_1);
  XThreadPool::Get()->AddTask(read_task_2);

  read_task_1->get_return();
  read_task_2->get_return();

  return 0;
}