//
// Created by Homin Su on 2021/10/2.
//

#include <iostream>
#include <string>
#include <list>
#include <filesystem>

#include "task/file_crypt.h"

int main(int _argc, char *_argv[]) {
  if (_argc != 5) {
    std::cerr << "para error!" << std::endl;
    std::cout << "\tEncrypt folder: " << _argv[0] << " -e src_dir dst_dir password" << std::endl;
    std::cout << "\tDecrypt folder: " << _argv[0] << " -d src_dir dst_dir password" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string option = _argv[1];    // 加解密选项
  std::string in_file = _argv[2];   // 输入文件夹
  std::string out_file = _argv[3];  // 输入文件夹
  std::string password = _argv[4];  // 密码

  bool is_encrypt{};
  if ("-e" == option) {
    is_encrypt = true;
  } else if ("-d" == option) {
    is_encrypt = false;
  }

  // 创建输出文件夹
  std::filesystem::create_directories(out_file);

  std::list<std::shared_ptr<FileCrypt>> file_crypt_list;

  // 遍历输入目录
  for (auto &it: std::filesystem::directory_iterator(in_file)) {
    // 只处理文件
    if (!it.is_regular_file()) {
      continue;
    }

    auto file_crypt = std::make_shared<FileCrypt>();

    file_crypt->Start(it.path().string(),
                      out_file + "/" + it.path().filename().string(),
                      password,
                      is_encrypt);

    file_crypt_list.push_back(file_crypt);
  }

  // 等待任务执行完成
  for (auto &file_crypt: file_crypt_list) {
    file_crypt->Wait();
  }

  return 0;
}

// 加密: build/XFileCrypt -e test/src test/enc 123456
// 解密: build/XFileCrypt -d test/enc test/dec 123456