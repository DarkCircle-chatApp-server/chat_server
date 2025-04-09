#  :speech_balloon: Chat Program made by DARK CIRCLE
<!--![제목을-입력해주세요_-001](https://github.com/user-attachments/assets/088cec18-9a27-4358-a075-70851aba208a)-->

![제목을-입력해주세요_-002 (1)](https://github.com/user-attachments/assets/41019166-8a3f-4097-960f-26393208ed2c)

# 주제 선정 배경
**현재 문제점**
- 교육과정 공지용 채팅방에 수업시간 채팅도 함께 올라오는 상황
- 이미 3개의 카카오톡 톡방 존재
- 줌 채팅의 휘발성
- 다소 불편한 인스타그램 pc버전 UI/UX

**Dark Circle 채팅앱의 필요성**
- 카카오톡과 분리된 독자적 채팅앱
- 사용자 편의성을 고려한 기능
- 보안성을 고려한 채팅앱
- **대규모 서비스 확장 고려한 개발**
- **성능 최적화**

# 👥 팀원 소개
| 박관호 | 권광호 | 김동국 | 박건형 | 박현민 |
|--------|--------|--------|--------|--------|
| <img src="https://github.com/user-attachments/assets/47cba1ea-59a7-4b18-a196-0e3ad2a75e44" width="100"/> | <img src="https://github.com/user-attachments/assets/23e22012-ae25-477c-8db2-0442cf3f67cc" width="100"/> | <img src="https://github.com/user-attachments/assets/67315a0e-d258-4b18-9ec0-ddc84fac7f37" width="100"/> | <img src="https://github.com/user-attachments/assets/aeaf6b2a-4b0c-493b-bd18-ae470ec7875e" width="100"/> | <img src="https://github.com/user-attachments/assets/3534a4f8-a71d-464e-a9cb-9b8a2ebc3532" width="100"/> |
| [깃허브](https://github.com/talCSHN) | [깃허브](https://github.com/GhwangHo-Kwon) | [깃허브](https://github.com/CLIVEJACK) | [깃허브](https://github.com/pgh157) | [깃허브](https://github.com/phm0423) |

# 개발 환경
- **Server** : <img src="https://img.shields.io/badge/c++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/mysql-4479A1?style=for-the-badge&logo=mysql&logoColor=white">  <img src="https://img.shields.io/badge/Redis-DC382D?style=for-the-badge&logo=Redis&logoColor=white"> <img src="https://img.shields.io/badge/docker-%230db7ed.svg?style=for-the-badge&logo=docker&logoColor=white">
- **Client** : <img src="https://img.shields.io/badge/react-61DAFB?style=for-the-badge&logo=react&logoColor=black"> <img src="https://img.shields.io/badge/javascript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black">
- **Tools** : <img src="https://img.shields.io/badge/Visual Studio-5C2D91?style=for-the-badge&logo=Visual Studio&logoColor=white"/> <img src="https://img.shields.io/badge/Visual Studio Code-007ACC?style=for-the-badge&logo=Visual Studio Code&logoColor=white"/> <img src="https://img.shields.io/badge/github-181717?style=for-the-badge&logo=github&logoColor=white"> <img src="https://img.shields.io/badge/Discord-%235865F2.svg?style=for-the-badge&logo=discord&logoColor=white">
- **Libraries** : `vcpkg`, `httplib`, `mysqlcppconnector`, `Redis++`, `json.hpp`, `cpp-jwt`, `openssl`, `nlohmann`

# 중점 사항
1. Redis PUB/SUB 기반 실시간 메시지 처리
   - `Redis Publish/Subscribe` 기능을 활용하여 **실시간 메시지 송수신** 구현
   - 클라이언트는 해당 채팅방 `채널(Topic)`을 `구독(Subscribe)`하고, 서버는 메시지를 해당 `채널(Topic)`에 `발행(Publish)`
   - 메시지 전송 요청 -> `json` 형태로 들어온 `요청 body`의 요소들을 서버에서 `Redis`로 `Publish` -> `Subscribe` 중인 클라이언트에게 즉시 전달
   - **다중 사용자 처리** - 해당 채팅 `Topic`을 `Subscribe`하는 **모든 클라이언트**에게 동시에 메시지 전달 가능

2. 멀티스레드 구현
   - 새로운 클라이언트가 접속할 때마다 **별도의 스레드**로 **Redis 연결객체 관리**
   - `std::thread`와 `detach()`를 활용하여 채팅 수신처리를 **백그라운드에서 병렬로 수행**
   - **별도의 스레드**에서 `SSE(Server Sent Event)` 통신 처리 - 클라이언트는 서버와의 `SSE 통신`을 통해 서버에서 `subscribe`한 응답 데이터를 스트리밍 방식으로 **실시간 수신**
<img src="https://github.com/user-attachments/assets/cf15fbbd-d8bc-41e5-8318-03b4e066bba4" width="300"/>

3. 트랜잭션 처리
   - **메시지 전송 요청 시** : `Redis에 메시지 데이터 캐싱` -> `DB에 해당 데이터 삽입` -> `캐시 데이터 삭제`
   - 위의 동작들을 **하나의 흐름으로 묶어** 순차적, 안정적으로 처리
   - 동작들 중 하나라도 실패시 바로 `Rollback` 처리하여 **데이터 무결성** 보장


4. 동시성 제어
   - 여러 스레드에서 동시에 `Redis 연결객체` 또는 `MySQL 연결객체`에 접근 시 중복 저장, 삭제 등 **이상현상 발생 가능**
   - `std::mutex`, `std::lock_guard` 사용하여 `mutex`로 선언된 블록에 진입할 수 있는 스레드 갯수를 **하나로 제한**
   - 동시에 여러 스레드가 접근하지 못하게 하여 **원자성** 보장

5. 대규모 서비스 확장 가능성
   - `MSA 아키텍처`를 적용한 설계 - 각 기능별로 **독립적** 확장 및 배포 가능
   - 서비스를 **기능 단위로 분리** - `회원가입/로그인`, `채팅`, `관리자`
   - 각 기능별로 `Docke`r로 컨테이너화 하여 `Kubernetes` 등 오케스트레이션 툴로 **CI/CD 파이프라인 구축에 용이**
   - 부하 발생한 서버만 따로 여러 인스턴스로 띄워 로드밸런싱 적용 가능
   - 필요시 특정 기능만 업데이트 / 롤백 - **무중단 배포 가능**

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
|   |   api-gateway.vcxproj
|   |   api-gateway.vcxproj.filters
|   |   api-gateway.vcxproj.user
|   |   main.cpp
|   |           
|   \---x64
|       +---Debug
|       |       api-gateway.exe
|       |       api-gateway.pdb
|       |       
|       \---Release
|               api-gateway.exe
|               api-gateway.pdb
|               
+---chat
|   |   chat.sln
|   |   chat.vcxproj
|   |   chat.vcxproj.filters
|   |   chat.vcxproj.user
|   |   chat_print.hpp
|   |   chat_room.hpp
|   |   chat_send.hpp
|   |   DB.hpp
|   |   httplib.h
|   |   json.hpp
|   |   libcrypto-3-x64.dll
|   |   libeay32.dll
|   |   libssl-3-x64.dll
|   |   main.cpp
|   |   mysqlcppconn-10-vs14.dll
|   |   mysqlcppconn.dll
|   |   ssleay32.dll
|   |                   
|   \---x64
|       +---Debug
|       |       chat.exe
|       |       chat.pdb
|       |       hiredisd.dll
|       |       libcrypto-3-x64.dll
|       |       libssl-3-x64.dll
|       |       mysqlcppconn-10-vs14.dll
|       |       redis++.dll
|       |       
|       \---Release
|               chat.exe
|               chat.pdb
|               hiredis.dll
|               libcrypto-3-x64.dll
|               libeay32.dll
|               libssl-3-x64.dll
|               mysqlcppconn.dll
|               redis++.dll
|               ssleay32.dll
|               
+---chat-admin
|   |   admin_select_delete.hpp
|   |   change_pw.hpp
|   |   chat-admin.sln
|   |   chat-admin.vcxproj
|   |   chat-admin.vcxproj.filters
|   |   chat-admin.vcxproj.user
|   |   chat_ban.hpp
|   |   DBex2.hpp
|   |   DBex3.cpp
|   |   DB_admin.hpp
|   |   httplib.h
|   |   json.hpp
|   |   libcrypto-3-x64.dll
|   |   libeay32.dll
|   |   libssl-3-x64.dll
|   |   main.cpp
|   |   mysqlcppconn-10-vs14.dll
|   |   mysqlcppconn.dll
|   |   ssleay32.dll
|   \---x64
|       +---Debug
|       |       chat-admin.exe
|       |       chat-admin.pdb
|       |       libcrypto-3-x64.dll
|       |       libssl-3-x64.dll
|       |       mysqlcppconn-10-vs14.dll
|       |       
|       \---Release
|               chat-admin.exe
|               chat-admin.pdb
|               libcrypto-3-x64.dll
|               libeay32.dll
|               libssl-3-x64.dll
|               mysqlcppconn-10-vs14.dll
|               mysqlcppconn.dll
|               ssleay32.dll
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
    |   libcrypto-3-x64.dll
    |   libeay32.dll
    |   libmysql.dll
    |   libssl-3-x64.dll
    |   main.cpp
    |   main111.cpp
    |   mysqlcppconn-10-vs14.dll
    |   mysqlcppconn.dll
    |   signIn-login.sln
    |   signIn-login.vcxproj
    |   signIn-login.vcxproj.filters
    |   signIn-login.vcxproj.user
    |   signinClass.hpp
    |   signinClass11111.hpp
    |   ssleay32.dll
    |   test1.hpp
    |   test2.hpp
    |   
    +---jwt
    |   |   algorithm.hpp
    |   |   assertions.hpp
    |   |   base64.hpp
    |   |   config.hpp
    |   |   error_codes.hpp
    |   |   exceptions.hpp
    |   |   jwt.hpp
    |   |   parameters.hpp
    |   |   short_string.hpp
    |   |   stack_alloc.hpp
    |   |   string_view.hpp
    |           
    |                   
    \---x64
        +---Debug
        |       Dockerfile
        |       libcrypto-3-x64.dll
        |       libmysql.dll
        |       libssl-3-x64.dll
        |       mysqlcppconn-10-vs14.dll
        |       ngrok.exe
        |       signIn-login.exe
        |       signIn-login.pdb
        |       
        \---Release
                libcrypto-3-x64.dll
                libeay32.dll
                libmysql.dll
                libssl-3-x64.dll
                mysqlcppconn-10-vs14.dll
                mysqlcppconn.dll
                signIn-login.exe
                signIn-login.pdb
                ssleay32.dll
```

# 서비스 아키텍처 및 로직

![제목을-입력해주세요_-006](https://github.com/user-attachments/assets/86f49a17-55bd-4254-b669-405439b40b63)











# chat_client(채팅앱 화면 작업 리포지토리)
https://github.com/DarkCircle-chatApp/chat_client.git
