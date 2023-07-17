#include "blockchain.h"

void showMenu();

int main(void) {
  int serverSocket;
  struct sockaddr_in serverAddr;

  // サーバとの接続用ソケットの作成
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    perror("socket");
    return -1;
  }

  // サーバアドレスの設定
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons((unsigned short)SERVER_PORT);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

  // サーバに接続
  if (connect(serverSocket, (struct sockaddr *)&serverAddr,
              sizeof(serverAddr)) == -1) {
    perror("connect");
    return -1;
  }

  showMenu();

  while (1) {
    int choice;
    char *data;

    // ユーザの選択を受け付ける
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      // ブロックのデータを入力
      printf("Enter data for the block: ");
      scanf("%s", data);

      // サーバに選択とデータを送信
      if (send(serverSocket, &choice, sizeof(choice), 0) == -1) {
        perror("send");
        return -1;
      }
      if (send(serverSocket, data, sizeof(data), 0) == -1) {
        perror("send");
        return -1;
      }
      break;
    case 2:
      // クライアントの接続を終了
      if (send(serverSocket, &choice, sizeof(choice), 0) == -1) {
        perror("send");
        return -1;
      }
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
  printf("2. Exit\n");
  printf("----------------\n");
}