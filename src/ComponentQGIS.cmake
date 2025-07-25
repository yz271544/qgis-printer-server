message("ComponentQGIS.cmake")

message("CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")

add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)



if (WIN32)
    message("WIN32")
    if(MSVC)
        message("MSVC")
        IF (NOT OSGEO4W_QGIS_SUBDIR OR "${OSGEO4W_QGIS_SUBDIR}" STREQUAL "")
            IF (NOT "$ENV{OSGEO4W_QGIS_SUBDIR}" STREQUAL "")
                SET(OSGEO4W_QGIS_SUBDIR $ENV{OSGEO4W_QGIS_SUBDIR})
            ELSE()
                SET(OSGEO4W_QGIS_SUBDIR qgis)
            ENDIF()
        ENDIF()
        message("OSGEO4W_QGIS_SUBDIR by judge: ${OSGEO4W_QGIS_SUBDIR}")
        set(ENV{LIB} "D:/OSGeo4W/apps/qgis-ltr-dev/bin")
        set(ENV{PATH} "$ENV{PATH};D:/OSGeo4W/apps/qgis-ltr-dev/bin;D:/OSGeo4W/apps/Qt5/bin")
        message("ENV LIB: $ENV{LIB}")
        set(CMAKE_INSTALL_RPATH "D:/OSGeo4W/apps/qgis-ltr-dev/bin;D:/OSGeo4W/apps/qgis-ltr-dev/plugins;D:/OSGeo4W/apps/qgis-ltr-dev")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
        add_compile_options(/wd4819)
        add_definitions(-D_USE_MATH_DEFINES)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        # Ensure consistent runtime library settings
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif()
    #message("Win32 Searching for QGIS in $ENV{QGISPROGRAMFILES}/qgis")
    message("Win32 Searching for QGIS in $ENV{OSGEO4W_ROOT}/apps/${OSGEO4W_QGIS_SUBDIR}")
    # 设置QGIS库的路径
    #set(QGIS_PREFIX_PATH $ENV{QGISPROGRAMFILES}/qgis)
    set(QGIS_PREFIX_PATH $ENV{OSGEO4W_ROOT}/apps/${OSGEO4W_QGIS_SUBDIR})
    message("QGIS_PREFIX_PATH: ${QGIS_PREFIX_PATH}")
    #set(QGIS_DIR $ENV{QGISPROGRAMFILES}/qgis)
    set(QGIS_DIR $ENV{OSGEO4W_ROOT}/apps/${OSGEO4W_QGIS_SUBDIR})
    message("QGIS_DIR: ${QGIS_DIR}")
    set(QGIS_INCLUDE_DIR ${QGIS_DIR}/include)
    message("QGIS_INCLUDE_DIR: ${QGIS_INCLUDE_DIR}")
    set(QGIS_LIB_PATH ${QGIS_DIR}/lib)
    message("QGIS_LIB_PATH: ${QGIS_LIB_PATH}")
    set(QT5_BIN_DIR "$ENV{OSGEO4W_ROOT}/apps/Qt5/bin")
    set(ENV{PATH} "$ENV{PATH};${QT5_BIN_DIR}")
    set(QT5_CMAKE_DIR $ENV{QGISPROGRAMFILES}/Qt5/lib/cmake/Qt5)
    message("QT5_CMAKE_DIR: ${QT5_CMAKE_DIR}")
    set(QT5_DIR $ENV{QGISPROGRAMFILES}/Qt5/lib)
    #    set(QT5_GUI_CMAKE_DIR $ENV{QGISPROGRAMFILES}/Qt5/lib/cmake/Qt5Gui)
    #    set(QT5_QML_CMAKE_DIR $ENV{QGISPROGRAMFILES}/Qt5/lib/cmake/Qt5Qml)
    #    set(QT5_XML_CMAKE_DIR $ENV{QGISPROGRAMFILES}/Qt5/lib/cmake/Qt5Xml)
    #    set(CMAKE_MODULE_PATH ${QGIS_DIR} ${QT5_DIR} ${QT5_GUI_CMAKE_DIR} ${QT5_CMAKE_DIR} ${QT5_QML_CMAKE_DIR} ${QT5_XML_CMAKE_DIR} ${CMAKE_MODULE_PATH})
    set(CMAKE_MODULE_PATH ${QGIS_DIR} ${QGIS_LIB_PATH} ${QT5_DIR} ${QT5_CMAKE_DIR} ${CMAKE_MODULE_PATH})
    message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
    # 使用QGIS安装目录下的FindQGIS.cmake来查找QGIS库（前提是这个文件符合CMake查找模块规范）
    message("CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH}")

    set(CMAKE_PREFIX_PATH $ENV{QT5DIR})
    message("CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH}")
    set(QT_VERSION_MAJOR 5)
    set(QT_MIN_VERSION 5.15.2)
    set(QT_VERSION_BASE "Qt5")
    set(QT_VERSION_BASE_LOWER "qt5")
    set(HAS_KDE_QT5_PDF_TRANSFORM_FIX FALSE CACHE BOOL "Using KDE's Qt 5.15 fork with the PDF brush transform fix")
    set(HAS_KDE_QT5_SMALL_CAPS_FIX FALSE CACHE BOOL "Using KDE's Qt 5.15 fork with the QFont::SmallCaps fix")
    set(HAS_KDE_QT5_FONT_STRETCH_FIX FALSE CACHE BOOL "Using KDE's Qt 5.15 fork with the QFont stretch fix")
    find_package(Qt5 COMPONENTS Core Gui Qml Quick Widgets Xml Svg Multimedia Test 3DCore PrintSupport REQUIRED)
    find_package(Qt5Keychain CONFIG REQUIRED)

    find_path(GDAL_INCLUDE_DIR NAMES gdal.h ogr_api.h gdalwarper.h cpl_conv.h cpl_string.h HINTS $ENV{QGISPROGRAMFILES}/gdal-dev/include)
    find_library(GDAL_LIBRARY NAMES gdal HINTS $ENV{QGISPROGRAMFILES}/gdal-dev/lib)

    message("find yaml-cpp")
    set(YAML_CPP_PATH "$ENV{QGISPROGRAMFILES}/yaml-cpp")
    #    find_package(yaml-cpp REQUIRED PATHS YAML_CPP_PATH)

    message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        find_path(YAML_CPP_INCLUDE_DIR
                NAMES
                yaml-cpp/null.h
                yaml-cpp/yaml.h
                yaml-cpp/traits.h
                PATHS
                "$ENV{QGISPROGRAMFILES}/yaml-cpp/include"
        )
        set(YAML_CPP_NODE_INCLUDE_DIR ${YAML_CPP_INCLUDE_DIR}/node)
        set(YAML_CPP_NODE_DETAIL_INCLUDE_DIR ${YAML_CPP_NODE_INCLUDE_DIR}/detail)
        set(YAML_CPP_CONTRIB_INCLUDE_DIR ${YAML_CPP_INCLUDE_DIR}/contrib)
        find_library(YAML_CPP_LIBRARIES
                NAMES yaml-cppd.lib
                PATHS
                "$ENV{QGISPROGRAMFILES}/yaml-cpp/lib"
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#        message("PROGRAMFILES: $ENV{PROGRAMFILES}")
#        message("PROGRAMFILES(x86): $ENV{PROGRAMFILES\(x86\)}")
#        message("PROGRAM_FILES_X86: ${PROGRAM_FILES_X86}")
#        set(PROGRAM_FILES_X86 $ENV{PROGRAMFILES\(x86\)})
#        set(YAML_CPP_DIR ${PROGRAM_FILES_X86}/YAML_CPP)
#        message("YAML_CPP_DIR: ${YAML_CPP_DIR}")
#        find_package(yaml-cpp REQUIRED PATHS $ENV{QGISPROGRAMFILES}/yaml-cpp)

#        message("YAML_CPP_INCLUDE_DIR: ${YAML_CPP_INCLUDE_DIR}")
#        find_path(YAML_CPP_INCLUDE_DIR
#                NAMES
#                yaml-cpp/null.h
#                yaml-cpp/yaml.h
#                yaml-cpp/traits.h
#                PATHS
#                ${YAML_CPP_DIR}/include
#        )
#        set(YAML_CPP_NODE_INCLUDE_DIR ${YAML_CPP_INCLUDE_DIR}/node)
#        set(YAML_CPP_NODE_DETAIL_INCLUDE_DIR ${YAML_CPP_NODE_INCLUDE_DIR}/detail)
#        set(YAML_CPP_CONTRIB_INCLUDE_DIR ${YAML_CPP_INCLUDE_DIR}/contrib)
#        find_library(YAML_CPP_LIBRARIES
#                NAMES yaml-cpp.lib
#                PATHS
#                ${YAML_CPP_DIR}/lib
#        )
#        message("find_library YAML_CPP_LIBRARIES: ${YAML_CPP_LIBRARIES}")
        find_package(yaml-cpp REQUIRED)
    endif()
    message("yaml-cpp found successfully.")
    message("YAML_CPP_INCLUDE_DIR: ${YAML_CPP_INCLUDE_DIR}")
    #message("YAML_CPP_LIBRARIES: ${YAML_CPP_DIR}/lib/${YAML_CPP_LIBRARIES}")
    message("YAML_CPP_LIBRARIES: ${YAML_CPP_LIBRARIES}")
    message("YAML_CPP_LIBRARY_DIR: ${YAML_CPP_LIBRARY_DIR}")
    #    if (yaml-cpp_FOUND)
    #        message("yaml-cpp found successfully.")
    #        message("YAML_CPP_INCLUDE_DIR: ${YAML_CPP_INCLUDE_DIR}")
    #        message("yaml-cpp_LIBRARIES: ${YAML_CPP_LIBRARIES}")
    #    else()
    #        message(FATAL_ERROR "Failed to find yaml-cpp.")
    #    endif()
    #find_package(PkgConfig REQUIRED)
    find_package(LibArchive REQUIRED)
    find_package(LibZip REQUIRED)
    if(NOT LIBZIP_LIBRARIES AND LibZip_LIBRARIES)
        set(LIBZIP_LIBRARIES ${LibZip_LIBRARIES})
    endif()
    if(NOT LIBZIP_LIBRARIES AND LibZip_LIBRARY)
        set(LIBZIP_LIBRARIES ${LibZip_LIBRARY})
    endif()
    message("LIBZIP_LIBRARIES: ${LIBZIP_LIBRARIES}")
elseif(UNIX)
    message("NIX system - QGISSOURCE: ${QGISSOURCE}")
    if (NOT DEFINED $ENV{QGISSOURCE} OR $ENV{QGISSOURCE} STREQUAL "")
        message("release env")
        set(CMAKE_MODULE_PATH /usr/share/qgis /usr/local/share/qgis)
        message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

        message("find yaml-cpp")
        find_package(yaml-cpp REQUIRED)

        message("find library qgis_app")
        find_library(QGIS_APP_LIBRARY NAMES qgis_app)
        message("QGIS_APP_LIBRARY: ${QGIS_APP_LIBRARY}")
    else()
        message("debug env")
        message("NIX QGIS SOURCE DIR: $ENV{QGISSOURCE}")
        # 设置QGIS库的路径
        set(QGIS_INSTALL_DIR "$ENV{QGISSOURCE}/output" CACHE STRING "QGIS install directory")
        set(QGIS_DIR_CMAKE "$ENV{QGISSOURCE}/output/share/qgis" CACHE STRING "QGIS cmake directory")
        set(QGIS_BUILD_PATH "$ENV{QGISSOURCE}" CACHE STRING "QGIS build directory")
        set(QGIS_PREFIX_PATH "$ENV{QGISSOURCE}/output" CACHE STRING "QGIS prefix path")
        set(QGIS_LIB_PATH "$ENV{QGISSOURCE}/output/lib" CACHE STRING "QGIS lib path")
        message("QGIS_INSTALL_DIR: ${QGIS_INSTALL_DIR}")
        message("QGIS_DIR_CMAKE: ${QGIS_DIR_CMAKE}")
        set(CMAKE_MODULE_PATH ${QGIS_INSTALL_DIR} ${QGIS_DIR_CMAKE} ${QT5_CMAKE_DIR} ${QGIS_LIB_PATH} ${CMAKE_MODULE_PATH})
        message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

        #        find_path(QGIS_3D_INCLUDE_DIR NAMES qgis_3d.h HINTS ${QGIS_INSTALL_DIR}/include/qgis)
        #        find_library(QGIS_3D_LIBRARY NAMES qgis_3d HINTS ${QGIS_LIB_PATH})
        message("find yaml-cpp")
        find_package(yaml-cpp REQUIRED)

        message("find library qgis_app")
        find_library(QGIS_APP_LIBRARY NAMES qgis_app HINTS $ENV{QGISSOURCE}/output/lib)
        message("QGIS_APP_LIBRARY: ${QGIS_APP_LIBRARY}")
    endif()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBARCHIVE REQUIRED libarchive)
    pkg_check_modules(LIBZIP REQUIRED libzip)


    # 检查 CMAKE_BUILD_TYPE 的值
    message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("111")
        set(QGIS_PREFIX_PATH "/usr/local")
        # set(QGIS_PREFIX_PATH "/lyndon/iProject/cpath/QGIS/output")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        message("222")
        set(QGIS_PREFIX_PATH "/usr/local")
        # set(QGIS_PREFIX_PATH "/lyndon/iProject/cpath/QGIS/output")
        # set(QGIS_PREFIX_PATH "/usr")
    else()
        # 可以添加一个默认值或错误处理
        message(FATAL_ERROR "Unsupported CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    endif()

    message("start find Qt package")
    find_package(Qt5 REQUIRED COMPONENTS
            Core
            Gui
            Qml
            Quick
            Widgets
            Xml
            Svg
            Multimedia
            3DRender
            3DCore
            PrintSupport
            Network
            Test
    )
    find_package(Qt5Keychain CONFIG REQUIRED)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    find_path(GDAL_INCLUDE_DIR NAMES gdal.h ogr_api.h gdalwarper.h cpl_conv.h cpl_string.h HINTS /usr/include/gdal)
    find_library(GDAL_LIBRARY NAMES gdal HINTS /usr/lib /usr/lib/x86_64-linux-gnu)


else()
    message("Unsupported platform")
endif()

# 生成 config.h 头文件
#message("CMAKE_HOME_DIRECTORY:${CMAKE_HOME_DIRECTORY}")
#configure_file(config.h.in ${CMAKE_HOME_DIRECTORY}/config.h)

message("start find qgis package")
find_package(QGIS REQUIRED)
find_path(QGIS_3D_INCLUDE_DIR NAMES qgis_3d.h HINTS ${QGIS_INCLUDE_DIR})
find_library(QGIS_3D_LIBRARY NAMES qgis_3d HINTS ${QGIS_LIB_PATH})

message("QGIS_3D_LIBRARY: ${QGIS_3D_LIBRARY}")

message("QGIS_INCLUDE_DIR: ${QGIS_INCLUDE_DIR}")
include_directories(${QGIS_INCLUDE_DIR})
message("QGIS_3D_INCLUDE_DIR: ${QGIS_3D_INCLUDE_DIR}")
include_directories(${QGIS_3D_INCLUDE_DIR})
message("Qt5Core_INCLUDE_DIRS: ${Qt5Core_INCLUDE_DIRS}")
include_directories(${Qt5Core_INCLUDE_DIRS})
message("Qt5Gui_INCLUDE_DIRS: ${Qt5Gui_INCLUDE_DIRS}")
include_directories(${Qt5Gui_INCLUDE_DIRS})
message("Qt5Qml_INCLUDE_DIRS: ${Qt5Qml_INCLUDE_DIRS}")
include_directories(${Qt5Qml_INCLUDE_DIRS})
message("Qt5Quick_INCLUDE_DIRS: ${Qt5Quick_INCLUDE_DIRS}")
include_directories(${Qt5Quick_INCLUDE_DIRS})
message("Qt5Widgets_INCLUDE_DIRS: ${Qt5Widgets_INCLUDE_DIRS}")
include_directories(${Qt5Widgets_INCLUDE_DIRS})
message("Qt5Multimedia_INCLUDE_DIRS: ${Qt5Multimedia_INCLUDE_DIRS}")
include_directories(${Qt5Multimedia_INCLUDE_DIRS})
message("Qt5Xml_INCLUDE_DIRS: ${Qt5Xml_INCLUDE_DIRS}")
include_directories(${Qt5Xml_INCLUDE_DIRS})
message("Qt5Svg_INCLUDE_DIRS: ${Qt5Svg_INCLUDE_DIRS}")
include_directories(${Qt5Svg_INCLUDE_DIRS})
message("Qt5PrintSupport_INCLUDE_DIRS: ${Qt5PrintSupport_INCLUDE_DIRS}")
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
message("Qt53DCore_INCLUDE_DIRS: ${Qt53DCore_INCLUDE_DIRS}")
include_directories(${Qt53DCore_INCLUDE_DIRS})
message("Qt5Network_INCLUDE_DIRS: ${Qt5Network_INCLUDE_DIRS}")
include_directories(${Qt5Network_INCLUDE_DIRS})
if(WIN32)
    if(MSVC)
        message("LibArchive_INCLUDE_DIR: ${LibArchive_INCLUDE_DIR}")
        message("LibZip_INCLUDE_DIR: ${LibZip_INCLUDE_DIR}")
        include_directories(${LibArchive_INCLUDE_DIR} ${LibZip_INCLUDE_DIR})
    endif()
elseif(UNIX)
    message("LIBARCHIVE_INCLUDE_DIRS: ${LIBARCHIVE_INCLUDE_DIRS}")
    message("LIBZIP_INCLUDE_DIRS: ${LIBZIP_INCLUDE_DIRS}")
    include_directories(${LIBARCHIVE_INCLUDE_DIRS} ${LIBZIP_INCLUDE_DIRS})
endif()

find_path(Qt53DRender_INCLUDE_DIRS
        NAMES qt3drender_global.h
        HINTS
        ${CMAKE_PREFIX_PATH}/include/Qt3DRender
        $ENV{QGISPROGRAMFILES}/Qt5/include/Qt3DRender
)
message("Qt53DRender_INCLUDE_DIRS: ${Qt53DRender_INCLUDE_DIRS}")
include_directories(${Qt53DRender_INCLUDE_DIRS})

find_library(Qt53DRender_LIBRARY
        NAMES Qt53DRender
        HINTS 
        ${CMAKE_PREFIX_PATH}/lib
        $ENV{QGISPROGRAMFILES}/Qt5/lib
)

message("Qt53DRender_LIBRARY: ${Qt53DRender_LIBRARY}")

message("GDAL_INCLUDE_DIR: ${GDAL_INCLUDE_DIR}")
include_directories(${GDAL_INCLUDE_DIR})

include_directories(${YAML_CPP_INCLUDE_DIR} ${YAML_CPP_NODE_INCLUDE_DIR} ${YAML_CPP_CONTRIB_INCLUDE_DIR})

# Add library directories
if(MSVC)
    link_directories(${QGIS_PREFIX_PATH}/lib /usr/lib/x86_64-linux-gnu ${YAML_CPP_LIBRARY_DIR}/${YAML_CPP_LIBRARIES}.lib)
else()
    link_directories(${QGIS_PREFIX_PATH}/lib /usr/lib/x86_64-linux-gnu ${YAML_CPP_LIBRARIES} ${YAML_CPP_NODE_DETAIL_INCLUDE_DIR})
endif()


message("QGIS_CORE_LIBRARY: ${QGIS_CORE_LIBRARY}")
message("QGIS_GUI_LIBRARY: ${QGIS_GUI_LIBRARY}")
message("QGIS_3D_LIBRARY: ${QGIS_3D_LIBRARY}")
