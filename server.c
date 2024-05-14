#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void usage() {
    printf("syntax : echo-client <ip> <port>\n");
    printf("sample : echo-client 192.168.10.2 1234\n");
}


void myerror(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void* handleClient(void* arg) {
    int clientSock = *((int*)arg);
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;

    while (1) {
        bytesReceived = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            printf("Client disconnected\n");
            close(clientSock);
            pthread_exit(NULL);
        }
        buffer[bytesReceived] = '\0';
        printf("Received message from client: %s", buffer);

        // Echo back to the client
        send(clientSock, buffer, strlen(buffer), 0);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    int serverSock = socket(AF_INET, SOCK_STREAM, 0); // Socket 클래스는 client에서 서버로 접속하거나 Server에서 accept 하는데 필요한 클래스
    if (serverSock == -1) {
        myerror("socket");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(argv[1]));

    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        myerror("bind");
    }

    if (listen(serverSock, MAX_CLIENTS) == -1) {
        myerror("listen");
    }

    printf("Server started. Waiting for connections...\n");

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSock == -1) {
            myerror("accept");
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, &clientSock) != 0) {
            myerror("pthread_create");
        }
        pthread_detach(thread);
        printf("New client connected\n");
    }

    close(serverSock);
    return 0;
}
