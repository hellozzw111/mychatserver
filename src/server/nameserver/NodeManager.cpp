#include "nameserver/NodeManager.h"

NodeManager::NodeManager() {
    InitNodeManager();
}
void NodeManager::InitNodeManager() {
    Node* root = new Node;
    root->file_name = "/";
    root->is_file = false;
    root->parent = nullptr;
    root->is_new_dataserver = false;
    root->host = "124.220.24.66:1316";
    NodeTree = root;
}
void NodeManager::UpdateNodeTree(string name, string host, vector<string> change_files, vector<bool> add) {
    string cur_path_name = "/";
    Node* cur_path_node = NodeTree;
    while(cur_path_name != name) { // first
        int left = cur_path_name.size(), right = 0;
        string next_file = "";
        for(int i=left;i<name.size();i++) {
            if(name[i] == '/') {
                right = i;
                next_file = name.substr(left, right-left);
            }
        }
        for(auto it = cur_path_node->childs.begin();it!=cur_path_node->childs.end();it++) {
            if((*it)->file_name == next_file) {
                cur_path_name += next_file;
                cur_path_name += "/";
                cur_path_node = *it;
            }
        }
    }
    for(int i=0;i<change_files.size();i++) {
        UpdateNodeFile(change_files[i], add[i], cur_path_node);
    }
}
void NodeManager::UpdateNodeFile(string file_name, bool add, Node* cur_node) {
    int len = cur_node->file_name.size();
    int left = len, right = len;
    string tmp_node_name;
    while(right < file_name.size()) {
        if(file_name[right] == '/') {
            tmp_node_name = file_name.substr(left, right-left);
            for(auto it = cur_node->childs.begin();it<cur_node->childs.end();it++) {
                if((*it)->file_name == tmp_node_name) {
                    cur_node = *it;
                    left = right+1;
                    right++;
                }
            }
        }
    }
    if(left == right) {
        // 改变的是一个目录 TODO: 判断是一个新的服务器Node，断开连接后再删除节点
        if(add) {
            Node* new_node = new Node;
            new_node->file_name = tmp_node_name;
            new_node->is_file = false;
            new_node->is_new_dataserver = false;
            new_node->parent = cur_node;
            cur_node->childs.push_back(new_node);
        }else{
            Node* cur_node_parent = cur_node->parent;
            // 递归删除目录下所有的节点
            deleteNodeFile(cur_node);
            cur_node_parent = nullptr;
        }
    }
}
void NodeManager::deleteNodeFile(Node* cur_node) {
    if(cur_node->is_file) {
        delete cur_node;
    }else{
        for(auto it = cur_node->childs.begin();it<cur_node->childs.end();it++) {
            deleteNodeFile(*it);
        }
    }
}
Node* NodeManager::getNodeTree() {
    return NodeTree;
}

bool NodeManager::IsExist(string file_name) {
    int idx = 0; // 判断 '/' 的位置，一个层级一个层级的查找
    Node* cur = NodeTree;
    file_name = file_name.substr(1, file_name.size() - 1);
    while(1) {
        idx = file_name.find("/");
        if(idx == -1) {
            return true; // 找到上传路径的最后一级
        }
        std::string next = file_name.substr(0, idx);
        for(int i = 0; i < cur->childs.size(); i++) {
            if(cur->childs[i]->file_name == next) {
                cur = cur->childs[i];
                break;
            }
            if(i == cur->childs.size() - 1) {
                return false; // 在某一层没找到对应的Node
            }
        }
    }
}

Node* NodeManager::getLastParentNode(string file_name) {
    int idx = 0; // 判断 '/' 的位置，一个层级一个层级的查找
    Node* cur = NodeTree;
    Node* dataservernode = nullptr;
    file_name = file_name.substr(1, file_name.size() - 1);
    while(1) {
        idx = file_name.find("/");
        if(idx == -1) {
            return dataservernode; // 找到上传路径的最后一级
        }
        std::string next = file_name.substr(0, idx);
        for(int i = 0; i < cur->childs.size(); i++) {
            if(cur->childs[i]->file_name == next) {
                cur = cur->childs[i];
                if(cur->is_new_dataserver) {
                    dataservernode = cur;
                }
                break;
            }
            if(i == cur->childs.size() - 1) {
                return nullptr; // 在某一层没找到对应的Node
            }
        }
    }
}