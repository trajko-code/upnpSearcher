cmake_minimum_required(VERSION 3.1)

project(InternProject)

set(CMAKE_CXX_STANDARD 11)

set(exe_include include/Searcher.h include/STB.h)
set(exe_source src/Searcher.cpp src/STB.cpp)

add_executable(mainUPnP main.cpp ${exe_include} ${exe_source})

target_include_directories(mainUPnP PUBLIC include)

include_directories(/home/syrmia/Desktop/SocketLib/include)
add_library(MySocket SHARED IMPORTED)
set_target_properties(MySocket PROPERTIES IMPORTED_LOCATION /home/syrmia/Desktop/SocketLib/build/libMySocket.so)

target_link_libraries(mainUPnP MySocket)