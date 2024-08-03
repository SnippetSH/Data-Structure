cmake_minimum_required(VERSION 3.22)

# 프로젝트 이름과 사용 언어 설정
project(Btree)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 소스 파일 설정
set(SOURCES main.cpp)

# 헤더 파일 포함 경로 설정
include_directories(${PROJECT_SOURCE_DIR})

# 실행 파일 생성
add_executable(main ${SOURCES})
