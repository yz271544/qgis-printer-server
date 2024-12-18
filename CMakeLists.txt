cmake_minimum_required(VERSION 3.30)
project(cboot)

set(CMAKE_CXX_STANDARD 20)

find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(oatpp REQUIRED)

add_executable(cboot src/main.cpp
        src/core/starter.h
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
)

target_link_libraries(cboot yaml-cpp spdlog::spdlog oatpp::oatpp)