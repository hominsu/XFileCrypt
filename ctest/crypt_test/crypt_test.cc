//
// Created by Homin Su on 2021/10/28.
//

#include <iostream>
#include <memory>
#include <cstring>

#include "../../crypt/aes_cbc_crypt.h"

int main(int _argc, char *_argv[]) {
  size_t encrypt_size;

  char cipher_text[1024]{'\0'};
  char plain_text[1024]{'\0'};

  {
    AesCBCCrypt crypt;
    crypt.Init(_argv[1], true);

    std::string in_str(_argv[2]);

    encrypt_size = crypt.Encrypt(in_str.c_str(), in_str.size(), cipher_text, true);
    std::cout << "encrypt_size = " << encrypt_size << ", encrypt content = \"" << cipher_text << "\"" << std::endl;
  }

  {
    AesCBCCrypt crypt;
    crypt.Init(_argv[1], false);

    auto decrypt_size = crypt.Decrypt(cipher_text, encrypt_size, plain_text, true);
    std::cout << "decrypt_size = " << decrypt_size << ", decrypt content = \"" << plain_text << "\"" << std::endl;
  }


  return 0;
}
