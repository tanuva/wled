project("WLED")
cmake_minimum_required(VERSION 2.8.11)

if(APPLE OR WIN32)
	message("This program doesn't really make sense on non-Linux boxes but you're free to try.")
endif()

set(TARGET_HOST "root@wled" CACHE STRING "scp binaries to user@host")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CGI_SRC
	src/cgi_main.cpp
)

add_executable(wled.cgi ${CGI_SRC})

add_custom_command(
	TARGET wled.cgi POST_BUILD
	COMMAND "scp"
	ARGS ${CMAKE_CURRENT_BINARY_DIR}/wled.cgi ${TARGET_HOST}:/www/cgi-bin
)

set(DAEMON_SRC
	src/daemon_main.cpp
	src/gpio.cpp
)

add_executable(wledd ${DAEMON_SRC})

add_custom_command(
	TARGET wledd POST_BUILD
	COMMAND "scp"
	ARGS ${CMAKE_CURRENT_BINARY_DIR}/wledd ${TARGET_HOST}: # TODO Find a better location
)

add_custom_command(
	TARGET wledd POST_BUILD
	COMMAND "scp"
	ARGS ${CMAKE_CURRENT_BINARY_DIR}/../wledd.initscript ${TARGET_HOST}:/etc/init.d/wledd
)

set(COPY_SRC
	src/web/wled.js
	src/web/index.html
)

add_custom_target(web
	SOURCES ${COPY_SRC}
)

add_custom_command(
	TARGET web
	COMMAND "scp"
		ARGS ${CMAKE_CURRENT_BINARY_DIR}/../src/web/wled.js ${CMAKE_CURRENT_BINARY_DIR}/../src/web/index.html ${CMAKE_CURRENT_BINARY_DIR}/../src/web/wled.css ${TARGET_HOST}:/www/wled
)
