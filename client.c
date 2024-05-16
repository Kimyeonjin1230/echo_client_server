#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// 에러 출력 및 프로그램 종료 함수
void myerror(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(){
    printf("syntax : client <ip> <port>\n");
    printf("sample : client 192.168.10.2 1234\n");

}
int main(int argc, char* argv[]) {
    // 커맨드 라인 인자가 3개가 아닌 경우 사용법 출력 후 종료
    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    // 클라이언트 소켓 생성
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == -1) {
        myerror("socket");
    }

    // 서버 주소 설정
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]); // IP 주소 설정
    serverAddr.sin_port = htons(atoi(argv[2])); // 포트 설정

    // 서버에 연결
    if (connect(clientSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        myerror("connect");
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        // 사용자로부터 메시지 입력 받음
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        
        // 메시지 전송
        send(clientSock, buffer, strlen(buffer), 0);

        // 서버로부터 응답 받음
        ssize_t bytesReceived = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            myerror("recv");
        }
        buffer[bytesReceived] = '\0'; // 문자열 마지막에 널 문자 추가
        printf("Server response: %s", buffer);
    }

    close(clientSock); // 클라이언트 소켓 닫기
    return 0;
}
