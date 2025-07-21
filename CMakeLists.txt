cmake_minimum_required(VERSION 3.25)
project(jingweiprinter)

include(FetchContent)

set($ENV{http_proxy} "http://127.0.0.1:10808")
set($ENV{https_proxy} "http://127.0.0.1:10808")

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message("CMAKE_CXX_STANDARD_REQUIRED: ${CMAKE_CXX_STANDARD_REQUIRED}")
message("CMAKE_AUTOMOC: ${CMAKE_AUTOMOC}")
message("CMAKE_AUTOUIC: ${CMAKE_AUTOUIC}")
message("CMAKE_AUTORCC: ${CMAKE_AUTORCC}")


if (MSVC)
    set(ENV{PKG_CONFIG_PATH} "D:/OSGeo4W/lib/pkgconfig")
    #set(ENV{LIB} "D:/OSGeo4W/apps/qgis-ltr-dev/bin;D:/OSGeo4W/lib;D:/OSGeo4W/bin;D:/OSGeo4W/apps")
    #set(ENV{PATH} "$ENV{PATH};D:/cygwin64/bin;D:/OSGeo4W/apps/qgis-ltr-dev/bin;D:/OSGeo4W/apps/Qt5/bin;D:/OSGeo4W/lib;D:/OSGeo4W/bin;D:/OSGeo4W/apps")
    #message("ENV LIB: $ENV{LIB}")
    # D:/OSGeo4W/lib/cmake/CURL;
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};D:/OSGeo4W;D:/OSGeo4W/lib/pkgconfig;D:/OSGeo4W/bin;D:/OSGeo4W/lib;D:/OSGeo4W/apps")
    set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH};D:/OSGeo4W/include")
    set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH};D:/OSGeo4W/lib")

    #find_package(CURL REQUIRED)
    #include_directories(${CURL_INCLUDE_DIRS})
    #link_libraries(${CURL_LIBRARIES})

    #set(OPENSSL_ROOT_DIR "D:/OSGeo4W")

    #find_package(OpenSSL REQUIRED)
    #include_directories(${OPENSSL_INCLUDE_DIR})
    #link_libraries(${OPENSSL_LIBRARIES})

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /ignore:4099")
    add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    add_compile_options(/wd4267)
    # 在 find_package(oatpp REQUIRED) 之前添加
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};D:/iProject/cpath/oatpp-installed")
    # 在 find_package(oatpp REQUIRED) 之前添加
    set(oatpp_DIR "D:/iProject/cpath/oatpp-installed/lib/cmake/oatpp-1.3.0")
    #set(CMAKE_PREFIX_PATH D:/iProject/cpath/vcpkg/packages/oatpp_x64-windows 
    #D:/iProject/cpath/vcpkg/packages/oatpp-curl_x64-windows
    #D:/iProject/cpath/vcpkg/packages/curl_x64-windows
    #D:/iProject/cpath/vcpkg/packages/zlib_x64-windows)
endif()
#if (MSVC)
#    target_compile_options(${PROJECT_NAME} PRIVATE /wd4267)
#endif()

# 生成 config.h 头文件
message("CMAKE_HOME_DIRECTORY: ${CMAKE_HOME_DIRECTORY}")
configure_file(${CMAKE_HOME_DIRECTORY}/src/config.h.in ${CMAKE_HOME_DIRECTORY}/src/config.h)

if (NOT DEFINED $ENV{MASTER_RELEASE} AND $ENV{MASTER_RELEASE} STREQUAL "")
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
    #set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};D:/OSGeo4W/lib/cmake/CURL")
    # 使用 FetchContent 下载并构建 oatpp-curl
    FetchContent_Declare(
            oatpp-curl
            GIT_REPOSITORY https://github.com/oatpp/oatpp-curl.git
            GIT_TAG 1.3.0
    )
    FetchContent_MakeAvailable(oatpp-curl)
else()
    set(CMAKE_PREFIX_PATH "D:/iProject/cpath/oatpp-curl-installed;D:/iProject/cpath/oatpp-installed;D:/iProject/cpath/spdlog-installed;D:/iProject/cpath/yaml-cpp-installed")
    find_package(yaml-cpp REQUIRED)
    find_package(spdlog REQUIRED)
    find_package(oatpp REQUIRED)
    find_package(oatpp-curl REQUIRED)
endif()
#find_package(oatpp REQUIRED)
#find_package(oatpp-curl REQUIRED)
#find_package(gtest REQUIRED)

message("oatpp_VERSION: ${oatpp_VERSION}")
message("oatpp-curl_VERSION: ${oatpp_VERSION}")

include_directories(src)
add_subdirectory(src)
add_subdirectory(lib/googletest)
add_subdirectory(tests)

