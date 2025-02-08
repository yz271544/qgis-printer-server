cmake_minimum_required(VERSION 3.25)
project(jingweiprinter)

include(FetchContent)

#set($ENV{http_proxy} "http://127.0.0.1:10808")
#set($ENV{https_proxy} "http://127.0.0.1:10808")

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
#    set(CMAKE_AUTOMOC ON)
#    set(CMAKE_AUTOUIC ON)
#    set(CMAKE_AUTORCC ON)
endif()

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message("CMAKE_CXX_STANDARD_REQUIRED: ${CMAKE_CXX_STANDARD_REQUIRED}")
message("CMAKE_AUTOMOC: ${CMAKE_AUTOMOC}")
message("CMAKE_AUTOUIC: ${CMAKE_AUTOUIC}")
message("CMAKE_AUTORCC: ${CMAKE_AUTORCC}")


if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /ignore:4099")
    add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    add_compile_options(/wd4267)
endif()
#if (MSVC)
#    target_compile_options(${PROJECT_NAME} PRIVATE /wd4267)
#endif()

# 生成 config.h 头文件
message("CMAKE_HOME_DIRECTORY: ${CMAKE_HOME_DIRECTORY}")
configure_file(${CMAKE_HOME_DIRECTORY}/src/config.h.in ${CMAKE_HOME_DIRECTORY}/src/config.h)

# 使用 FetchContent 下载并构建 yaml-cpp
FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# 使用 FetchContent 下载并构建 spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.9.2
)
FetchContent_MakeAvailable(spdlog)

find_package(oatpp REQUIRED)
find_package(oatpp-curl REQUIRED)
#find_package(gtest REQUIRED)

message("oatpp_VERSION: ${oatpp_VERSION}")
message("oatpp-curl_VERSION: ${oatpp_VERSION}")

include_directories(src)
add_subdirectory(src)
add_subdirectory(lib/googletest)
add_subdirectory(tests)

