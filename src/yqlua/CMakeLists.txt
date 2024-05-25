cmake_minimum_required(VERSION 3.5)
if(MSVC)
    # 获取上级目录名做为库名
    get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
    get_filename_component(LIBRARY_NAME ${CURRENT_DIR} NAME)
    file(GLOB SOURCE_FILES 
        "${PROJECT_SOURCE_DIR}/include/${LIBRARY_NAME}/*.*"
        "${PROJECT_SOURCE_DIR}/src/${LIBRARY_NAME}/src/*.cpp"
    )
    # 创建库
    add_library(${LIBRARY_NAME} ${SOURCE_FILES})

    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        # 为Debug配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Debug>:${PROJECT_SOURCE_DIR}/3rdparty/qt/include>)
        # 为Release配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Release>:${PROJECT_SOURCE_DIR}/3rdparty/qt/include>)
        # 为ReleaseMT配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:ReleaseMT>:${PROJECT_SOURCE_DIR}/3rdparty/qt/include>)
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        # 为Debug配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Debug>:${PROJECT_SOURCE_DIR}/3rdparty/qt/include>)
        # 为Release配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Release>:${PROJECT_SOURCE_DIR}/3rdparty/qt/include>)
        # 为ReleaseMT配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:ReleaseMT>:${PROJECT_SOURCE_DIR}/3rdparty/qt/include>)
    endif()
endif()


########################  安装  ########################
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)

set_property(TARGET ${LIBRARY_NAME} PROPERTY FOLDER "core")