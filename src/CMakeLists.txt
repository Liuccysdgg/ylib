cmake_minimum_required(VERSION 3.5)




# Recursively get all source files
file(GLOB_RECURSE SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADER_FILES
    "${PROJECT_SOURCE_DIR}/include/*.h"
)


if(MSVC)
    set(MYSQL_CPPCONN_INCLUDE  CACHE PATH "mysql-connector include")
    include_directories(${MYSQL_CPPCONN_INCLUDE})
	include_directories(${PROJECT_SOURCE_DIR}/3rdparty/HP-Socket/Windows/Include)
    add_compile_options(/utf-8)
endif()


# Add the executable or library target
add_library(${PROJECT_NAME} ${SOURCE_FILES}  ${HEADER_FILES})

foreach(source IN LISTS SOURCE_FILES)
    get_filename_component(source_path "${source}" PATH)
    file(RELATIVE_PATH source_path_rel "${PROJECT_SOURCE_DIR}" "${source_path}")
    string(REPLACE "/" "\\" source_path_rel_win "${source_path_rel}") 
    source_group(${source_path_rel_win} FILES "${source}")
endforeach()


# Group header files by their relative path
foreach(header IN LISTS HEADER_FILES)
    get_filename_component(header_path "${header}" PATH)
    file(RELATIVE_PATH header_path_rel "${PROJECT_SOURCE_DIR}" "${header_path}")
    string(REPLACE "/" "\\" header_path_rel_win "${header_path_rel}")
    source_group(${header_path_rel_win} FILES "${header}")
endforeach()

########################  安装  ########################
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/db 
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/base 
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/util 
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/net 
DESTINATION include)


