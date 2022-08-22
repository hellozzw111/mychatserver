# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
set(SRC_LIST main.cpp ../../rpcserver/search.pb.cc)

# 指定生成可执行文件
add_executable(ChatClient ${SRC_LIST})
# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(ChatClient pthread mprpc protobuf zookeeper_mt)