// Compile: gcc multi_server.c blockchain.c -o multi_server -lssl -lcrypto -lpthread
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "blockchain.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// クライアント情報を格納する構造体
typedef struct {
  int socket;
  struct sockaddr_in address;
} client_t;

// グローバル変数
client_t clients[MAX_CLIENTS]; // クライアント情報を格納する配列
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // スレッド間の排他制御用のミューテックス
void *client_thread(void *arg); // クライアントとの通信を行うスレッドのエントリーポイント

int main(void) {
  Blockchain *blockchain = initializeBlockchain();
  Block *genesisBlock = createBlock("Genesis Block", (unsigned char *)"");
  addBlock(blockchain, genesisBlock);

  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_size;
  pthread_t thread;

  // サーバソケットを作成
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&server_address, 0, sizeof(struct sockaddr_in));

  // サーバアドレスを設定
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons((unsigned short)SERVER_PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // サーバソケットを指定したアドレスにバインド
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    perror("Binding failed");
    exit(EXIT_FAILURE);
  }

  // サーバソケットをリッスン状態にする
  if (listen(server_socket, MAX_CLIENTS) < 0) {
    perror("Listening failed");
    exit(EXIT_FAILURE);
  }

  printf("Server started. Waiting for clients...\n");

  while (1) {
    // クライアント接続の受け入れ
    client_address_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
    if (client_socket < 0) {
      perror("Accepting connection failed");
      exit(EXIT_FAILURE);
    }

    // クライアント情報を配列に格納
    pthread_mutex_lock(&mutex);
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].socket == 0) {
        clients[i].socket = client_socket;
        clients[i].address = client_address;
        break;
      }
    }
    pthread_mutex_unlock(&mutex);

    if (i == MAX_CLIENTS) {
      // クライアント数が上限に達した場合は接続を拒否する
      printf("Maximum number of clients reached. Connection rejected.\n");
      close(client_socket);
    } else {
      // クライアントとの通信を行うスレッドを作成
      int *client_index_ptr = malloc(sizeof(int));
      *client_index_ptr = i;
      pthread_create(&thread, NULL, client_thread, client_index_ptr);
      pthread_detach(thread);

      // クライアントのIPアドレスとポートを表示
      char client_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
      printf("Client connected: %s:%d\n", client_ip, ntohs(client_address.sin_port));
    }
  }

  // サーバソケットをクローズ
  close(server_socket);

  return 0;
}

// クライアントとの通信を行うスレッドのエントリーポイント
void *client_thread(void *arg) {
  int client_index = *(int *)arg;
  char buffer[BUFFER_SIZE];

  while (1) {
    memset(buffer, 0, sizeof(buffer));

    // クライアントからのデータを受信
    ssize_t recv_size =
        recv(clients[client_index].socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_size <= 0) {
      // クライアントとの接続が切れた場合、スレッドを終了する
      break;
    }

    // 受信したデータを処理（ここでは単純にクライアントにエコーバック）
    send(clients[client_index].socket, buffer, strlen(buffer), 0);
  }

  // クライアントが切断された場合、クライアント情報を削除する
  pthread_mutex_lock(&mutex);
  clients[client_index].socket = 0;
  pthread_mutex_unlock(&mutex);

  return NULL;
}
