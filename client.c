#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

void myerror(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == -1) {
        myerror("socket");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(atoi(argv[2]));

    if (connect(clientSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        myerror("connect");
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(clientSock, buffer, strlen(buffer), 0);

        ssize_t bytesReceived = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            myerror("recv");
        }
        buffer[bytesReceived] = '\0';
        printf("Server response: %s", buffer);
    }

    close(clientSock);
    return 0;
}
