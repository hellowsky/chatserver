#ifndef OFFLIENMESSAGEMMODEL_HPP
#define OFFLIENMESSAGEMMODEL_HPP

#include <string>
#include <vector>

// 提供离线消息表的操作接口方法

class OfflineMsgModel
{
public:
    void insert(int userid, std::string msg);

    // 删除离线消息
    void remove(int userid);

    // 获取离线消息
    std::vector<std::string> query(int userid);
};

#endif