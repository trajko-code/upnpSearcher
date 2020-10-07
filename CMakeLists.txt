cmake_minimum_required(VERSION 3.1)

project(upnpSearcher)

set(CMAKE_CXX_STANDARD 11)

set(exe_include include/Searcher.h include/STB.h include/XMLParser.h include/HTTPCommunicator.h include/Configuration.h include/InOut.h)
set(exe_source src/Searcher.cpp src/STB.cpp src/XMLParser.cpp src/HTTPCommunicator.cpp src/InOut.cpp)

add_executable(mainUPnP main.cpp ${exe_include} ${exe_source})

target_include_directories(mainUPnP PUBLIC include)

include_directories(/home/syrmia/Desktop/socketLib/include)
add_library(MySocket SHARED IMPORTED)
set_target_properties(MySocket PROPERTIES IMPORTED_LOCATION /home/syrmia/Desktop/socketLib/build/libMySocket.so)

target_link_libraries(mainUPnP MySocket)
