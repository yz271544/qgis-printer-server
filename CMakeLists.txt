cmake_minimum_required(VERSION 3.25)
project(cboot)

include(FetchContent)

set($ENV{proxy_http} "http://127.0.0.1:10808")
set($ENV{proxys_http} "http://127.0.0.1:10808")

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4267)
endif()

# 使用 FetchContent 下载并构建 yaml-cpp
FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master
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

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        yaml-cpp
        spdlog::spdlog
        oatpp::oatpp)