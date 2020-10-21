cmake_minimum_required(VERSION 3.9)

project(upnpSearcher)

set(CMAKE_CXX_STANDARD 11)

set(exe_include 
        include/Searcher.hpp 
        include/XMLParser.hpp 
        include/HTTPCommunicator.hpp
        include/Config.hpp 
        include/InOut.hpp
        include/setTopBox/STB.hpp
        include/setTopBox/Service.hpp
        include/setTopBox/Action.hpp
        include/setTopBox/Argument.hpp
        include/setTopBox/STBTypes.hpp
        include/setTopBox/KeyCodes.hpp)

set(exe_source 
        src/Searcher.cpp 
        src/STB.cpp
        src/Service.cpp
        src/Action.cpp
        src/Argument.cpp
        src/XMLParser.cpp 
        src/HTTPCommunicator.cpp 
        src/InOut.cpp)

add_executable(mainUPnP main.cpp ${exe_include} ${exe_source})

target_include_directories(mainUPnP PUBLIC include)

set(MY_SOCKET_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/MySocket/include")

if(NOT EXISTS "${MY_SOCKET_INCLUDE_PATH}")
	file(DOWNLOAD 
		"https://github.com/trajko-code/socketLib/releases/download/v1.0.0/MySocket.tar.xz" 
		"${CMAKE_SOURCE_DIR}/lib/MySocket.tar.xz"
		TLS_VERIFY ON)

	if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/MySocket")
		execute_process(
			COMMAND mkdir MySocket
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
	endif()

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/lib/MySocket.tar.xz
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/MySocket)
		
	file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/lib/MySocket.tar.xz")
endif()

set(MY_SOCKET_LIB_PATH "${CMAKE_SOURCE_DIR}/lib/MySocket/libMySocket.so")

include_directories(${MY_SOCKET_INCLUDE_PATH})

target_link_libraries(mainUPnP ${MY_SOCKET_LIB_PATH})
