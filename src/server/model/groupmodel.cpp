#include "groupmodel.hpp"
#include "db.h"
#include <iostream>

bool GroupModel::createGroup(Group &group)
{
    char str[1024] = {0};

    sprintf(str, "insert  into AllGroup(groupname,groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    // if (mysql.connect())
    // {
    //     if (mysql.connect())
    //     {
    //         if (mysql.update(str))
    //         {
    //             group.setId(mysql_insert_id(mysql.getConnection()));
    //             return true;
    //         }
    //     }
    // }

    // return false;

    if (!mysql.connect())
    {
        std::cerr << "Database connection failed." << std::endl;
        return false;
    }

    // 执行 SQL 语句
    if (!mysql.update(str))
    {
        std::cerr << "Failed to execute the SQL statement: " << std::endl;
        return false;
    }

    // 获取新插入记录的 ID
    group.setId(mysql_insert_id(mysql.getConnection()));
    if (group.getId() <= 0)
    {
        std::cerr << "Failed to retrieve the new group's ID." << std::endl;
        return false;
    }

    return true;
}

bool GroupModel::joinGroup(int userid, int groupid, std::string role)
{
    MySQL mysql;
    // if (mysql.connect())
    // {
    //     // 准备sql语句
    //     std::string sql = ("insert into GroupUser(groupid,userid,grouprole) values (%d, %d, '%s')", groupid, userid, role.c_str());
    //     if (mysql.update(sql))
    //     {
    //         return true;
    //     }
    // }
    // return false;

    if (!mysql.connect())
    {
        std::cerr << "Database connection failed." << std::endl;
        return false;
    }

    // 准备 SQL 语句
    std::string sql = "INSERT INTO GroupUser(groupid, userid, grouprole) VALUES (" +
                      std::to_string(groupid) + ", " +
                      std::to_string(userid) + ", '" +
                      role + "')";

    if (!mysql.update(sql))
    {
        std::cerr << "Join group failed! - db.cpp:44" << std::endl;
        std::cerr << "Error message: " << std::endl;
        return false;
    }

    return true;
}

std::vector<Group> GroupModel::queryGroups(int userid)
{
    /*
  1.先根据userid在groupuser表中查询出该用户所属的群组信息
  2.再根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查询出用户的详细信息
  */
    vector<Group> groupVec;
    MySQL mysql;
    if (mysql.connect())
    {
        // 准备sql语句
        char sql[1024] = {0};
        sprintf(sql, "select a.id,a.groupname,a.groupdesc from AllGroup as a inner join GroupUser as g on a.id = g.groupid where g.userid = %d ", userid);

        MYSQL_RES *res = mysql.query(sql);

        if (res == nullptr)
        {
            std::cerr << "Query failed! - db.cpp:54" << std::endl;
            std::cerr << "Error message: "<< std::endl;
            return groupVec;
        }

        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 查询出userid的所有群组信息
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }

        for (Group &group : groupVec)
        {
            sprintf(sql, "select u.id,u.name,u.state,g.grouprole from User as u inner join GroupUser as g on u.id = g.userid where g.groupid = %d", group.getId());

            MYSQL_RES *res = mysql.query(sql);
            if (res != nullptr)
            {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res)) != nullptr)
                {
                    GroupUser user;
                    user.setId(atoi(row[0]));
                    user.setName(row[1]);
                    user.setState(row[2]);
                    user.setRole(row[3]);
                    group.getUsers().push_back(user);
                }
                mysql_free_result(res);
            }
        }
    }

    return groupVec;
}

// 根据指定的groupid查询用户组id列表,除userid自己
std::vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    MySQL mysql;
    vector<int> idVec;
    char sql[1024] = {0};
    sprintf(sql, "select userid from GroupUser where groupid = %d and userid != %d ", groupid, userid);
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }

    return idVec;
}
