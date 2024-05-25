cmake_minimum_required(VERSION 3.5)
find_package(Threads REQUIRED)


# 获取上级目录名做为库名
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(EXECUTABLE_NAME ${CURRENT_DIR} NAME)

file(GLOB CLIENT_SOURCE_FILES "${PROJECT_SOURCE_DIR}/test/${EXECUTABLE_NAME}/client.cpp")
file(GLOB SERVER_SOURCE_FILES "${PROJECT_SOURCE_DIR}/test/${EXECUTABLE_NAME}/server.cpp")

if(MSVC)
	include_directories(${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Include)
endif()

if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
    set(HPSOCKET_DIR "x86")
elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
    set(HPSOCKET_DIR "x64")
endif()



# 创建EXE CLIENT
add_executable(${EXECUTABLE_NAME}_client ${CLIENT_SOURCE_FILES})
# 创建EXE SERVER
add_executable(${EXECUTABLE_NAME}_server ${SERVER_SOURCE_FILES})
if(MSVC)
target_link_libraries(${EXECUTABLE_NAME}_client PRIVATE 
	kernel32.lib
	Shell32.lib
	Iphlpapi.lib
	$<$<CONFIG:ReleaseMT>:${CMAKE_BINARY_DIR}/lib/ReleaseMT/ynet.lib>
	$<$<CONFIG:ReleaseMT>:${CMAKE_BINARY_DIR}/lib/ReleaseMT/ybase.lib>
	$<$<CONFIG:ReleaseMT>:${CMAKE_BINARY_DIR}/lib/ReleaseMT/yutil.lib>
	$<$<CONFIG:ReleaseMT>:${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Lib/HPSocket/${HPSOCKET_DIR}/static/HPSocket.lib>
	$<$<CONFIG:Debug>:${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Lib/HPSocket/${HPSOCKET_DIR}/HPSocket_D.lib>
	$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/lib/Debug/ynet.lib>
	$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/lib/Debug/ybase.lib>
	$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/lib/Debug/yutil.lib>
	$<$<CONFIG:Release>:${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Lib/HPSocket/${HPSOCKET_DIR}/HPSocket.lib>
	$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/lib/Release/ynet.lib>
	$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/lib/Release/ybase.lib>
	$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/lib/Release/yutil.lib>
	
	
)
target_link_libraries(${EXECUTABLE_NAME}_server PRIVATE 
	kernel32.lib
	Shell32.lib
	Iphlpapi.lib
	$<$<CONFIG:ReleaseMT>:${CMAKE_BINARY_DIR}/lib/ReleaseMT/ynet.lib>
	$<$<CONFIG:ReleaseMT>:${CMAKE_BINARY_DIR}/lib/ReleaseMT/ybase.lib>
	$<$<CONFIG:ReleaseMT>:${CMAKE_BINARY_DIR}/lib/ReleaseMT/yutil.lib>
	$<$<CONFIG:ReleaseMT>:${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Lib/HPSocket/${HPSOCKET_DIR}/static/HPSocket.lib>
	$<$<CONFIG:Debug>:${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Lib/HPSocket/${HPSOCKET_DIR}/HPSocket_D.lib>
	$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/lib/Debug/ynet.lib>
	$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/lib/Debug/ybase.lib>
	$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/lib/Debug/yutil.lib>
	$<$<CONFIG:Release>:${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Lib/HPSocket/${HPSOCKET_DIR}/HPSocket.lib>
	$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/lib/Release/ynet.lib>
	$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/lib/Release/ybase.lib>
	$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/lib/Release/yutil.lib>
)
else()
target_link_libraries(${EXECUTABLE_NAME}_client PRIVATE 
	m
	${CMAKE_BINARY_DIR}/lib/libynet.a
	${CMAKE_BINARY_DIR}/lib/libybase.a
	${CMAKE_BINARY_DIR}/lib/libyutil.a
	hpsocket
	Threads::Threads
)
target_link_libraries(${EXECUTABLE_NAME}_server PRIVATE 
	m
	${CMAKE_BINARY_DIR}/lib/libynet.a
	${CMAKE_BINARY_DIR}/lib/libybase.a
	${CMAKE_BINARY_DIR}/lib/libyutil.a
	hpsocket
	Threads::Threads
)
endif()


# 设置 Visual Studio 中的文件夹
set_property(TARGET ${EXECUTABLE_NAME}_client PROPERTY FOLDER "test")
set_property(TARGET ${EXECUTABLE_NAME}_server PROPERTY FOLDER "test")