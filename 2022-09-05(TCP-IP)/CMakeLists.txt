# CMake 最低版本要求
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 项目信息
project(Test_TCP)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#AUX_SOURCE_DIRECTORY(. DIR_SRCS)
#add_executable(server ${DIR_SRCS})
add_executable(server ${PROJECT_BINARY_DIR}/../src/server.cc)
add_executable(client ${PROJECT_BINARY_DIR}/../src/client.cc)
#target_link_libraries()
#TARGET_LINK_LIBRARIES(server -pthread muduo_base sqlpool config log httpconn  timer -lmysqlclient) 

