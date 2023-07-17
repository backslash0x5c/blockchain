#include <arpa/inet.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

/* from blockchain.c */
typedef struct Block {
  time_t timestamp;
  const char *data;
  unsigned char previousHash[SHA256_DIGEST_LENGTH];
  unsigned char hash[SHA256_DIGEST_LENGTH];
  struct Block *next;
} Block;

typedef struct {
  Block *head;
  int length;
} Blockchain;

extern *initializeBlockchain(void);
extern *createBlock(const char *data, unsigned char *previousHash);
extern void calculateHash(Block *block, unsigned char *hash);
extern void addBlock(Blockchain *blockchain, Block *block);
extern void freeBlockchain(Blockchain *blockchain);

extern void printHash(const unsigned char *hash, int length);
extern void printBlockchain(Blockchain *blockchain);

/* from server.c */
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 1024
#define QUEUE_SIZE 5

extern void handleClient(int clientSocket, Blockchain *blockchain);
extern void sendBlockAddedMessage(int clientSocket, Block *block);
extern void sendBlockchainInfo(int clientSocket, Blockchain *blockchain);

/* from client.c */
extern void showMenu();