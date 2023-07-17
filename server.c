#include "blockchain.h"

void handleClient(int clientSocket, Blockchain *blockchain);
void sendBlockAddedMessage(int clientSocket, Block *block);
void sendBlockchainInfo(int clientSocket, Blockchain *blockchain);

int main(void) {
  Blockchain *blockchain = initializeBlockchain();

  int serverSocket, clientSocket;
  struct sockaddr_in serverAddr, clientAddr;
  socklen_t clientAddrLen;

  // サーバソケットの作成
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    perror("socket");
    return -1;
  }

  memset(&serverAddr, 0, sizeof(struct sockaddr_in));

  // サーバアドレスの設定
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons((unsigned short)SERVER_PORT);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

  // サーバにアドレスをバインド
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("bind");
    close(serverSocket);
    return -1;
  }

  // クライアントからの接続待機
  if (listen(serverSocket, QUEUE_SIZE) == -1) {
    perror("listen");
    close(serverSocket);
    return -1;
  }

  printf("Server is listening on port %d...\n", SERVER_PORT);

  while (1) {
    // クライアントからの接続受付
    clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
      perror("accept");
      continue;
    }
    printf("Client connected: %s\n", inet_ntoa(clientAddr.sin_addr));

    // クライアントとの通信を処理
    handleClient(clientSocket, blockchain);

    close(clientSocket);
  }

  // ブロックチェーンの解放
  freeBlockchain(blockchain);

  close(serverSocket);

  return 0;
}

void handleClient(int clientSocket, Blockchain *blockchain) {
  int choice;
  char *data;

  // メニューの表示
  printf("----- Menu -----\n");
  printf("1. Add Block\n");
  printf("2. Exit\n");
  printf("----------------\n");

  while (1) {
    // クライアントからの選択を受け取る
    if (recv(clientSocket, &choice, sizeof(choice), 0) == -1) {
      perror("recv");
      return;
    }

    // 選択に応じた処理を実行
    switch (choice) {
    case 1:
      // ブロックのデータを受け取る
      if (recv(clientSocket, data, sizeof(data), 0) == -1) {
        perror("recv");
        return;
      }

      // ブロックの作成と追加
      Block *newBlock = createBlock(data, blockchain->head->hash);
      addBlock(blockchain, newBlock);

      // ブロック追加のメッセージとブロックチェーン情報をクライアントに送信
      sendBlockAddedMessage(clientSocket, newBlock);
      sendBlockchainInfo(clientSocket, blockchain);
      break;
    case 2:
      // クライアントからの接続を終了
      return;
    default:
      // 無効な選択
      printf("Invalid choice\n");
      break;
    }
  }
}

void sendBlockAddedMessage(int clientSocket, Block *block) {
  char message[256];
  snprintf(message, sizeof(message), "Block added (Hash: ");
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    snprintf(message + strlen(message), sizeof(message) - strlen(message),
             "%02x", block->hash[i]);
  }
  snprintf(message + strlen(message), sizeof(message) - strlen(message), ")");
  if (send(clientSocket, message, sizeof(message), 0) == -1) {
    perror("send");
  }
}

void sendBlockchainInfo(int clientSocket, Blockchain *blockchain) {
  Block *currentBlock = blockchain->head;
  char message[512];

  snprintf(message, sizeof(message), "Blockchain Info:\n");

  while (currentBlock != NULL) {
    snprintf(message + strlen(message), sizeof(message) - strlen(message),
             "Timestamp: %s", ctime(&(currentBlock->timestamp)));
    snprintf(message + strlen(message), sizeof(message) - strlen(message),
             "Data: %s\n", currentBlock->data);
    snprintf(message + strlen(message), sizeof(message) - strlen(message),
             "Previous Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      snprintf(message + strlen(message), sizeof(message) - strlen(message),
               "%02x", currentBlock->previousHash[i]);
    }
    snprintf(message + strlen(message), sizeof(message) - strlen(message),
             "\nHash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      snprintf(message + strlen(message), sizeof(message) - strlen(message),
               "%02x", currentBlock->hash[i]);
    }
    snprintf(message + strlen(message), sizeof(message) - strlen(message),
             "\n\n");

    currentBlock = currentBlock->next;
  }

  if (send(clientSocket, message, sizeof(message), 0) == -1) {
    perror("send");
  }
}
