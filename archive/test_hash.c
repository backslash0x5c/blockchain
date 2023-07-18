#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

// ハッシュ値を16進数文字列に変換して表示する関数
void printHash(unsigned char* hash, int length) {
  for (int i = 0; i < length; i++) {
    printf("%02x", hash[i]);
  }
  printf("\n");
}

// 使用例
int main(void) {
  char data[] = "Hello, world!";

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char*)data, strlen(data), hash);

  printf("Hash: ");
  printHash(hash, SHA256_DIGEST_LENGTH);

  return 0;
}
