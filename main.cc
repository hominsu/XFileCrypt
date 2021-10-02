//
// Created by Homin Su on 2021/10/2.
//

#include <sstream>
#include <memory>
#include <memory_resource>

#include "x_thread_pool/x_thread_pool.h"
#include "task/read/read_task.h"
#include "task/crypt/crypt_task.h"

int main() {
  std::string password = "12345678";

  XThreadPool::Get()->Init();

  // 创建线程安全的内存池
  auto memory_resource = std::make_shared<std::pmr::synchronized_pool_resource>();

  auto read_task_1 = std::make_shared<ReadTask>();
  read_task_1->Init("../test/src/WechatIMG1.jpeg");
  read_task_1->set_memory_resource(memory_resource);

  auto crypt_task_1 = std::make_shared<CryptTask>();
  crypt_task_1->set_memory_resource(memory_resource);
  crypt_task_1->Init(password);
  crypt_task_1->set_is_encrypt(true);

  read_task_1->set_next(crypt_task_1);

  XThreadPool::Get()->AddTask(read_task_1);
  XThreadPool::Get()->AddTask(crypt_task_1);

  auto read_task_2 = std::make_shared<ReadTask>();
  read_task_2->Init("../test/src/test.txt");
  read_task_2->set_memory_resource(memory_resource);

  auto crypt_task_2 = std::make_shared<CryptTask>();
  crypt_task_2->set_memory_resource(memory_resource);
  crypt_task_2->Init(password);
  crypt_task_2->set_is_encrypt(true);

  read_task_2->set_next(crypt_task_2);

  XThreadPool::Get()->AddTask(read_task_2);
  XThreadPool::Get()->AddTask(crypt_task_2);

  read_task_1->get_return();
  read_task_2->get_return();

  return 0;
}