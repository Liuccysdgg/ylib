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
            $<$<CONFIG:Debug>:"C:/Qt/Qt5.14.2/5.14.2/msvc2017/include">)
        # 为Release配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Release>:"C:/Qt/Qt5.14.2/5.14.2/msvc2017/include">)
        # 为ReleaseMT配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:ReleaseMT>:"C:/Qt/Qt-5.14.2/include">)
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        # 为Debug配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Debug>:"C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/include">)
        # 为Release配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:Release>:"C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/include">)
        # 为ReleaseMT配置设置特定的include目录
        target_include_directories(${LIBRARY_NAME} PUBLIC 
            $<$<CONFIG:ReleaseMT>:"C:/Qt/Qt-5.14.2/include">)
    endif()
endif()


########################  安装  ########################
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)