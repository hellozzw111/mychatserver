#include <iostream>
#include <string>
using namespace std;
#include "producer.h"
Producer::Producer(std::string brokers, std::string topic_str){
    // std::string brokers = "localhost";
    std::string errstr;
    // std::string topic_str="logs";//自行制定主题topic
    MyHashPartitionerCb hash_partitioner;
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    bool do_conf_dump = false;
    int opt;

    int use_ccb = 0;

    //Create configuration objects
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if (tconf->set("partitioner_cb", &hash_partitioner, errstr) != RdKafka::Conf::CONF_OK) 
     {
          std::cerr << errstr << std::endl;
          exit(1);
     }

    /*
    * Set configuration properties
    */
    conf->set("metadata.broker.list", brokers, errstr);
    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    ExampleDeliveryReportCb ex_dr_cb;

    /* Set delivery report callback */
    conf->set("dr_cb", &ex_dr_cb, errstr);

    /*
     * Create producer using accumulated global configuration.
     */
    producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) 
    {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;

    /*
     * Create topic handle.
     */
    topic = RdKafka::Topic::create(producer, topic_str, tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }
  }
bool Producer::message(string line){
    if (line.empty())
    {
        producer->poll(0);
        return false;
    }

/*
    * Produce message
    // 1. topic
    // 2. partition
    // 3. flags
    // 4. payload
    // 5. payload len
    // 6. std::string key
    // 7. msg_opaque? NULL
    */
    std::string key=line.substr(0,5);//根据line前5个字符串作为key值
    // int a = MyHashPartitionerCb::djb_hash(key.c_str(),key.size());
    // std::cout<<"hash="<<a<<std::endl;
    RdKafka::ErrorCode resp = producer->produce(topic, 0,
        RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
        const_cast<char *>(line.c_str()), line.size(),
        key.c_str(), key.size(), NULL);//这里可以设计key值,因为会根据key值放在对应的partition
        if (resp != RdKafka::ERR_NO_ERROR)
            std::cerr << "% Produce failed: " <<RdKafka::err2str(resp) << std::endl;
        else
            std::cerr << "% Produced message (" << line.size() << " bytes)" <<std::endl;
        producer->poll(0);//对于socket进行读写操作。poll方法才是做实际的IO操作的。return the number of events served
    //
    //退出前处理完输出队列中的消息
    if(!run_producer){
        while (run_producer && producer->outq_len() > 0) {
        std::cerr << "Waiting for " << producer->outq_len() << std::endl;
        producer->poll(1000);
        }
    }
}
Producer::~Producer(){
    delete topic;
    delete producer;
}