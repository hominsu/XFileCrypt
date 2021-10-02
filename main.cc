//
// Created by Homin Su on 2021/10/2.
//

#include <sstream>
#include <memory>
#include <memory_resource>

#include "x_thread_pool/x_thread_pool.h"
#include "task/read_task.h"
#include "task/crypt_task.h"
#include "task/write_task.h"

int main() {
  std::string password = "12345678";

  XThreadPool::Get()->Init();

  // 加密
  {
    // 创建线程安全的内存池
    auto memory_resource = std::make_shared<std::pmr::synchronized_pool_resource>();

    // 读取任务
    auto read_task = std::make_shared<ReadTask>();
    read_task->set_memory_resource(memory_resource);
    read_task->Init("../test/src/WechatIMG1.jpeg");

    // 加密任务
    auto crypt_task = std::make_shared<CryptTask>();
    crypt_task->set_memory_resource(memory_resource);
    crypt_task->Init(password);
    crypt_task->set_is_encrypt(true);

    // 写出任务
    auto write_task = std::make_shared<WriteTask>();
    write_task->set_memory_resource(memory_resource);
    write_task->Init("../test/dec/WechatIMG1.crypt");

    // 设置责任链: read_task -> crypt_task -> write_task
    read_task->set_next(crypt_task);
    crypt_task->set_next(write_task);

    // 任务加入线程池
    XThreadPool::Get()->AddTask(read_task);
    XThreadPool::Get()->AddTask(crypt_task);
    XThreadPool::Get()->AddTask(write_task);

    // 等待任务执行完成
    read_task->get_return();
    crypt_task->get_return();
    write_task->get_return();
  }

  // 解密
  {
    // 创建线程安全的内存池
    auto memory_resource = std::make_shared<std::pmr::synchronized_pool_resource>();

    // 读取任务
    auto read_task = std::make_shared<ReadTask>();
    read_task->set_memory_resource(memory_resource);
    read_task->Init("../test/dec/WechatIMG1.crypt");

    // 解密任务
    auto crypt_task = std::make_shared<CryptTask>();
    crypt_task->set_memory_resource(memory_resource);
    crypt_task->Init(password);
    crypt_task->set_is_encrypt(false);

    // 写出任务
    auto write_task = std::make_shared<WriteTask>();
    write_task->set_memory_resource(memory_resource);
    write_task->Init("../test/enc/WechatIMG1.jpeg");

    // 设置责任链: read_task -> crypt_task -> write_task
    read_task->set_next(crypt_task);
    crypt_task->set_next(write_task);

    // 任务加入线程池
    XThreadPool::Get()->AddTask(read_task);
    XThreadPool::Get()->AddTask(crypt_task);
    XThreadPool::Get()->AddTask(write_task);

    // 等待任务执行完成
    read_task->get_return();
    crypt_task->get_return();
    write_task->get_return();
  }

  return 0;
}