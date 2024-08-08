#include "friendmodel.hpp"
#include <db.h>

void FriendModel::insert(int userid, int friendid)
{
    char str[1024] = {0};
    sprintf(str, "insert into Friend values(%d,%d)", userid, friendid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.query(str);
    }
}

std::vector<User> FriendModel::query(int userid)
{
    char str[1024] = {0};
    sprintf(str, "select u.id,u.name,u.state  from  User as u inner join Friend as f on u.id = f.friendid where f.userid = %d",userid);

    vector<User> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(str);

        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);  
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }

    return vec;
}
