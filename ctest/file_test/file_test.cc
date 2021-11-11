//
// Created by Homin Su on 2021/11/3.
//

#include <iostream>
#include <fstream>
#include <filesystem>

#include "../../task/file_crypt.h"

int main(int _argc, char *_argv[]) {
  {
    std::filesystem::create_directories("test/src");

    std::ofstream ofs;
    ofs.open("test/src/test.txt", std::ios::out);

    std::string in_str(_argv[2]);
    ofs.write(in_str.c_str(), in_str.length());
    ofs.close();
  }

  {
    std::filesystem::create_directories("test/enc");

    // 线程安全的内存资源
    auto memory_resource = std::make_shared<std::pmr::synchronized_pool_resource>();

    auto file_crypt_encrypt = std::make_shared<FileCrypt>();
    file_crypt_encrypt->Start("test/src/test.txt",
                              "test/enc/test.txt",
                              _argv[1],
                              true,
                              memory_resource);
    file_crypt_encrypt->Wait();
  }

  {
    std::filesystem::create_directories("test/dec");

    // 线程安全的内存资源
    auto memory_resource = std::make_shared<std::pmr::synchronized_pool_resource>();

    auto file_crypt_decrypt = std::make_shared<FileCrypt>();
    file_crypt_decrypt->Start("test/enc/test.txt",
                              "test/dec/test.txt",
                              _argv[1],
                              false,
                              memory_resource);
    file_crypt_decrypt->Wait();
  }

  {
    char buf[1024]{'\0'};

    std::ifstream ifs;
    ifs.open("test/dec/test.txt", std::ios::in);
    ifs.read(buf, 1024);
    ifs.close();

    std::cout << buf << std::endl;
  }

  {
    std::filesystem::remove_all("test");
  }

  return 0;
}

