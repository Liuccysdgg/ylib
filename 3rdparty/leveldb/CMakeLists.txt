cmake_minimum_required(VERSION 3.5)

# 获取上级目录名做为库名
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(LIBRARY_NAME ${CURRENT_DIR} NAME)

file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/3rdparty/${LIBRARY_NAME}/src/*.*")


if(MSVC)
	add_definitions(
		-D_WINDOWS
		-DLEVELDB_COMPILE_LIBRARY
		-DLEVELDB_PLATFORM_WINDOWS=1
		-D_UNICODE
		-DUNICODE
		-D_HAS_EXCEPTIONS=0 
	)
endif()

# 创建库
add_library(${LIBRARY_NAME} ${SOURCE_FILES})

# 将包含目录与目标相关联，这样只有在编译此库时才会包含这些目录
target_include_directories(${LIBRARY_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/3rdparty/${LIBRARY_NAME}/include")


set_property(TARGET ${LIBRARY_NAME} PROPERTY FOLDER "3rdparty") 