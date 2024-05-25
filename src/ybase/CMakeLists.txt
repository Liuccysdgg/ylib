cmake_minimum_required(VERSION 3.5)
# 获取上级目录名做为库名
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(LIBRARY_NAME ${CURRENT_DIR} NAME)

    file(GLOB SOURCE_FILES 
        "${PROJECT_SOURCE_DIR}/include/${LIBRARY_NAME}/*.*"
        "${PROJECT_SOURCE_DIR}/src/${LIBRARY_NAME}/src/*.cpp"
    )
# 创建库
add_library(${LIBRARY_NAME} ${SOURCE_FILES})

########################  安装  ########################
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)


set_property(TARGET ${LIBRARY_NAME} PROPERTY FOLDER "core")