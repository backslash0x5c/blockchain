###
###  Makefile: blockchain
###

# コンパイル フラグ
CC = gcc
CFLAGS = -g -Wall -pedantic
#CFLAGS = -O2

#OBJS = blockchain.o server.o
OBJS = blockchain.o client.o

#TARGET = server
TARGET = client

# 依存関係とターゲットの構築ルール
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBRARIES) -lssl -lcrypto

blockchain.o: blockchain.h 
#server.o: blockchain.h 
client.o: blockchain.h 

clean: 
	rm -f $(OBJS) *~ $(TARGET) 

