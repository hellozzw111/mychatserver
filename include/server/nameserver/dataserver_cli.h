#ifndef  __DATA_SERVER_CLIENT_H_
#define  __DATA_SERVER_CLIENT_H_

#include <iostream>

#include "public.hpp"
#include "nameserver/NodeManager.h"
#include "nameserver/ServerManager.h"

class DataServerClient {
public:
    DataServerClient() 
    {
        nodemanager = new NodeManager();
        servermanager = new ServerManager();
    }
    ~DataServerClient()
    {
        delete nodemanager;
        delete servermanager;
    }
    void Add(std::string filepath, std::string contents)
    {
        if(nodemanager->IsExist(filepath))
        {
            Node* parent = nodemanager->getLastParentNode(filepath);
            servermanager->doAdd(filepath, parent, contents);
        }
        else
        {
            std::cerr << "file path error!" << std::endl;
            return;
        }
    }
    void Del(std::string filepath) 
    {
        if(nodemanager->IsExist(filepath))
        {
            Node* parent = nodemanager->getLastParentNode(filepath);
            servermanager->doDel(filepath, parent);
        }
        else
        {
            std::cerr << "file path error!" << std::endl;
            return;
        }
    }
    string Get(std::string filepath)
    {
        if(nodemanager->IsExist(filepath))
        {
            Node* parent = nodemanager->getLastParentNode(filepath);
            return servermanager->doGet(filepath, parent);
        }
        else
        {
            std::cerr << "file path error!" << std::endl;
            return "";
        }
    }
    Node* GetAll(int groupid)
    {
        std::string filespath = "/" + to_string(groupid);
        if(nodemanager->IsExist(filespath))
        {
            return nodemanager->getNodeTree();
        }
        else
        {
            std::cerr << "file path error!" << std::endl;
            return;
        }
    }
private:
    NodeManager* nodemanager;
    ServerManager* servermanager;
};


#endif