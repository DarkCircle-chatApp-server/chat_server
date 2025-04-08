cmake_minimum_required(VERSION 3.10)   # 최소 CMake 버전
project(signIn-login)                 # 프로젝트 이름

set(CMAKE_CXX_STANDARD 17)             # C++ 표준 설정 (C++17)
set(CMAKE_CXX_STANDARD_REQUIRED True)  # C++ 표준 강제

# 소스 파일 지정 (main.cpp 외에 필요한 파일 추가)
set(SOURCE_FILES 
    main.cpp 
    DBmodule.hpp 
    json.hpp
    signinClass.hpp
)

# 실행 파일 생성
add_executable(signIn-login ${SOURCE_FILES})

# MySQL & Redis 라이브러리 링크
find_package(PkgConfig REQUIRED)
pkg_check_modules(REDIS REQUIRED hiredis) # Redis 라이브러리 찾기

# MySQL C++ Connector 찾기
find_path(MYSQL_INCLUDE_DIR mysql/jdbc.h PATHS /usr/include /usr/local/include)
find_library(MYSQL_LIB mysqlcppconn PATHS /usr/lib /usr/local/lib)

# 라이브러리 포함
target_include_directories(chat-server PRIVATE ${MYSQL_INCLUDE_DIR})
target_link_libraries(chat-server PRIVATE ${MYSQL_LIB} ${REDIS_LIBRARIES})
