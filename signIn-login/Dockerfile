# 1. Amazon Linux 2 기반 이미지 사용
FROM amazonlinux:2

# 2. 필수 패키지 설치 (curl 추가)
RUN yum update -y && \
    yum install -y gcc gcc-c++ make mariadb-devel redis curl tar

# 3. 최신 CMake 설치 (소스 빌드)
WORKDIR /tmp
RUN curl -LO https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.tar.gz && \
    tar -xvzf cmake-3.26.4-linux-x86_64.tar.gz && \
    mv cmake-3.26.4-linux-x86_64 /usr/local/cmake && \
    ln -s /usr/local/cmake/bin/cmake /usr/bin/cmake

# 4. 로컬 C++ 프로젝트 파일을 컨테이너에 복사
COPY . .

# 5. CMake를 사용하여 C++ 프로젝트 빌드
RUN mkdir build && cd build && cmake .. && make -j$(nproc)

# 6. 실행할 바이너리 지정
CMD ["./build/main.cpp"]
