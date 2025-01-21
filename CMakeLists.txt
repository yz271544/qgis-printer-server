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



add_executable(${PROJECT_NAME} src/main.cpp
        src/core/starter.h
        src/core/starter.cpp
        src/core/starterregister.cpp
        src/core/starterregister.h
        src/core/confstarter.cpp
        src/core/confstarter.h
        src/core/loggerstarter.cpp
        src/core/loggerstarter.h
        src/core/webstarter.cpp
        src/core/webstarter.h
        src/core/handler/hellohandler.h
        src/core/handler/hellohandler.cpp
        src/core/hookstarter.cpp
        src/core/hookstarter.h
        src/core/error/egonexception.cpp
        src/core/error/egonexception.h
        src/core/bootapplication.cpp
        src/core/bootapplication.h
)

# 添加自定义命令，在构建时复制 conf 目录（构建阶段）
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/conf
        ${CMAKE_BINARY_DIR}/conf
        COMMENT "Copying conf directory to build directory"
)


target_link_libraries(${PROJECT_NAME}
        PRIVATE
        yaml-cpp
        spdlog::spdlog
        oatpp::oatpp)