#include "blockchain.h"

void handleServer(int serverSocket);

int main(void) {
  int serverSocket;
  struct sockaddr_in serverAddr;

  // サーバとの接続用ソケットの作成
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

  // サーバに接続
  if (connect(serverSocket, (struct sockaddr *)&serverAddr,
              sizeof(serverAddr)) == -1) {
    perror("connect");
    close(serverSocket);
    return -1;
  }
  printf("Server connected: %s\n", inet_ntoa(serverAddr.sin_addr));

  showMenu();

  handleServer(serverSocket);
  close(serverSocket);

  return 0;
}

void handleServer(int serverSocket) {
  int choice;
  char data[MAX_DATA_LENGTH];
  char AddedMessage[256];
  char chainInfo[512];

  while (1) {
    // ユーザの選択を受け付ける
    printf("Enter your choice: ");
    scanf("%d", &choice);

    // サーバに選択とデータを送信
    if (send(serverSocket, &choice, sizeof(choice), 0) == -1) {
      perror("send");
      return;
    }

    switch (choice) {
    case 1:
      // ブロックのデータを入力
      printf("Enter data for the block: ");
      scanf("%s", data);
      printf("\n");

      // ブロックのデータを送信
      if (send(serverSocket, data, sizeof(data), 0) == -1) {
        perror("send");
        return;
      }

      // ブロックの追加結果を受信
      if(recv(serverSocket, AddedMessage, sizeof(AddedMessage), 0) == -1) {
        perror("recv");
        return;
      }
      else {
        printf("recv success: BlockAddedMessage\n");
        printf("%s\n\n", AddedMessage);
      }
      
      if(recv(serverSocket, chainInfo, sizeof(chainInfo), 0) == -1) {
        perror("recv");
        return;
      }
      else {
        printf("recv success: BlockchainInfo\n");
        printf("%s\n\n", chainInfo);
      }
      break;
    case 2:
      // クライアントの接続を終了
      if (send(serverSocket, &choice, sizeof(choice), 0) == -1) {
        perror("send");
      }
      return;
    default:
      printf("Invalid choice\n");
      break;
    }
  }
}
