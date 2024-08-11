#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

// 处理服务器cltr+c结束后,重置user的状态信息
void resetHandler(int)
{
    ChatService::getInstance()->reset();
  
    exit(0);
}

int main(int argc, char **argv)
{

     if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);

    // 创建事件循环
    EventLoop loop;

    // 创建服务器地址
    //InetAddress addr("127.0.0.1", 8080);
    InetAddress addr(ip, port);

    // 创建服务器
    ChatServer server(&loop, addr, "ChatServer");

    // 启动服务器
    server.start();

    // 启动事件循环
    loop.loop();

    return 0;
}