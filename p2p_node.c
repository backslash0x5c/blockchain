#include "blockchain.h"

#define MAX_PEERS 10

typedef struct {
  int socket;
  struct sockaddr_in address;
} Peer;

void showMenu();
void addBlock(int serverSocket);
void handlePeerConnection(int peerSocket, Peer *peers, int *numPeers);
void sendBlockchainInfo(int peerSocket);
void updateBlockchain(int peerSocket);

int main(void) {
  Blockchain *blockchain = initializeBlockchain();

  int serverSocket, peerSocket;
  struct sockaddr_in serverAddr, peerAddr;
  socklen_t peerAddrLen;

  Peer peers[MAX_PEERS];
  int numPeers = 0;

  // ノード（Peer）との接続用ソケットの作成
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    perror("socket");
    return -1;
  }

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

  printf("Peer is listening on port %d...\n", SERVER_PORT);

  while (1) {
    peerAddrLen = sizeof(peerAddr);
    // ピアからの接続受付
    peerSocket =
        accept(serverSocket, (struct sockaddr *)&peerAddr, &peerAddrLen);
    if (peerSocket == -1) {
      perror("accept");
      continue;
    }
    printf("Client connected: %s\n", inet_ntoa(clientAddr.sin_addr));

    // ピアとの通信を処理
    handlePeerConnection(peerSocket, peers, &numPeers);

    close(peerSocket);
  }

  // ブロックチェーンの解放
  freeBlockchain(blockchain);

  close(serverSocket);

  return 0;
}

void showMenu() {
  printf("----- Menu -----\n");
  printf("1. Add Block\n");
  printf("2. Update Blockchain\n");
  printf("3. Exit\n");
  printf("----------------\n");
}

void addBlock(int serverSocket) {
  int choice;
  char *data;

  choice = 1;

  // ブロックのデータを入力
  printf("Enter data for the block: ");
  scanf("%s", data);

  // サーバに選択とデータを送信
  if (send(serverSocket, &choice, sizeof(choice), 0) == -1) {
    perror("send");
    return;
  }
  if (send(serverSocket, data, sizeof(data), 0) == -1) {
    perror("send");
    return;
  }
}

void handlePeerConnection(int peerSocket, Peer *peers, int *numPeers) {
  char addressStr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(peers[*numPeers].address.sin_addr), addressStr,
            INET_ADDRSTRLEN);
  printf("New peer connected: %s\n", addressStr);

  // ピアを追加
  peers[*numPeers].socket = peerSocket;
  memcpy(&(peers[*numPeers].address), &(peers[*numPeers].address),
         sizeof(struct sockaddr_in));
  (*numPeers)++;

  while (1) {
    int choice;

    // ピアからの選択を受け取る
    if (recv(peerSocket, &choice, sizeof(choice), 0) == -1) {
      perror("recv");
      return;
    }

    switch (choice) {
    case 1:
      // ピアからのブロック追加リクエストを処理
      addBlock(peerSocket);
      break;
    case 2:
      // ピアからのブロックチェーン更新リクエストを処理
      updateBlockchain(peerSocket);
      break;
    case 3:
      // ピアとの接続を終了
      printf("Peer disconnected: %s\n", addressStr);
      return;
    default:
      printf("Invalid choice\n");
      break;
    }
  }
}

void sendBlockchainInfo(int peerSocket) {
  // ブロックチェーン情報をピアに送信
  // 送信処理の詳細は実装してください
}

void updateBlockchain(int peerSocket) {
  // ブロックチェーンの更新処理
  // 更新処理の詳細は実装してください
}
