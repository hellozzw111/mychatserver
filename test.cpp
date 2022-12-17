#include <iostream>
#include <string>
using namespace std;

int main(){
    string str = "test:arg1:arg2";
    int idx = str.find(":");
    cout<<idx<<endl;
    return 0;
}