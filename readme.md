# ChatServer
可以工作在nginx tcp负载均衡环境中的集群聊天服务器和客户端源码  基于muduo实现

## 功能
* 使用muduo网络库作为项目的网络的核心模块，提高高并发网络IO服务，结构网络和业务代码模块
* 使用json序列化和反序列化消息作为私有通信协议
* 配置nginx基于tcp的负载均衡，实现聊天服务器的集群功能，提高后端服务的并发能力
* 基于redis的发布订阅功能，实现跨服务器的消息通信
* 基于kafka消息队列，实现分布式日志系统，记录服务器运行状态
* 基于protobuf实现的轻量级rpc框架，并应用于聊天服务器，用于分离后端服务(https://github.com/hellozzw111/mprpc.git)

## 环境要求
* Linux
* C++14
* MySql
* Nginx
* Json
* Dafka
* Protobuf
* Muduo
* Redis
* Zookeeper
* Cmke

## 目录树
```
.
├── bin            可执行文件
│   └── server/client
├── build          
├── include        头文件
│   ├── server
├── lib            包含轻量级rpc框架libmpprc.a
│   ├── include
│   ├── libmprpc.a
├── logs           保存日志
├── rpcserver      用于发布rpc方法
│   ├── rpcee
├── src           源代码
│   ├── client
│   ├── server
│   |   ├── db
│   |   ├── kafka
│   |   ├── log
│   |   ├── model
│   |   ├── redis
├── thirdparty           包含json第三方库
│   └── json.hpp
├── autobuild.sh     编译脚本
├── CMakeLists.txt
├── LICENSE
└── readme.md
```

## 数据库配置

表设计

User表

| 字段名称 | 字段类型                  | 字段说明     | 约束                        |
| -------- | ------------------------- | ------------ | --------------------------- |
| id       | INT                       | 用户id       | PRIMARY KEY、AUTO_INCREMENT |
| name     | VARCHAR(50)               | 用户名       | NOT NULL、UNIQUE            |
| password | VARCHAR(50)               | 用户密码     | NOT NULL                    |
| state    | ENUM('online', 'offline') | 当前登录状态 | DEFAULT 'offline'           |

Friend表

| 字段名称 | 字段类型 | 字段说明 | 约束               |
| -------- | -------- | -------- | ------------------ |
| userid   | INT      | 用户id   | NOT NULL、联合主键 |
| friendid | INT      | 好友id   | NOT NULL、联合主键 |

ALLGroup表

| 字段名称  | 字段类型     | 字段说明   | 约束                        |
| --------- | ------------ | ---------- | --------------------------- |
| id        | INT          | 组id       | PRIMARY KEY、AUTO_INCREMENT |
| groupname | VARCHAR(50)  | 组名称     | NOT NULL、UNIQUE            |
| groupdesc | VARCHAR(200) | 组功能表述 | DEFAULT ' '                 |

GroupUser表

| 字段名称  | 字段类型                  | 字段说明 | 约束               |
| --------- | ------------------------- | -------- | ------------------ |
| groupid   | INT                       | 组id     | NOT NULL、联合主键 |
| userid    | INT                       | 组员id   | NOT NULL、联合主键 |
| grouprole | ENUM('creator', 'normal') | 组内角色 | DEFAULT 'normal'   |

OfflineMessage表

| 字段名称 | 字段类型     | 字段说明                   | 约束     |
| -------- | ------------ | -------------------------- | -------- |
| userid   | INT          | 用户id                     | NOT NULL |
| message  | VARCHAR(500) | 离线消息（JSON类型字符串） | NOT NULL |

