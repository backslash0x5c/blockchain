// Compile: gcc blockchain.c -o blockchain.exe -lssl -lcrypto
#include "blockchain.h"

// Blockchain *initializeBlockchain(void);
// Block *createBlock(const char *data, unsigned char *prevHash);
// void calculateHash(Block *block, unsigned char *hash);
// void addBlock(Blockchain *blockchain, Block *block);
// void freeBlockchain(Blockchain *blockchain);

// void printHash(const unsigned char *hash, int length);
// void printBlockchain(Blockchain *blockchain);

// int main(void) {
//   Blockchain *blockchain = initializeBlockchain();

//   Block *genesisBlock = createBlock("Genesis Block", "");
//   addBlock(blockchain, genesisBlock);

//   Block *block1 = createBlock("Data 1", blockchain->head->hash);
//   addBlock(blockchain, block1);

//   Block *block2 = createBlock("Data 2", blockchain->head->hash);
//   addBlock(blockchain, block2);

//   printBlockchain(blockchain);
//   freeBlockchain(blockchain);

//   return 0;
// }

// initializeBlockchain initializes a blockchain.
Blockchain *initializeBlockchain(void) {
  Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
  blockchain->head = NULL;
  blockchain->length = 0;

  return blockchain;
}

// createBlock creates a block.
Block *createBlock(const char *data, unsigned char *prevHash) {
  Block *block = (Block *)malloc(sizeof(Block));

  block->timestamp = time(NULL);
  block->data = data;
  memcpy(block->prevHash, prevHash, SHA256_DIGEST_LENGTH);
  block->next = NULL;

  calculateHash(block, block->hash);
  return block;
}

// calculateHash calculates a hash value of a block.
void calculateHash(Block *block, unsigned char *hash) {
  // header = timestamp + data + previousHash
  char header[sizeof(block->timestamp) + sizeof(block->data) +
              SHA256_DIGEST_LENGTH];
  memcpy(header, &(block->timestamp), sizeof(block->timestamp));
  memcpy(header + sizeof(block->timestamp), block->data, sizeof(*(block->data)));
  memcpy(header + sizeof(block->timestamp) + sizeof(block->data),
         block->prevHash, SHA256_DIGEST_LENGTH);

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, header, sizeof(header));
  SHA256_Final(hash, &sha256);
}

// addBlock adds a block to a blockchain.
void addBlock(Blockchain *blockchain, Block *block) {
  block->next = blockchain->head;
  blockchain->head = block;
  blockchain->length++;
}

// printHash prints a hash value.
void printHash(const unsigned char *hash, int length) {
  for (int i = 0; i < length; i++) {
    printf("%02x", hash[i]);
  }
  printf("\n");
}

// printBlockchain prints a blockchain.
void printBlockchain(Blockchain *blockchain) {
  Block *currentBlock = blockchain->head;
  while (currentBlock != NULL) {
    printf("Timestamp: %s", ctime(&(currentBlock->timestamp)));
    printf("Data: %s\n", currentBlock->data);
    printf("Previous Hash: ");
    printHash(currentBlock->prevHash, SHA256_DIGEST_LENGTH);
    printf("Hash: ");
    printHash(currentBlock->hash, SHA256_DIGEST_LENGTH);
    printf("\n");

    currentBlock = currentBlock->next;
  }
}

// freeBlockchain frees a blockchain.
void freeBlockchain(Blockchain *blockchain) {
  Block *currentBlock = blockchain->head;
  Block *nextBlock;
  while (currentBlock != NULL) {
    nextBlock = currentBlock->next;
    free(currentBlock);
    currentBlock = nextBlock;
  }
  free(blockchain);
}
