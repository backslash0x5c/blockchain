#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void showMenu();
void addBlock(int serverSocket);
void updateBlockchain(int serverSocket);

int main(void) {
  int serverSocket;
  struct sockaddr_in serverAddr;

  // ノード（Peer）との接続用ソケットの作成
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    perror("socket");
    return -1;
  }

  // サーバアドレスの設定
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // サーバに接続
  if (connect(serverSocket, (struct sockaddr *)&serverAddr,
              sizeof(serverAddr)) == -1) {
    perror("connect");
    return -1;
  }

  showMenu();

  while (1) {
    int choice;

    // ユーザの選択を受け付ける
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      // ピアに対してブロック追加リクエストを送信
      addBlock(serverSocket);
      break;
    case 2:
      // ピアに対してブロックチェーン更新リクエストを送信
      updateBlockchain(serverSocket);
      break;
    case 3:
      // クライアントの接続を終了
      close(serverSocket);
      return 0;
    default:
      printf("Invalid choice\n");
      break;
    }
  }

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
  char data[MAX_DATA_LENGTH];

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

void updateBlockchain(int serverSocket) {
  int choice;

  choice = 2;

  // サーバに選択を送信
  if (send(serverSocket, &choice, sizeof(choice), 0) == -1) {
    perror("send");
    return;
  }
}
