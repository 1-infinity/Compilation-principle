#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    ifstream in("./test.txt",ios::in);
    if(!in.is_open()){
        cerr<<"open fail"<<endl;
        return -1;
    }
    char ch = in.get();
    cout<<char(in.peek())<<endl;
    in.close();
}