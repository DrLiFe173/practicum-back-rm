cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/http_server.cpp
	src/http_server.h
	src/sdk.h
	src/model.h
	src/model.cpp
	src/tagged.h
	src/boost_json.cpp
	src/json_loader.h
	src/json_loader.cpp
	src/request_handler.cpp
	src/request_handler.h
	src/response.cpp
	src/response.h
	src/logger.h
	src/logger.cpp
	src/magic_defs.h
	src/api_handler.h
	src/api_handler.cpp
	src/url_handler.h
	src/url_handler.cpp
)
target_include_directories(game_server PRIVATE CONAN_PKG::boost) 
target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost)
