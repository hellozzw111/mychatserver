#ifndef PUBLIC_H
#define PUBLIC_H

#include <string>
#include <vector>
#include <chrono>
/*
server和client的公共文件
*/
enum EnMsgType
{
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_ACK, // 登录响应消息
    LOGINOUT_MSG, // 注销消息
    REG_MSG, // 注册消息
    REG_MSG_ACK, // 注册响应消息
    ONE_CHAT_MSG, // 聊天消息
    ADD_FRIEND_MSG, // 添加好友消息

    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG, // 加入群组
    GROUP_CHAT_MSG, // 群聊天
    GROUP_FILE_UPLOAD_MSG, // 上传群文件
    GROUP_FILE_DOWNLOAD_MSG, // 下载群文件
    GROUP_FILE_DOWNLOAD_MSG_ACK, // 下载文件响应消息
    GROUP_FILE_DELETE_MSG, // 删除群文件
    GROUP_FILE_GET_ALL_MSG, // 获得所有群文件
};

struct Node {
    std::string file_name;
    bool is_file;
    Node* parent;
    std::vector<Node*> childs;
    bool is_new_dataserver;
    std::string host;
    int port;
};

struct status {
    std::string name;
    std::string role;
    std::string ip;
    int port;
    int score;
    std::chrono::_V2::system_clock::time_point last_heartbeat_time;
};

#endif