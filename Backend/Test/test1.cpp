#include <bits/stdc++.h>
#include <chrono>

class temp{
public:
    void job1(){
        int i=0;
        while(1){
            if(i==5) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            i++;
        }
    }
    void start(){
        std::thread(&temp::job1).join();
    }
};
int main(){
    temp t;
    t.start();
}