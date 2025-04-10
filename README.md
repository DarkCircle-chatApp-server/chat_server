#  :speech_balloon: Chat Program made by DARK CIRCLE
<!--![제목을-입력해주세요_-001](https://github.com/user-attachments/assets/088cec18-9a27-4358-a075-70851aba208a)-->

![제목을-입력해주세요_-002 (1)](https://github.com/user-attachments/assets/41019166-8a3f-4097-960f-26393208ed2c)

# 주제 선정 배경
### 현재 문제점

- 교육과정 공지 채팅방에 수업시간 채팅도 함께 올라오는 상황
- 교육과정 내 이미 3개의 카톡 채팅방 존재
- 줌 채팅의 휘발성
- 다소 불편한 인스타그램 pc버전 UI/UX

### Dark Circle 채팅앱의 필요성

- 기존 채팅앱과 분리된 독자적 서비스
- 사용자 편의성을 고려한 기능
- 보안성이 확보된 서비스
- **대규모 서비스 확장 고려한 개발**
- **성능 최적화**
***
# :black_circle: TEAM 'DARK CIRCLE'
| 박관호 | 권광호 | 김동국 | 박건형 | 박현민 |
|--------|--------|--------|--------|--------|
| <img src="https://github.com/user-attachments/assets/47cba1ea-59a7-4b18-a196-0e3ad2a75e44" width="100"/> | <img src="https://github.com/user-attachments/assets/23e22012-ae25-477c-8db2-0442cf3f67cc" width="100"/> | <img src="https://github.com/user-attachments/assets/67315a0e-d258-4b18-9ec0-ddc84fac7f37" width="100"/> | <img src="https://github.com/user-attachments/assets/aeaf6b2a-4b0c-493b-bd18-ae470ec7875e" width="100"/> | <img src="https://github.com/user-attachments/assets/3534a4f8-a71d-464e-a9cb-9b8a2ebc3532" width="100"/> |
| [깃허브](https://github.com/talCSHN) | [깃허브](https://github.com/GhwangHo-Kwon) | [깃허브](https://github.com/CLIVEJACK) | [깃허브](https://github.com/pgh157) | [깃허브](https://github.com/phm0423) |
***
# 개발 환경

- **Server** : <img src="https://img.shields.io/badge/c++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/mysql-4479A1?style=for-the-badge&logo=mysql&logoColor=white">  <img src="https://img.shields.io/badge/Redis-DC382D?style=for-the-badge&logo=Redis&logoColor=white"> <img src="https://img.shields.io/badge/docker-%230db7ed.svg?style=for-the-badge&logo=docker&logoColor=white">

- **Client** : <img src="https://img.shields.io/badge/react-61DAFB?style=for-the-badge&logo=react&logoColor=black"> <img src="https://img.shields.io/badge/javascript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black">

- **Tools** : <img src="https://img.shields.io/badge/Visual Studio-5C2D91?style=for-the-badge&logo=Visual Studio&logoColor=white"/> <img src="https://img.shields.io/badge/Visual Studio Code-007ACC?style=for-the-badge&logo=Visual Studio Code&logoColor=white"/> <img src="https://img.shields.io/badge/github-181717?style=for-the-badge&logo=github&logoColor=white"> <img src="https://img.shields.io/badge/Discord-%235865F2.svg?style=for-the-badge&logo=discord&logoColor=white"> `vcpkg`

- **Libraries** : `httplib`, `mysqlcppconnector`, `Redis++`, `json.hpp`, `cpp-jwt`, `openssl`, `nlohmann`
***
# 서비스 아키텍처

<img src="https://github.com/user-attachments/assets/86f49a17-55bd-4254-b669-405439b40b63" width="700"/>
***

# 중점 사항
### 1. Redis PUB/SUB 기반 실시간 메시지 처리

   - `Redis Publish/Subscribe` 기능을 활용하여 **실시간 메시지 송수신** 구현
   
   - 클라이언트에서 메시지 전송 요청 시, 요청 `body`에 포함된 정보를 서버에서 `json` 형태로 파싱하여 `Redis`에 `Publish`
   
   - 서버는 해당 채널을 `Subscribe`하고 있으며, 메시지를 즉시 수신한 뒤 `SSE(Server-Sent Events)` 를 통해 클라이언트로 실시간 전달
   
   - **다중 사용자 처리** - 해당 채팅 `Topic`을 `Subscribe`하는 **모든 클라이언트**에게 동시에 메시지 전달 가능

### 2. 멀티스레드 구현

   - 새로운 클라이언트가 접속할 때마다 **별도의 스레드**로 **Redis 연결객체 관리**
   
   - `std::thread`와 `detach()`를 활용하여 채팅 수신처리를 **백그라운드에서 병렬로 수행**
   
   - **별도의 스레드**에서 `SSE(Server Sent Event)` 연결 처리 - 클라이언트는 서버와의 `SSE 연결`을 통해 서버에서 `subscribe`한 응답 데이터를 스트리밍 방식으로 **실시간 수신**
<img src="https://github.com/user-attachments/assets/cf15fbbd-d8bc-41e5-8318-03b4e066bba4" width="300"/>

### 3. 트랜잭션 처리

   
   - **메시지 전송 요청 시** : `캐싱DB에 메시지 데이터 캐싱` -> `물리DB에 해당 데이터 삽입` -> `캐시 데이터 삭제`
   
   - 위의 동작들을 **하나의 흐름으로 묶어** 순차적, 안정적으로 처리
   
   - 동작들 중 하나라도 실패시 바로 `Rollback` 처리하여 **데이터 무결성** 보장

### 4. 동시성 제어

   - 여러 스레드에서 동시에 `Redis 연결객체` 또는 `MySQL 연결객체`에 접근 시 중복 저장, 삭제 등 **이상현상 발생 가능**
   
   - `std::mutex`, `std::lock_guard` 사용하여 `mutex`로 선언된 블록에 진입할 수 있는 스레드 갯수를 **하나로 제한**
   
   - 동시에 여러 스레드가 접근하지 못하게 하여 **원자성** 보장

### 5. 대규모 서비스 확장 가능성

   - `MSA 아키텍처`를 적용한 설계 - 각 기능별로 **독립적 확장 및 배포 가능**
   
   - 서비스를 **기능 단위로 분리** - `회원가입/로그인`, `채팅`, `관리자`
   
   - 각 기능을 `Docker` 컨테이너로 올린 후 `Kubernetes` 등 오케스트레이션 툴로 **CI/CD 파이프라인 구축 가능**
   
   - 부하 발생한 서버만 따로 여러 인스턴스로 띄워 로드밸런싱 적용 가능
   
   - 필요시 특정 기능만 업데이트 / 롤백 - **무중단 배포 가능**

### 6. 로그인 데이터 보안

   - `JWT(JSON Web Token)` 기반 인증 방식 적용
   
   - 로그인 성공 시 서버에서 `JWT 토큰` 발급
   
   - 이후 클라이언트 요청 시 `Authorization 헤더`에 `token`을 포함하여 요청 전송
   
   - 서버는 해당 `token`을 검증하여 **사용자 인증**
***
# 프로젝트 작업 트리
```shell
Folder PATH listing
Volume serial number is 4A02-284C
C:.
|   .gitignore
|   libcrypto-3-x64.dll
|   libssl-3-x64.dll
|   mysqlcppconn-10-vs14.dll
|   README.md
|
|   
+---.github
|   \---workflows
|           c-cpp-test.yml
|           c-cpp.yml
|           cmake-multi-platform.yml
|           
+---api-gateway
|   |   api-gateway.sln
|   |   main.cpp
|   |           
|   \---x64
|       |       
|       \---Release
|               api-gateway.exe
|               api-gateway.pdb
|               
+---chat
|   |   chat.sln
|   |   chat_print.hpp
|   |   chat_room.hpp
|   |   chat_send.hpp
|   |   DB.hpp
|   |   httplib.h
|   |   json.hpp
|   |   main.cpp
|   |                   
|   \---x64
|       |       
|       \---Release
|               chat.exe
|               chat.pdb
|               
+---chat-admin
|   |   admin_select_delete.hpp
|   |   change_pw.hpp
|   |   chat-admin.sln
|   |   chat_ban.hpp
|   |   DBex2.hpp
|   |   DBex3.cpp
|   |   DB_admin.hpp
|   |   httplib.h
|   |   json.hpp
|   |   main.cpp
|   \---x64
|       |       
|       \---Release
|               chat-admin.exe
|               chat-admin.pdb
|               
+---dockerConfig
|       docker-compose.yml
|       redis.conf
|       
\---signIn-login
    |   CMakeLists.txt
    |   DBmodule.hpp
    |   DBmodule1.hpp
    |   Dockerfile
    |   json.hpp
    |   main.cpp
    |   main111.cpp
    |   signIn-login.sln
    |   signinClass.hpp
    |   signinClass11111.hpp
    |   
    +---jwt
    |   |   jwt.hpp        
    |                   
    \---x64
        |       
        \---Release
                signIn-login.exe
                signIn-login.pdb
```
***
# 역할 분담
### - 박관호(팀장) - Backend / Frontend
- 회원가입/로그인 서버
- SSE 연결 서버
- 서비스 전체 페이지 - [[Chat-Client 리포지토리]](https://github.com/DarkCircle-chatApp/chat_client.git)

### - 권광호(팀원) - Backend
- 채팅 서버
- Redis 연결 관리

### - 김동국(팀원) - Backend
- 채팅 서버

### - 박건형(팀원) - Backend
- 관리자 서버

### - 박현민(팀원) - Backend
- 관리자 서버

**추후 리팩토링 작업하면서 본인이 맡지 않았던 기능도 직접 구현하고 개선시켜볼 것.** **필수!!** 
***
# 개발 기간 및 작업 관리

### 개발 기간

- 전체 개발 기간 : 2025.03.26 ~ 2025.04.07

- 프로젝트 설계 : 2025.03.26 ~ 2025.03.27

- 기능 구현 : 2025.03.27 ~ 2025.04.05

- UI 구현 : 2025.03.29 ~ 2025.04.06

- 프로젝트 발표 : 2025.04.07

### 협업 규칙 및 컨벤션

<img src="https://github.com/user-attachments/assets/b8e9f058-b51c-4616-a18d-00afcf3f2851" width="700"/>

### Git 협업 방식

**Git Flow 브랜치 전략 활용**
   
   - `main`, `develop`, `feat` 브랜치로 분리

     - **main** - 최종 배포 단계에서만 사용

     - **develop** - merge 기준 브랜치(master) 역할

     - **feat** - 기능 별 독립적으로 생성. develop과 merge 후 삭제

### Discord 활용

- 공지사항, 레퍼런스, 회의록, 질문 및 이슈 공유

<img src="https://github.com/user-attachments/assets/3206d7b2-31be-4ddf-aebd-da8b143f64a3" width="700"/>

  
- 웹훅을 통해 원격 리포지토리와 연동 - 실시간 깃 푸쉬 알림 수신

<img src="https://github.com/user-attachments/assets/1a423d5f-c3ad-4a5b-b1cb-46229b976b7e" width="700"/>

### 칸반보드 활용

- Notion 칸반보드 활용하여 작업 일정 및 역할 관리

<img src="https://github.com/user-attachments/assets/dfd8f5c5-c829-4d7d-97bf-1dfd0cc41e37" width="700"/>

***
# 페이지별 기능




# chat_client(채팅앱 화면 작업 리포지토리)
https://github.com/DarkCircle-chatApp/chat_client.git
