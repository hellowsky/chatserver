 cmake_minimum_required(VERSION 3.0)
#工程名
 project(chat)

 #配置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 设置构建类型
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)


#配置最终的可执行文件输出的路径
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)



#加载子目录
add_subdirectory(src)
