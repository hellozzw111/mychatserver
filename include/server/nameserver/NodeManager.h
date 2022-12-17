#ifndef  __NODEMANAGER_H_
#define  __NODEMANAGER_H_

#include <string>
#include <vector>
#include <mutex>

#include "public.hpp"

using namespace std;

class NodeManager {
public:
    NodeManager();  
    void BuildNodeTree();
    void UpdateNodeTree(string name, string host, vector<string> change_files, vector<bool> add);
    void UpdateNodeFile(string file_name, bool add, Node* data_server_node);
    void deleteNodeFile(Node* cur_node);
    bool IsExist(string file_name);
    Node* getNodeTree(){ return NodeTree; }
    Node* getLastParentNode(string file_name);
private:
    void InitNodeManager();
    Node* NodeTree; // "/"
    static mutex m_Mutex;
};


#endif