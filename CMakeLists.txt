cmake_minimum_required(VERSION 3.0)
project(chat)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/include/server/kafka)
include_directories(${PROJECT_SOURCE_DIR}/include/server/log)
include_directories(${PROJECT_SOURCE_DIR}/lib/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/rpcserver)
include_directories(${PROJECT_SOURCE_DIR}/src)
# -L
link_directories(/usr/lib64/mysql)
link_directories(${PROJECT_SOURCE_DIR}/lib)
# 加载子目录
add_subdirectory(src)
add_subdirectory(rpcserver)