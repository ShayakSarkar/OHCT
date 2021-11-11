#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <bits/stdc++.h>

#include "./timer_thread.cpp"

std::vector<std::string> tokenize(std::string& data){
    std::vector<std::string> result;
    int i=0;
    std::string current_str="";
    while(i<data.size()){
        if(data[i]==' '){
            //std::cout<<"current str="<<current_str<<std::endl;
            result.push_back(current_str);
            current_str="";
            i++;
            continue;
        }
        current_str+=data[i];
        i++;
    }
    result.push_back(current_str);
    return result;
}

void print(std::vector<std::string>& arr){
    std::cout<<"[";
    for(auto elm : arr){
        std::cout<<elm<<" ";
    }
    std::cout<<"]"<<std::endl;
}
int main(){
    std::vector<std::thread> threads;
    while(1){
        int fifo_read_fd=open("./FIFOs/hash_cracking_engine_fifo",O_RDONLY);
        char buffer[400];
        int bytes_read=read(fifo_read_fd,buffer,400);
        if(bytes_read==0){
            close(fifo_read_fd);
            fifo_read_fd=open("./FIFOs/hash_cracking_engine_fifo",O_RDONLY);
            bytes_read=read(fifo_read_fd,buffer,400);
        }
        buffer[bytes_read-1]='\0';
        std::string data(buffer);
        std::cout<<data<<" received"<<std::endl;
        std::vector<std::string> data_tokens=tokenize(data);
        std::cout<<"tokens: ";
        std::cout<<"new thread spawned to process the request"<<std::endl;
        // print(data_tokens);
        std::string hash=data_tokens[0];
        std::string hash_type=data_tokens[1]; 
        std::string request_id=data_tokens[2];
        std::cout<<"data=["<<hash<<" "<<hash_type<<" "<<request_id<<"]"<<std::endl;
        // int fifo_write_fd=open(("./FIFOs/"+request_id).c_str(),O_WRONLY);
        // write(fifo_write_fd,"data from hash cracking engine",30);
        // close(fifo_write_fd);
        std::thread new_thread(timer_thread::start,hash,std::ref(hash_type),request_id);
        threads.push_back(std::move(new_thread));
    } 
    for(auto& th : threads){
        th.join();
    }
}