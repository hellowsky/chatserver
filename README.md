# chatserver
基于Nginx tcp负载均衡的集群聊天服务器

编译方式
cd build
rm -rf *
cmake ..
make

./ChatServer 127.0.0.1 6001    ./ChatServer 127.0.0.1 6002
