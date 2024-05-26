cmake_minimum_required(VERSION 3.5)

########################  项目名称  #######################  
set(PROJECT_NAME ylib)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# 设置自定义配置类型
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;ReleaseMT" CACHE STRING "Build config types" FORCE)

# 开启文件夹
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

########################  生成路径  ########################  
# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 设置共享库输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置静态库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置中间目录
if(MSVC)
    set(CMAKE_VS_INTERMEDIATE_DIRECTORY "${CMAKE_BINARY_DIR}/obj")
endif()
########################  头文件  ########################  
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty)
if(MSVC)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        include_directories("D:/3rdparty/OpenSSL-Win32/include")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        include_directories("D:/3rdparty/OpenSSL-Win64/include")
    else()
        include_directories("D:/3rdparty/OpenSSL-Win64/include")
    endif()
    
else()
    include_directories(/usr/local/include)
endif()

########################  编译选项  ########################
# 预处理宏等
add_definitions(-DHPSOCKET_STATIC_LIB)
# C++等级
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
# MSVC编译器
if(MSVC)
    # 设置自定义构建类型的编译标志
    set(CMAKE_CXX_FLAGS_RELEASEMT "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "Flags for ReleaseMT" FORCE)
    set(CMAKE_C_FLAGS_RELEASEMT "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "Flags for ReleaseMT" FORCE)
    # 警告等级
    add_compile_options(/W3 /wd4819)
else()
    
endif()
########################  子项目  ########################
add_subdirectory(src)
########################  三方库  ########################
add_subdirectory(3rdparty/zlib)
add_subdirectory(3rdparty/lua)
add_subdirectory(3rdparty/sqlite3)
if(MSVC)
add_subdirectory(3rdparty/libzip)
endif()
add_subdirectory(3rdparty/leveldb)



