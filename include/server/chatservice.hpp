#ifndef CHATSERVICE_HPP
#define CHATSERVICE_HPP

#include <muduo/net/TcpConnection.h>
#include <functional>
#include <mutex>
#include <unordered_map>

#include"redis.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include"groupmodel.hpp"

using namespace muduo;
using namespace muduo::net;


#include "json.hpp"
using json = nlohmann::json;
using namespace std;

// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象的接口函数
    static ChatService *getInstance();

    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    
    //创建群业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //加入群业务
    void joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //群聊天业务
    void chatGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid,std::string msg);

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 处理客户端异常断开
    void clientCloseException(const TcpConnectionPtr &conn);
    // 服务器异常,业务重置方法
    void reset();

private:
    ChatService();
    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存在在线任务的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 定义互斥锁 保证_userConnMap的线程安全
    mutex _userConnMutex;

    // 数据操作类对象
    UserModel _userModel;

    // 离线消息操作类对象
    OfflineMsgModel _offlineMsgModel;

    //添加好友操作对象
    FriendModel _friendModel;

    //操作群对象
    GroupModel _groupModel;
    
    //redis操作对象
    Redis _redis;
};

#endif