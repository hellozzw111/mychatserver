// 用来管理注册的dataserver的类，包含主从状态和数据保存情况
#include <vector>
#include <string>
#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include <chrono>
#include <mutex>
#include <set>

#include "public.hpp"
#include "rpc/src/include/mprpcapplication.h"
#include "data_serv.pb.h"

const std::chrono::seconds time_interval(1);

using namespace std;

class ServerManager {
public:
    ServerManager();
    void checkHeartBeat();
    void dealDisconnect(status& obj);
    void upgradeServer(status& slave);
    void downgradeServer(status& master);
    void updataStatus(status& obj, int score);
    void doAdd(std::string file_name, Node* parent, string contents);
    std::string doGet(std::string file_name, Node* parent);
    void doDel(std::string file_name, Node* parent);
    status& getStatus(string server_name);
    unordered_map<string, set<string>>& getMasterSlaves() { return master_slaves; }
private:
    unordered_map<string, muduo::net::TcpConnectionPtr> masters;
    unordered_map<string, muduo::net::TcpConnectionPtr> slaves;
    unordered_map<string, set<string>> master_slaves;
    unordered_map<string, status> server_status;
    dataserver::DataService_Stub* stub;
};