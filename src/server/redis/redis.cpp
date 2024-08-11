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
    // SUBSCRIBE命令本身会造成线程阻塞等待通道里面发生消息，这里只做订阅通道，不接收通道消息
    // 通道消息的接收专门在observer_channel_message函数中的独立线程中进行
    // 只负责发送命令，不阻塞接收redis server响应消息，否则和notifyMsg线程抢占响应资源
    if (REDIS_ERR == redisAppendCommand(_subscribe_context, "SUBSCRIBE %d", channel))
    {
        std::cerr << "subscribe channel error" << std::endl;
        return false;
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

// 向redis指定的通道unsubscribe取消订阅消息
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

// void Redis::observer_channel_message()
// {
//     redisReply *reply = nullptr;
//     while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply))
//     {
//         // 订阅收到的信息是一个带三个元素的数组
//         if (reply != nullptr && reply->element[1] != nullptr && reply->element[2]->str != nullptr)
//         {
//             //给业务层上报  通道上发生的消息
//             _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
//         }
//         freeReplyObject(reply);
//     }
//     std::cerr << ">>>>>>>>>>>>>>>>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
// }

void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply))
    {
        if (reply != nullptr)
        {
            // 确保回复包含至少三个元素
            if (reply->type == REDIS_REPLY_ARRAY && reply->elements >= 3)
            {
                // 确保第二个和第三个元素都不为空
                if (reply->element[1] != nullptr && reply->element[2] != nullptr &&
                    reply->element[1]->str != nullptr && reply->element[2]->str != nullptr)
                {
                    // 给业务层上报  通道上发生的消息
                    _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
                }
                else
                {
                    std::cerr << "Invalid reply format from Redis subscription context." << std::endl;
                }
            }
            else
            {
                std::cerr << "Unexpected reply type from Redis subscription context." << std::endl;
            }
        }
        else
        {
            std::cerr << "Received null reply from Redis subscription context." << std::endl;
        }
        freeReplyObject(reply);
    }

    if (redisGetReply(this->_subscribe_context, (void **)&reply) == REDIS_ERR)
    {
        std::cerr << "Error getting reply from Redis subscription context: "  << std::endl;
    }

    std::cerr << ">>>>>>>>>>>>>>>>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
}


void Redis::init_notify_handler(std::function<void(int, std::string)> fn)
{
    this->_notify_message_handler = fn;
}
