#ifndef FRIEND_H
#define FRIEND_H

#include <string>
using namespace std;

// Friend
class Friend
{
public:
    Friend(int userid = -1, int friendid = -1)
    {
        this->userid = userid;
        this->friendid = friendid;
    }

    void setUserid(int userid) { this->userid = userid; }
    void setFriendid(int friendid){ this->friendid = friendid; }

    int getUserid() { return this->userid; }
    int getFriendid() { return this->friendid; }

protected:
    int userid;
    int friendid;
};

#endif