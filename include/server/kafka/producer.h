#include <iostream>
using namespace std;
#include <librdkafka/rdkafkacpp.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#if _AIX
#include <unistd.h>
#endif

static volatile sig_atomic_t run_producer = 1;


class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb 
{
 public:
  void dr_cb (RdKafka::Message &message) {
    std::cout << "Message delivery for (" << message.len() << " bytes): " <<
        message.errstr() << std::endl;
    if (message.key())
      std::cout << "Key: " << *(message.key()) << ";" << std::endl;
  }
};


class ExampleEventCb : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {
    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
          run_producer = false;
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

/* Use of this partitioner is pretty pointless since no key is provided
 * in the produce() call.so when you need input your key */
class MyHashPartitionerCb : public RdKafka::PartitionerCb {
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
class Producer{
public:
  Producer(std::string brokers, std::string topic_str);
  bool message(string line);
  ~Producer();
private:
  RdKafka::Producer* producer;
  RdKafka::Topic *topic;
};