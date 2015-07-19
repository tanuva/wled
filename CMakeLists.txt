project("WLED")
cmake_minimum_required(VERSION 2.8.11)

if(APPLE OR WIN32)
	message("This program doesn't really make sense on non-Linux boxes but you're free to try.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CGI_SRC
	src/cgi_main.cpp
)

add_executable(wled.cgi ${CGI_SRC})

set(DAEMON_SRC
	src/daemon_main.cpp
)

add_executable(wled ${DAEMON_SRC})
