include(../src/ComponentQGIS.cmake)
set(BINARY ${CMAKE_PROJECT_NAME}_tst)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
message("TEST_SOURCES: ${TEST_SOURCES}")
set(SOURCES ${TEST_SOURCES}
        DegreeUtilTest.cpp)

# Include util source files
file(GLOB_RECURSE UTIL_SOURCES LIST_DIRECTORIES false ../src/utils/*.h ../src/utils/*.cpp)
message("UTIL_SOURCES: ${UTIL_SOURCES}")
list(APPEND SOURCES ${UTIL_SOURCES})

add_executable(${BINARY} ${SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

# 获取 oatpp 的版本号
if(${oatpp_VERSION} VERSION_LESS "1.4.0")
    message("${oatpp_VERSION} < 1.4.0")
    target_compile_definitions(${CMAKE_PROJECT_NAME}_tst PRIVATE OATPP_VERSION_LESS_1_4_0)
else()
    message("${oatpp_VERSION} >= 1.4.0")
    target_compile_definitions(${CMAKE_PROJECT_NAME}_tst PRIVATE OATPP_VERSION_GREATER_EQUAL_1_4_0)
endif()

# 添加自定义命令，在构建时复制 conf 目录（构建阶段）
add_custom_command(
        TARGET ${BINARY} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/conf
        ${CMAKE_BINARY_DIR}/tests/conf
        COMMENT "Copying conf directory to build directory"
)

# 链接src生成的lib库和gtest库
target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib
        yaml-cpp
        spdlog::spdlog
        oatpp::oatpp
        oatpp::oatpp-curl
        ${QGIS_CORE_LIBRARY} ${QGIS_GUI_LIBRARY} ${QGIS_3D_LIBRARY}
        Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::Xml Qt5::Svg Qt5::Test  Qt5::3DCore
        ${Qt53DRender_LIBRARY} ${GDAL_LIBRARY} ${YAML_CPP_LIBRARIES}
        ${LIBARCHIVE_LIBRARIES} ${LIBZIP_LIBRARIES}
        gtest)
