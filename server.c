#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

//사용법 출력 함수
void usage() {
printf("syntax : server <port> [-e[-b]]\n");
printf("sample : server 1234 -e -b\n");

}

//에러 츨력 함수
void myerror(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

//클라이언트 핸들링을 위한 스레드함수
void* handleClient(void* arg) {
    int clientSock = *((int*)arg); //클라이언트 소켓
    char buffer[BUFFER_SIZE]; //버퍼
    ssize_t bytesReceived; //받은 바이트 수

    while (1) {
        //클라이언트로부터 메시지 수신
        bytesReceived = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            //클라이언트가 연결을 끊었을때
            printf("Client disconnected\n");
            close(clientSock); //소켓닫기
            pthread_exit(NULL); //스레드 종료
        }
        buffer[bytesReceived] = '\0'; //널 문자 추가
        printf("Received message from client: %s", buffer);

        // 클라이언트에게 다시 메시지 전송(에코)
        send(clientSock, buffer, strlen(buffer), 0);
    }
}

int main(int argc, char* argv[]) {
    // 커맨드 라인 인자가 2개가 아닌 경우 사용법 출력 후 종료
    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    //서버 소켓 생성
    int serverSock = socket(AF_INET, SOCK_STREAM, 0); // Socket 클래스는 client에서 서버로 접속하거나 Server에서 accept 하는데 필요한 클래스
    if (serverSock == -1) {
        myerror("socket");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(argv[1])); //포트설정

    //소켓에 주소 할당
    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        myerror("bind");
    }

    //클라이언트의 연결 요청 대기
    if (listen(serverSock, MAX_CLIENTS) == -1) {
        myerror("listen");
    }

    printf("Server started. Waiting for connections...\n");

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        //클라이언트의 연결 요청 수락
        int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSock == -1) {
            myerror("accept");
        }
        
        //클라이언트 핸들링을 위한 스레드 생성
        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, &clientSock) != 0) {
            myerror("pthread_create");
        }
        pthread_detach(thread); //스레드를 detach하여 자원 해제
        printf("New client connected\n");
    }

    close(serverSock);
    return 0;
}
