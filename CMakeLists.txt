cmake_minimum_required(VERSION 3.16)
project(CrackArchive C)

set(CMAKE_C_STANDARD 11)

link_directories(${PROJECT_SOURCE_DIR}/lib) #添加连接库的路径

#set(CMAKE_EXE_LINKER_FLAGS "-static -pthread")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(CrackArchive main.c tinycthread.c)
add_executable(async async.c tinycthread.c)
add_executable(c c.c)
add_executable(ziptest ziptest.c)

target_link_libraries(CrackArchive libUnArchive)
target_link_libraries(c libua)

target_link_libraries(ziptest zip)

