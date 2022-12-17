#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <mprpcchannel.h>
#include <vector>
#include <thread>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "redis.hpp"
#include "groupmodel.hpp"
#include "friendmodel.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "json.hpp"
using json = nlohmann::json;
#include "friendmodel.hpp"
#include "user.hpp"
#include <zookeeperutil.h>
#include "search.pb.h"
#include "producer.h"
#include "consumer.h"
#include "nameserver/dataserver_cli.h"
#include "public.hpp"

class Consumer;
// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// 聊天服务器业务类
class ChatService
{
public:
    friend class Consumer;
    // 获取单例对象的接口函数
    static ChatService *instance();
    //析构函数
    ~ChatService();
    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组文件上传
    void groupfileupload(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组文件下载
    void groupfiledownload(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组文件删除
    void groupfiledelete(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 获取群组文件结构
    void groupallfile(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    // 服务器异常，业务重置方法
    void reset();
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);
    // 从rpc服务器上获取用户的好友对象
    vector<User> getFriends(int id);

private:
    ChatService();

    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;
    mutex _friendsMutex;
    // 数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // redis操作对象
    Redis _redis;
    // ZkClient对象

    //kafka消费者何生产者对象
    Consumer* consumer_;
    Producer* producer_;
    //消费者线程
    thread t;
    mutex dataserver_cli_lock;
    DataServerClient* dataserver_;
};

#endif