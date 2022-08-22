#include <iostream>
using namespace std;
#include <librdkafka/rdkafkacpp.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <mutex>
#include <functional>
#include <map>
#if _AIX
#include <unistd.h>
#endif
#include "chatservice.hpp"
class ChatService;
static volatile sig_atomic_t run_consumer = 1;
using MsgHandle = std::function<void(string)>;
class ExampleDeliveryReportCb_consumer : public RdKafka::DeliveryReportCb 
{
 public:
  void dr_cb (RdKafka::Message &message) {
    std::cout << "Message delivery for (" << message.len() << " bytes): " <<
        message.errstr() << std::endl;
    if (message.key())
      std::cout << "Key: " << *(message.key()) << ";" << std::endl;
  }
};

class ExampleEventCb_consumer : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {
    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
          run_consumer = false;
        break;

      case RdKafka::Event::EVENT_STATS:
        std::cerr << "\"STATS\": " << event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_LOG:
        fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
        break;

      default:
        std::cerr << "EVENT " << event.type() <<
            " (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;
    }
  }
};

class MyHashPartitionerCb_consumer : public RdKafka::PartitionerCb {
    public:
        int32_t partitioner_cb (const RdKafka::Topic *topic, const std::string *key,int32_t partition_cnt, void *msg_opaque) 
        {
            std::cout<<"partition_cnt="<<partition_cnt<<std::endl;
            return djb_hash(key->c_str(), key->size()) % partition_cnt;
        }
    private:
        static inline unsigned int djb_hash (const char *str, size_t len) 
        {
        unsigned int hash = 5381;
        for (size_t i = 0 ; i < len ; i++)
            hash = ((hash << 5) + hash) + str[i];
        std::cout<<"hash1="<<hash<<std::endl;

        return hash;
        }
};

const char* msg_consume(RdKafka::Message* message, void* opaque)
{
    switch (message->err())
    {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
          /* Get Real message */
            return static_cast<const char *>(message->payload()); // void* payload()
        case RdKafka::ERR__PARTITION_EOF:
              /* Last message */
              run_consumer = false;
              break;
        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run_consumer = false;
            break;
    default:
        /* Errors */
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        run_consumer = false;
    }
    return nullptr;
}
class ExampleConsumeCb_consumer : public RdKafka::ConsumeCb {
    public:
        void consume_cb (RdKafka::Message &msg, void *opaque)
        {
            msg_consume(&msg, opaque);
        }
};
class Consumer{
  public:
    friend class ChatService;
    Consumer(std::string brokers, std::string topic_str);
    ~Consumer();
    void message();
    void logger(string msg);
    void chatMsg(string msg);
  private:
    RdKafka::Topic *topic;
    RdKafka::Conf *conf, *tconf;
    RdKafka::Consumer *consumer;
    bool use_ccb;
    int64_t start_offset;
    map<std::string, MsgHandle> msghandle;
    ChatService* _chatservice;
};