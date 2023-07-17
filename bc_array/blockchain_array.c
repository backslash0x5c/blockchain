// Compile: gcc blockchain.c -o blockchain.exe -lcrypto
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CHAIN_LENGTH 100

// ブロックの構造体
typedef struct {
  int index;
  time_t timestamp;
  char* data;
  unsigned char previousHash[SHA256_DIGEST_LENGTH];
  unsigned char hash[SHA256_DIGEST_LENGTH];
  int nonce;
} Block;

// ブロックチェーンの構造体
typedef struct {
  Block* chain[MAX_CHAIN_LENGTH];
  int length;
} Blockchain;

Blockchain* initializeBlockchain(void);
Block* createBlock(int index, char* data, unsigned char* previousHash);
void calculateHash(Block* block, unsigned char* hash);

void mineBlock(Block* block, int difficulty);
void isBlockValid(Block* block, Block* previousBlock, int difficulty);

void addBlock(Blockchain* blockchain, Block* block);
void printHash(unsigned char* hash, int length);
void printBlockchain(Blockchain* blockchain);

int main(void) {
  // ブロックチェーンの初期化
  Blockchain* blockchain = initializeBlockchain();

  // 新しいブロックの作成と追加
  Block* block1 = createBlock(1, "Data 1", blockchain->chain[blockchain->length - 1]->hash);
  mineBlock(block1, 2);
  addBlock(blockchain, block1);

  Block* block2 = createBlock(2, "Data 2", blockchain->chain[blockchain->length - 1]->hash);
  mineBlock(block2, 2);
  addBlock(blockchain, block2);

  // ブロックチェーンの表示
  printBlockchain(blockchain);

  // メモリの解放
  for (int i = 0; i < blockchain->length; i++) {
    free(blockchain->chain[i]);
  }
  free(blockchain);

  return 0;
}

// ブロックチェーンを初期化する関数
Blockchain* initializeBlockchain(void) {
  Blockchain* blockchain = (Blockchain*)malloc(sizeof(Blockchain));
  blockchain->length = 0;

  // ジェネシスブロックの作成
  Block* genesisBlock = createBlock(0, "Genesis Block", "");
  addBlock(blockchain, genesisBlock);

  return blockchain;
}

// 新しいブロックを作成する関数
Block* createBlock(int index, char* data, unsigned char* previousHash) {
  Block* block = (Block*)malloc(sizeof(Block));
  block->index = index;
  block->timestamp = time(NULL);
  block->data = data;
  block->nonce = 0;

  memcpy(block->previousHash, previousHash, SHA256_DIGEST_LENGTH);
  calculateHash(block, block->hash);
  return block;
}

// ブロックのハッシュを計算する関数
void calculateHash(Block* block, unsigned char* hash) {
  char header[SHA256_DIGEST_LENGTH + sizeof(block->timestamp) + sizeof(block->data) + sizeof(block->nonce)];
  // header = timestamp + data + previousHash + nonce
  memcpy(header, &(block->timestamp), sizeof(block->timestamp));  // ブロックのタイムスタンプをヘッダーにコピー
  memcpy(header + sizeof(block->timestamp), block->data, sizeof(block->data)); // ブロックのデータをヘッダーにコピー
  memcpy(header + sizeof(block->timestamp) + sizeof(block->data), block->previousHash, SHA256_DIGEST_LENGTH); // 前のブロックのハッシュをヘッダーにコピー
  memcpy(header + sizeof(block->timestamp) + sizeof(block->data) + SHA256_DIGEST_LENGTH, &(block->nonce), sizeof(block->nonce)); // ブロックのnonceをヘッダーにコピー

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, header, sizeof(header));
  SHA256_Final(hash, &sha256);
}

void mineBlock(Block* block, int difficulty) {
  char target[difficulty + 1];
  memset(target, '0', difficulty);
  target[difficulty] = '\0';

  while (strncmp((const char*)block->hash, target, difficulty) != 0) {
    block->nonce++;
    calculateHash(block, block->hash);
  }

  printf("Block mined: Nonce = %d\n", block->nonce);
}

int isBlockValid(Block* block, Block* previousBlock, int difficulty) {
  char target[difficulty + 1];
  memset(target, '0', difficulty);
  target[difficulty] = '\0';

  if (strncmp((const char*)block->previousHash,
              (const char*)previousBlock->hash, SHA256_DIGEST_LENGTH) != 0) {
    return 0;  // 前のブロックのハッシュ値が一致しない
  }

  if (strncmp((const char*)block->hash, target, difficulty) != 0) {
    return 0;  // マイニングの難易度を満たしていない
  }

  return 1;
}

// ブロックチェーンにブロックを追加する関数
void addBlock(Blockchain* blockchain, Block* block) {
  blockchain->chain[blockchain->length] = block;
  blockchain->length++;
}

// ハッシュ値を16進数文字列に変換して表示する関数
void printHash(unsigned char* hash, int length) {
  for (int i = 0; i < length; i++) {
    printf("%02x", hash[i]);
  }
  printf("\n");
}

// ブロックチェーン全体を表示する関数
void printBlockchain(Blockchain* blockchain) {
  Block* block;

  for (int i = 0; i < blockchain->length; i++) {
    block = blockchain->chain[i];
    printf("Block %d\n", block->index);
    printf("Timestamp: %s", ctime(&(block->timestamp)));
    printf("Data: %s\n", block->data);
    printf("Previous Hash: ");
    printHash(block->previousHash, SHA256_DIGEST_LENGTH);
    printf("Hash: ");
    printHash(block->hash, SHA256_DIGEST_LENGTH);
    printf("\n");
  }
}
