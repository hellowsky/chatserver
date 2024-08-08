#include "redis.hpp"
#include <iostream>

Redis::Redis() : _publish_context(nullptr), _subscribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }
    if (_subscribe_context != nullptr)
    {
        redisFree(_subscribe_context);
    }
}

bool Redis::connect()
{
    // 负责publish发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (_publish_context == nullptr)
    {
        std::cout << "connect publish error" << std::endl;
        return false;
    }

    // 负责subscribe订阅消息的上下文连接
    _subscribe_context = redisConnect("127.0.0.1", 6379);
    if (_subscribe_context == nullptr)
    {
        std::cout << "connect subscribe error" << std::endl;
        return false;
    }

    // 在单独的线程中，监听通道上的事件。有消息给业务层上报
    std::thread t([&]()
                  { observer_channel_message(); });
    t.detach();

    std::cout << "connect redis-server success" << std::endl;
    return true;
}

bool Redis::publish(int channel, std::string message)
{
    //
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (reply == nullptr)
    {
        std::cout << "publish message error" << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向
bool Redis::subscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(_subscribe_context, "SUBSCRIBE %d", channel))
    {
        std::cerr << "subscribe channel error" << std::endl;
    }

    // redisBufferWrite可以循环发送缓冲区 知道缓冲区的数据发送完毕(done被置为1)
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_publish_context, &done))
        {
            std::cerr << "subscribe channel error" << std::endl;
            return false;
        }
    }

    return true;
}

bool Redis::unsubscibe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "UNSUBSCRIBE %d", channel))
    {
        std::cerr << "unsubscibe channel error" << std::endl;
        return false;
    }

    // redisBufferWrite可以循环发送缓冲区 知道缓冲区的数据发送完毕(done被置为1)
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            std::cerr << "unsubscribe channel error" << std::endl;
            return false;
        }
    }
    return true;
}

void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply))
    {
        // 订阅收到的信息是一个带三个元素的数组
        if (reply != nullptr && reply->element[1] != nullptr && reply->element[2]->str != nullptr)
        {
            //给业务层上报  通道上发生的消息
            _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    std::cerr << ">>>>>>>>>>>>>>>>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
}

void Redis::init_notify_handler(std::function<void(int, std::string)> fn)
{
    this->_notify_message_handler = fn;
}
