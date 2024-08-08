#ifndef USERMODEL_HPP
#define USERMODEL_HPP

#include"user.hpp"
//user表的数据操作类
class UserModel
{
public:
    //user表的增加方法
    bool insert(User& user);

    //根据用户号码查询用户信息
    User query(int id);

    //更新用户信息
    bool updateState(User user);

    //删除用户信息
    bool deleteById(int id);

    //重置用户的状态信息
    void resetState();
};

#endif