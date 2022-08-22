set(SRC_LIST searchservice.cc ../search.pb.cc)

add_executable(rpcprovider ${SRC_LIST})
target_link_libraries(rpcprovider mprpc protobuf mysqlclient hiredis pthread zookeeper_mt)