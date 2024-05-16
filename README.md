# echo_client_server
![alt text](image.png)

PF_INET(비연결지향형)은  read, write함수가 의미가 있지만 AF_INET(연결지향형)은 read, write함수가 의미가 없어 스레드를 생성해주는 것이 좋다.

