cmake_minimum_required(VERSION 3.30)
project(cboot)

set(CMAKE_CXX_STANDARD 20)

find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)

add_executable(cboot src/main.cpp
        src/core/starter.cpp
        src/core/starter.h
        src/core/starterregister.cpp
        src/core/starterregister.h
        src/core/confstarter.cpp
        src/core/confstarter.h
        src/core/loggerstarter.cpp
        src/core/loggerstarter.h
)

target_link_libraries(cboot yaml-cpp spdlog::spdlog)