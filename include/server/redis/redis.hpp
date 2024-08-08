#ifndef REDIS_HPP
#define REDIS_HPP

#include <hiredis/hiredis.h>
#include<thread>
#include <functional>

class Redis{
    public:
    Redis();
    ~Redis();

    //连接redis服务器
    bool connect();

    //向redis指定的童通道channel发布消息
    bool publish(int channel,std::string message);

    //向redis指定的通道subscribe订阅消息
    bool subscribe(int channel);

    //向redis指定的通道unsubscibe取消订阅消息
    bool unsubscibe(int channel);

    //在独立线程中接收订阅通道中的信息
    void observer_channel_message();

    //初始化向业务层上报通道消息的回调函数
    void init_notify_handler(std::function<void(int,std::string)>fn);

    private:
    //hiredis同步上下文对象 负责pulish消息
    redisContext* _publish_context;

    //hiredis同步上下文对象 负责subscibe消息
    redisContext *_subscribe_context;

    //回调函数 收到订阅的消息 给service层上报
    std::function<void (int,std::string)>_notify_message_handler;   

};

#endif