include(ComponentQGIS.cmake)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES})
message("SOURCES: ${SOURCES}")

add_executable(${PROJECT_NAME} ${SOURCES}
        core/qtwebstarter.cpp
        core/qtwebstarter.h
        utils/DegreeUtil.cpp
        utils/DegreeUtil.h)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

# 为 jingweiprinter 启用 AUTOMOC
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
set_target_properties(${PROJECT_NAME}_lib PROPERTIES AUTOMOC ON)

message("set target_compile_definitions OATPP_VERSION_LESS_1_4_0 oatpp_VERSION: ${oatpp_VERSION}")
# 获取 oatpp 的版本号
if(${oatpp_VERSION} VERSION_LESS "1.4.0")
  message("${oatpp_VERSION} < 1.4.0")
  target_compile_definitions(${PROJECT_NAME} PRIVATE OATPP_VERSION_LESS_1_4_0)
  target_compile_definitions(${PROJECT_NAME}_lib PRIVATE OATPP_VERSION_LESS_1_4_0)
else()
  message("${oatpp_VERSION} >= 1.4.0")
  target_compile_definitions(${PROJECT_NAME} PRIVATE OATPP_VERSION_GREATER_EQUAL_1_4_0)
  target_compile_definitions(${PROJECT_NAME}_lib PRIVATE OATPP_VERSION_GREATER_EQUAL_1_4_0)
endif()


# 添加自定义命令，在构建时复制 conf 目录（构建阶段）
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/conf
        ${CMAKE_BINARY_DIR}/src/conf
        COMMENT "Copying conf directory to build directory"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${yaml-cpp_SOURCE_DIR}/include ${spdlog_SOURCE_DIR}/include ${oatpp_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME}_lib PRIVATE ${yaml-cpp_SOURCE_DIR}/include ${spdlog_SOURCE_DIR}/include ${oatpp_INCLUDE_DIRS})

if (WIN32)
  if(MSVC)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            yaml-cpp::yaml-cpp
            spdlog::spdlog
            oatpp::oatpp
            oatpp::oatpp-curl
            ${QGIS_CORE_LIBRARY}
            ${QGIS_GUI_LIBRARY}
            ${QGIS_3D_LIBRARY}
            Qt5::Core
            Qt5::Gui
            Qt5::Qml
            Qt5::Quick
            Qt5::Widgets
            Qt5::Xml
            Qt5::Svg
            Qt5::Test
            Qt5::Network
            Qt5::3DCore
            ${Qt53DRender_LIBRARY}
            ${GDAL_LIBRARY}
            ${YAML_CPP_LIBRARY_DIR}/${YAML_CPP_LIBRARIES}.lib
            ${LibArchive_LIBRARY}
            libzip::zip
    )

    target_link_libraries(${PROJECT_NAME}_lib
            PRIVATE
            yaml-cpp::yaml-cpp
            spdlog::spdlog
            oatpp::oatpp
            oatpp::oatpp-curl
            gtest
            ${QGIS_CORE_LIBRARY}
            ${QGIS_GUI_LIBRARY}
            ${QGIS_3D_LIBRARY}
            Qt5::Core
            Qt5::Gui
            Qt5::Qml
            Qt5::Quick
            Qt5::Widgets
            Qt5::Xml
            Qt5::Svg
            Qt5::Test
            Qt5::Network
            Qt5::3DCore
            ${Qt53DRender_LIBRARY}
            ${GDAL_LIBRARY}
            ${YAML_CPP_LIBRARY_DIR}/${YAML_CPP_LIBRARIES}.lib
            ${LibArchive_LIBRARY}
            libzip::zip
    )
  endif()
else()
 target_link_libraries(${PROJECT_NAME}
          PRIVATE
          yaml-cpp
          spdlog::spdlog
          oatpp::oatpp
          oatpp::oatpp-curl
          ${QGIS_CORE_LIBRARY}
          ${QGIS_GUI_LIBRARY}
          ${QGIS_3D_LIBRARY}
          Qt5::Core
          Qt5::Gui
          Qt5::Qml
          Qt5::Quick
          Qt5::Widgets
          Qt5::Xml
          Qt5::Svg
          Qt5::Test
          Qt5::Network
          Qt5::3DCore
          ${Qt53DRender_LIBRARY}
          ${GDAL_LIBRARY}
          ${YAML_CPP_LIBRARIES}
          ${LIBARCHIVE_LIBRARIES}
          ${LIBZIP_LIBRARIES}
  )

  target_link_libraries(${PROJECT_NAME}_lib
          PRIVATE
          yaml-cpp
          spdlog::spdlog
          oatpp::oatpp
          oatpp::oatpp-curl
          gtest
          ${QGIS_CORE_LIBRARY}
          ${QGIS_GUI_LIBRARY}
          ${QGIS_3D_LIBRARY}
          Qt5::Core
          Qt5::Gui
          Qt5::Qml
          Qt5::Quick
          Qt5::Widgets
          Qt5::Xml
          Qt5::Svg
          Qt5::Test
          Qt5::Network
          Qt5::3DCore
          ${Qt53DRender_LIBRARY}
          ${GDAL_LIBRARY}
          ${YAML_CPP_LIBRARIES}
          ${LIBARCHIVE_LIBRARIES}
          ${LIBZIP_LIBRARIES}
  )
endif()
# install
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

