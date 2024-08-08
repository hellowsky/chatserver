#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"

#include <functional>
#include <string>
using namespace std;
using namespace std::placeholders;
using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;

// 初始化服务器聊天器
ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

// 上报连接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {   
        // 连客户端异常断开
        ChatService::getInstance()->clientCloseException(conn);

        // 连接断开
        conn->shutdown();
    }
}

// 上报读写事件相关的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    // 获取缓冲区对象
    string buf = buffer->retrieveAllAsString();
    // 解析json数据 反序列化
    json js = json::parse(buf);
    // 达到目的：完全解耦网络模块的代码和业务模块的代码
    // 通过js["msgid"]获取->业务Handler->conn js time

    // 获取消息类型
    auto msgHandler = ChatService::getInstance()->getHandler(js["msgid"].get<int>());
    // 回调消息绑定好的事件处理器，来执行相应的业务模块
    msgHandler(conn, js, time);
}
