#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

// 聊天服务器
class ChatServer
{
public:
    // 初始化聊天服务器
    ChatServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg);

    // 启动服务器
    void start();

private:
    // 上报链接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &conn);

    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &, Buffer *, muduo::Timestamp);

    // 服务器名称,组合的muduo库，实现服务器功能的类对象
    TcpServer _server;

    // 指向事件循环的指针
    EventLoop *_loop;
};

#endif