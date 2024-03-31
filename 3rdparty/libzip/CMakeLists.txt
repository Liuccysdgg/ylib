cmake_minimum_required(VERSION 2.8)

# 获取上级目录名做为库名
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(LIBRARY_NAME ${CURRENT_DIR} NAME)


include_directories(${PROJECT_SOURCE_DIR}/3rdparty/zlib)

file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/3rdparty/${LIBRARY_NAME}/*.c")

# 预处理宏等
add_definitions(-DZIP_STATIC)

# 创建库
add_library(${LIBRARY_NAME} ${SOURCE_FILES})

# 将包含目录与目标相关联，这样只有在编译此库时才会包含这些目录
target_include_directories(${LIBRARY_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/3rdparty/${LIBRARY_NAME}")

