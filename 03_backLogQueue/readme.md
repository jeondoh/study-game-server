<h1 align="center">백로그큐 알아보기</h1>

> 백로그큐의 일반적인 개수와 최대 개수를 알아보기</br> 추가로 시스템적 변화, 연결 개수 초과시 해결할 수 있는 방법 알아보기

## 설명
백로그큐
- 서버 하나의 소켓에 클라이언트의 연결 요청을 처리하기 위해 사용되는 대기열(queue)를 의미
- 백로그큐에 들어있는 연결을 꺼내지 않고(accept() 하지 않음) 최대 몇개까지 쌓이는지 테스트
</br>

### SOMAXCONN
```sh
retval = listen(listenSock, SOMAXCONN);
```
- accept를 하지 않고 연결을 백로그큐에 쌓아두게 되면 최대 <b>200개</b> 까지 쌓이게 된다.
- 이후의 연결에서 `WSAECONNREFUSED - 10061` 오류발생
- 오류내용
	- 연결이 거부되었습니다.
	- 대상 컴퓨터가 적극적으로 거부했기 때문에 연결할 수 없습니다.
	- 이는 일반적으로 서버 애플리케이션이 실행되지 않는 외부 호스트에서 비활성 상태인 서비스에 연결하려고 시도하기 때문에 발생합니다.
</br>

### SOMAXCONN_HINT(SOMAXCONN)
```sh
retval = listen(listenSock, SOMAXCONN_HINT(SOMAXCONN));
```
- 백로그의 최대 길이를 늘리기 위해 `SOMAXCONN_HINT`를 사용해 최대로 늘려봄
- accept를 하지 않고 연결을 백로그큐에 쌓아두게 되면 최대 <b>약 16,000개</b> 까지 쌓이게 된다.
- 이는 동적 포트의 개수로, 동적 포트의 개수만큼 허용한다는걸 알 수 있음
	- 동적포트의 범위 `49,152` ~ `65,535`
- 이후의 연결에서 `WSAENOBUFS - 10055` 오류발생
- 오류내용
	- 사용할 수 있는 버퍼 공간이 없습니다.
	- 시스템에 충분한 버퍼 공간이 부족하거나 큐가 가득 차서 소켓에서 작업을 수행할 수 없습니다.
</br>

### 동적포트 늘리기
- msdn문서를 참고하여 netsh로 늘려보았음
- well-known 포트를 제외한 모든 포트를 동적포트의 범위로 지정하였음
1. 동적포트의 범위를 볼 수 있음
```sh
netsh int ipv4 show dynamicport tcp
```
2. 동적포트의 범위를 조절할 수 있음
```sh
netsh int <ipv4|ipv6> set dynamic <tcp|udp> start=number num=range
```
- 동적포트를 최대치고 늘린 후 <b>약 65,000</b>개의 연결을 받아들일 수 있게 되었다.
- 이후의 연결에서 `WSAENOBUFS - 10055` 오류발생
</br>

### 작업관리자
![image](https://github.com/user-attachments/assets/2c04f57d-d90c-4f17-9a61-9a3a3ff36f54)
- backLogServer의 NonpagedPool의 크기가 클라이언트의 연결이 들어올때 마다 늘어난다.
	- 약 `65,000`개를 받은 뒤엔 NP풀이 `16,115k`만큼 증가한다는걸 알 수 있다.
- 소켓은 네트워크 연결이 항상 빠르게 접근 가능해야 하므로, 커널 수준에서 동작
- 따라서 항상 물리메모리에 올라가며, 절대 페이지 아웃이 되지 않는 `Non-Paged Pool`에 소켓 연결이 올라가게 된다는걸 알 수 있음
</br>

## 참고
- https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-listen
- https://learn.microsoft.com/ko-kr/windows/win32/winsock/accepting-a-connection
- https://learn.microsoft.com/ko-kr/troubleshoot/windows-client/networking/tcp-ip-port-exhaustion-troubleshooting