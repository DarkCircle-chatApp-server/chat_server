#  :speech_balloon: Chat Program made by DARK CIRCLE
<!--![제목을-입력해주세요_-001](https://github.com/user-attachments/assets/088cec18-9a27-4358-a075-70851aba208a)-->

![제목을-입력해주세요_-002 (1)](https://github.com/user-attachments/assets/41019166-8a3f-4097-960f-26393208ed2c)

***

# :page_facing_up:주제 선정 배경
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
# :computer:개발 환경

- **Server** : <img src="https://img.shields.io/badge/c++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/mysql-4479A1?style=for-the-badge&logo=mysql&logoColor=white">  <img src="https://img.shields.io/badge/Redis-DC382D?style=for-the-badge&logo=Redis&logoColor=white"> <img src="https://img.shields.io/badge/docker-%230db7ed.svg?style=for-the-badge&logo=docker&logoColor=white">

- **Client** : <img src="https://img.shields.io/badge/react-61DAFB?style=for-the-badge&logo=react&logoColor=black"> <img src="https://img.shields.io/badge/javascript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black">

- **Tools** : <img src="https://img.shields.io/badge/Visual Studio-5C2D91?style=flat-square&logo=Visual Studio&logoColor=white"/> <img src="https://img.shields.io/badge/Visual Studio Code-007ACC?style=for-the-badge&logo=Visual Studio Code&logoColor=white"/> <img src="https://img.shields.io/badge/github-181717?style=for-the-badge&logo=github&logoColor=white"> <img src="https://img.shields.io/badge/Discord-%235865F2.svg?style=for-the-badge&logo=discord&logoColor=white"> `vcpkg`

- **Libraries** : `httplib`, `mysqlcppconnector`, `Redis++`, `json.hpp`, `cpp-jwt`, `openssl`, `nlohmann`
***
# 🧱서비스 아키텍처

<img src="https://github.com/user-attachments/assets/86f49a17-55bd-4254-b669-405439b40b63" width="700"/>

***

# 🎯중점 사항
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

   - 여러 스레드에서 동시에 `Redis 연결객체` 또는 `MySQL 연결객체`에 접근 시 중복 저장, 삭제 등 **이상현상 발생 위험 존재**
   
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
# :evergreen_tree:프로젝트 작업 트리
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
# 👥역할 분담
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
# 🗓️개발 기간 및 작업 관리

### 개발 기간

- **전체 개발 기간** : 2025.03.26 ~ 2025.04.07

- **프로젝트 설계** : 2025.03.26 ~ 2025.03.27

- **기능 구현** : 2025.03.27 ~ 2025.04.05

- **UI 구현** : 2025.03.29 ~ 2025.04.06

- **프로젝트 발표** : 2025.04.07

### 협업 규칙 및 컨벤션

<img src="https://github.com/user-attachments/assets/b8e9f058-b51c-4616-a18d-00afcf3f2851" width="700"/>

### Git 협업 방식

**Git Flow 브랜치 전략 활용**
   
   - `main`, `develop`, `feat` 브랜치로 분리

     - **main** - 최종 배포 단계에서만 사용

     - **develop** - merge 기준 브랜치(master) 역할

     - **feat** - 기능 별 독립적으로 생성. develop과 merge 후 삭제

### Discord 활용

- `공지사항`, `레퍼런스`, `회의록`, `질문 및 이슈 공유`

<img src="https://github.com/user-attachments/assets/3206d7b2-31be-4ddf-aebd-da8b143f64a3" width="700"/>

  
- 웹훅을 통해 원격 리포지토리와 연동 - 실시간 깃 푸쉬 알림 수신

<img src="https://github.com/user-attachments/assets/1a423d5f-c3ad-4a5b-b1cb-46229b976b7e" width="700"/>

### 칸반보드 활용

- Notion 칸반보드 활용하여 작업 일정 및 역할 관리

<img src="https://github.com/user-attachments/assets/dfd8f5c5-c829-4d7d-97bf-1dfd0cc41e37" width="700"/>

***
# 📱페이지별 기능

### ➕[회원가입]

- **초기화면인 로그인 페이지의 중앙 하단 회원가입 버튼 클릭시 회원가입 페이지 이동**

- **ID / PW 유효성 체크**
  - DB에 이미 저장된 아이디인지 중복 체크
  - 비밀번호 / 비밀번호 확인칸에 입력한 내용이 일치하는지 체크

- **비밀번호 입력 숨김 / 숨김해제 버튼**
  
- **카카오맵 / 달력 라이브러리 연동**
  - 외부 라이브러리를 활용하여 실제 데이터 입력 가능 

- **각 회원 정보 입력 후 회원가입 버튼 클릭 시 회원가입 완료**
  - 중복된 아이디 / PW 불일치 / 누락 항목 존재 시 회원가입 실패
 
![회원가입](https://github.com/user-attachments/assets/9aafb660-05a3-4524-9794-dbc4585a48aa)
****

### :lock:[로그인]

- **회원가입한 ID / PW 데이터와 일치 시 로그인**
  - 불일치 시 로그인 실패
  - 관리자에 의해 차단된 사용자인 경우 로그인 실패

- **비밀번호 입력 숨김 / 숨김해제 버튼**

- **로그인 시 JWT 토큰 생성**
  - 성공 시 서버에서 생성된 JWT 토큰을 로컬스토리지에 저장
  - 로그인 이후 클라이언트는 요청을 보낼 때 토큰을 요청 헤더에 포함시켜 전송하여 사용자 인증
 
![로그인](https://github.com/user-attachments/assets/46bfe9fb-bf1f-47dc-8e16-fccebd3faded)
****

### 🙋‍♂️[마이페이지]

- **로그인 후 마이페이지 버튼 클릭 시 마이페이지 이동**

- **회원가입 시 입력했던 정보 조회 가능**

- **비밀번호 변경**
  - 현재 비밀번호 입력
    - 현재 비밀번호 유효성 체크 - DB에 저장되어 있는 비밀번호와 다르면 비밀번호 변경 불가
  - 새로운 비밀번호 입력
    - 새로운 비밀번호 유효성 체크 - New Password와 Confirm New Password칸의 입력값이 다르면 비밀번호 변경 불가

![마이페이지](https://github.com/user-attachments/assets/60a4b26f-8d7a-48ea-a17b-18aa296486a0)
****

### 🗨️[채팅]

- **로그인 후 채팅방 입장 버튼 클릭 시 채팅방 이동**

- **메시지 기록 조회**
  - 채팅방 입장 시 최근 15개 메시지 데이터 화면에 렌더링
  - 이전 기록 불러오기 버튼을 클릭하여 과거 모든 메시지 기록 조회 가능

- **채팅방 참여자 목록 조회**
  - 채팅방에 접속한 모든 유저들이 화면 우측 채팅 멤버 칸에 실시간으로 렌더링됨
  - 나가기 버튼을 누르면 채팅방에서 나가게 되고, 채팅 멤버 칸에서도 실시간으로 사라짐

- **채팅 전송 / 수신**
  - 메시지 텍스트 입력 후 send 버튼을 클릭하거나 엔터키를 입력하면 채팅 전송됨
  - 본인이 전송한 메시지는 우측, 다른 사람의 메시지는 좌측에 표시됨
  - 실시간으로 모든 채팅 멤버들의 메시지 송수신
 
![채팅](https://github.com/user-attachments/assets/8640e5c0-62cb-41ca-87ec-adcc7d747265)
****

### 🗝️[관리자 권한]

- **관리자 계정으로 로그인 후 강사 전용 페이지 버튼 클릭 시 관리자페이지 이동**
  - 현재 강사님 계정만 접속 가능
  - 가입된 모든 유저 목록 및 권한 설정 버튼

- **유저밴**
  - Ban 버튼 클릭 시 해당 유저 로그인 불가
  - 일반 사용자에서 차단된 사용자로 상태창 변경

- **채팅밴**
  - Chat Ban 버튼 클릭 시 해당 유저 채팅 전송 불가
  - 일반 사용자에서 차단된 사용자로 상태창 변경

- **밴 해제**
  - UnBan 버튼 클릭 시 해당 유저 차단 상태 해제
  - 차단된 사용자에서 일반 사용자로 상태창 변경

- **관리자 설정**
  - 관리자 설정 버튼 클릭 시 해당 유저 관리자로 지정
  - 일반 사용자에서 관리자로 상태창 변경
 
![유저밴](https://github.com/user-attachments/assets/271e24b8-a0bc-47de-ad75-9b7193bf3ebc)
![채팅밴](https://github.com/user-attachments/assets/dabe2642-5fd3-4ce1-bbea-3087be8d8dbf)
![관리자지정](https://github.com/user-attachments/assets/ee570f34-516a-4725-9dd7-7e3a4f47296a)

***

# 🐞트러블 슈팅

### Redis -> MySQL 데이터 유실 문제

- **문제 상황** : Redis에 캐싱된 메시지 데이터를 DB에 저장 후 캐시데이터 삭제가 이루어져야하는데, **메시지가 DB로 삽입되기 전에 삭제됨**

- **원인** : 트랜잭션 처리를 하지 않고 여러 동작들을 실행 -> **삽입이 실패했음에도 메시지 데이터 삭제** 동작 수행

- **해결 방법**
  - `setAutoCommit(false)`로 트랜잭션 시작
  - Redis의 데이터를 하나 씩 읽고 DB로 INSERT **성공 시에만** `commit`
  - 실패 시 `rollback` 처리 후 `check_s_data`로 복구

- **결과**
  - Redis 데이터는 DB 저장 **성공 시에만 삭제**되도록 처리되고 있음
  - 장애 상황에서도 메시지 손실 없이 안정적 데이터 이전 가능 

<img src="https://github.com/user-attachments/assets/b0343a40-0669-4332-a9f8-bbf9f046ca3a" width="500"/>

***

### 다수 유저 동시 접속 시 서버 과부하 문제

- **문제 상황** : 사용자 접속 증가 시 서버 응답 지연 및 충돌 발생

- **원인** : 모든 사용자들의 Redis 서버 연결을 **메인 스레드 하나로 처리**

- **해결 방법**
  - 유저 최초 접속 시 `Chat_send` 객체를 동적으로 생성
  - `std::thread`로 독립 실행. `user_set`에 저장

- **결과**
  - 서버 응답성 유지 및 부하 분산, 안정성 확보

<img src="https://github.com/user-attachments/assets/9589e0de-14a2-43f6-86bb-056bd8316574" width="500"/>

***

### Redis 메시지 중복 및 순서 꼬임 문제

- **문제 상황** : 메시지 순서가 **뒤섞이거나 중복 저장됨**

- **원인** : Redis로 메시지들이 **고유 키 없이** 임의의 순서대로 저장됨

- **해결 방법**
  - `msg_id:N` 형태의 고유 키로 메시지 저장
  - DB의 기본키처럼 순차적 삽입 및 복구 기준으로 고유 키 활용
 
- **결과**
  - DB의 기본키와 동일한 로직으로 설계하여 직관성 확보
  - 오류 발생 시 복구 지점으로도 활용 가능
  - 데이터 무결성 유지

 <img src="https://github.com/user-attachments/assets/43e980e4-8313-40aa-ac43-81d52ca2e7e1" width="500"/>

***

### 멀티스레드 환경에서의 동시성 문제

- **문제 상황** : 여러 사용자가 **동시에** 메시지를 보내면 **충돌 발생**

- **원인** : insert_chat() 함수 내 공유 연결객체에 대한 동시성 제어 부재

- **해결 방법**
  - 채팅방 접속한 유저별 **독립적인 스레드 및 연결 객체 생성**
  - `std::mutex`로 MySQL 관련 코드에 `lock_guard`로 lock
  - **한 동작에 한 스레드만** 접근
 
- **결과**
  - 데이터 무결성 유지 및 서버 안정성 확보

<img src="https://github.com/user-attachments/assets/39e63a14-4bc1-4567-80f7-f43ccb434912" width="500"/>

***

# 🔜향후 개선 계획

### #1 코드 리팩토링 - 소켓통신 수업 이후 Redis 대신 WebSocket, MQTT 활용

### #2 CI/CD - 빌드 자동화 - 테스트 - 배포 파이프 라인 구축(Docker, Kubernetis, Github Action, Jenkins, AWS)

### #3 미구현 기능 추가 - 메시지 검색, 마크다운 언어 인식, 사진 전송, 채팅방 개설 기능 추가

***

## chat_client(채팅앱 화면 작업 리포지토리)
[React 코드 작업](https://github.com/DarkCircle-chatApp/chat_client.git)
