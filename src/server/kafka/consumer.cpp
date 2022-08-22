#include <iostream>
#include <string>
#include "consumer.h"
#include "json.hpp"
#include <mutex>
using namespace std;
using json = nlohmann::json;
Consumer::Consumer(std::string brokers, std::string topic_str){
    // std::string brokers = "localhost";
    std::string errstr;
    // std::string topic_str="first";//first
    MyHashPartitionerCb_consumer hash_partitioner;
    int32_t partition = RdKafka::Topic::PARTITION_UA;//为何不能用？？在Consumer这里只能写0？？？无法自动吗？？？
    partition = 0;
    start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    bool do_conf_dump = false;
    int opt;

    int use_ccb = 0;

    //Create configuration objects
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if (tconf->set("partitioner_cb", &hash_partitioner, errstr) != RdKafka::Conf::CONF_OK) 
    {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    /*
    * Set configuration properties
    */
    conf->set("metadata.broker.list", brokers, errstr);
    ExampleEventCb_consumer ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    ExampleDeliveryReportCb_consumer ex_dr_cb;

    /* Set delivery report callback */
    conf->set("dr_cb", &ex_dr_cb, errstr);
    /*
    * Create consumer using accumulated global configuration.
    */
    consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) 
    {
    std::cerr << "Failed to create consumer: " << errstr << std::endl;
    exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
    * Create topic handle.
    */
    topic = RdKafka::Topic::create(consumer, topic_str, tconf, errstr);
    if (!topic)
    {
    std::cerr << "Failed to create topic: " << errstr << std::endl;
    exit(1);
    }

    //初始化消息回调函数
    msghandle.insert({"message", std::bind(&Consumer::chatMsg, this, std::placeholders::_1)});
    msghandle.insert({"log", std::bind(&Consumer::logger, this, std::placeholders::_1)});
}
Consumer::~Consumer(){
    delete topic;
    delete conf;
    delete tconf;
    delete consumer;
}
void Consumer::message(){
    /*
    * Start consumer for topic+partition at start offset
    */
    RdKafka::ErrorCode resp = consumer->start(topic, 0, start_offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
    std::cerr << "Failed to start consumer: " << RdKafka::err2str(resp) << std::endl;
    exit(1);
    }

    ExampleConsumeCb_consumer ex_consume_cb;

    /*
    * Consume messages
    */
    while (run_consumer)
    {
    if (use_ccb)
    {
        consumer->consume_callback(topic, 0, 1000, &ex_consume_cb, &use_ccb);
    }
    else 
    {
        RdKafka::Message *msg = consumer->consume(topic, 0, 1000);
        const char* messages = msg_consume(msg, NULL);
        MsgHandle handle = msghandle[topic->name()];
        handle(messages);
    }
    consumer->poll(0);
    }

    /*
    * Stop consumer
    */
    consumer->stop(topic, 0);

    consumer->poll(1000);
}
void Consumer::logger(string msg){ //写日志方法
    //获得日志等级
    string level = msg.substr(0, 3);
    msg = msg.substr(3, msg.size()-3);
    // 启动专门的写日志线程
    // 获取当前的日期，然后取日志信息，写入相应的日志文件当中 a+
    time_t now = time(nullptr);
    tm *nowtm = localtime(&now);

    char file_name[128];
    sprintf(file_name, "/root/mywchatserver/logs/%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

    FILE *pf = fopen(file_name, "a+");
    if (pf == nullptr)
    {
        std::cout << "logger file : " << file_name << " open error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    char time_buf[128] = {0};
    sprintf(time_buf, "%d:%d:%d =>[%s] ", 
            nowtm->tm_hour, 
            nowtm->tm_min, 
            nowtm->tm_sec,
            level);
    msg.insert(0, time_buf);
    msg.append("\n");

    fputs(msg.c_str(), pf);
    fclose(pf);
}
void Consumer::chatMsg(std::string msg){  //聊天消息
    json js = json::parse(msg);
    int toid = js["toid"].get<int>();
    _chatservice = ChatService::instance();
    lock_guard<mutex> lock_(_chatservice->_connMutex);
    auto it = _chatservice->_userConnMap.find(toid);
    if(it != _chatservice->_userConnMap.end()){
        it->second->send(msg);
        return;
    }
    //存储离线用户消息
    _chatservice->_offlineMsgModel.insert(toid, msg);
}