#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>
#include <chrono>

#include <cppconn/resultset.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

#include "./brute_force.cpp"

#define RESULT_TIME 15
#define JOB_TIME 1800
#define ATTEMPTS 100000000
#define THREADS 4

namespace timer_thread{

void populate_symbols(std::vector<char>& symbols){
    for(char c='a';c<='z';c++){
        symbols.push_back(c);
    }
    // for(char c='A';c<='Z';c++){
    //     symbols.push_back(c);
    // }
    // for(char c='0';c<='9';c++){
    //     symbols.push_back(c);
    // }
    // symbols.push_back('!');
    // symbols.push_back('@');
    // symbols.push_back('#');
    // symbols.push_back('$');
    // symbols.push_back('%');
    // symbols.push_back('^');
    // symbols.push_back('&');
    // symbols.push_back('*');
    // symbols.push_back('(');
    // symbols.push_back(')');
    // symbols.push_back('+');
    // symbols.push_back('=');
    // symbols.push_back('[');
    // symbols.push_back(']');
    // symbols.push_back('{');
    // symbols.push_back('}');
    // symbols.push_back('\\');
    // symbols.push_back('|');
    // symbols.push_back('/');
    // symbols.push_back('.');
    // symbols.push_back('<');
    // symbols.push_back('>');
    // symbols.push_back('?');
    // symbols.push_back(':');
    // symbols.push_back(';');
}

int get_seconds_since_epoch(){
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    std::chrono::system_clock::duration dtn = tp.time_since_epoch();
    int seconds=dtn.count() * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
    return seconds;
}

void timer_function(std::mutex& do_job_lock,
                    std::mutex& do_timing_lock,
                    std::mutex& op_fifo_lock,
                    bool& do_job,
                    bool& do_timing,
                    int op_fifo_fd){

    int start_time=get_seconds_since_epoch();
    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        op_fifo_lock.lock();
        if(get_seconds_since_epoch()>=start_time+RESULT_TIME && fcntl(op_fifo_fd,F_GETFD)!=-1){
            std::cout<<"15 seconds have passed since starting"<<std::endl;
            write(op_fifo_fd,"time elapsed",(std::string("time elapsed")).size());
            close(op_fifo_fd);
        }    
        op_fifo_lock.unlock();
        if(get_seconds_since_epoch()>=start_time+JOB_TIME){
            std::cout<<"thread running for too long...terminating now"<<std::endl;
            do_job_lock.lock();
            do_job=0;
            do_job_lock.unlock();
            break;
        }
        do_timing_lock.lock();
        bool state=do_timing;
        do_timing_lock.unlock();
        if(state==0){
            break;
        }
    }
}

std::string get_db_result(sql::Connection* con, 
                       std::string input_hash,
                       std::string table_name){

    sql::Statement *stmt;
  	sql::ResultSet *res;
		
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT word AS _message from "+ table_name +" where hash= '" + input_hash +"'");
    if(!res->next()){
        delete res;
        delete stmt;
  		return ""; //Handle Failiure
    }
    
    std::string plaintext=res->getString("_message");
    delete res;
    delete stmt;
    return plaintext;
}

void job_function(std::string hash,
                  std::string& hash_type,
                  std::mutex& do_job_lock,
                  std::mutex& do_timing_lock,
                  std::mutex& op_fifo_lock,
                  bool& do_job,
                  bool& do_timing,
                  int op_fifo_fd){

    //--------variables to configure the dummy workload, not part of actual code----------
    // int start_time=get_seconds_since_epoch();
    // int time_taken=5;
    //------------------------------------------------------------------------------------
    sql::Driver* driver=get_driver_instance();
    sql::Connection* con=driver->connect("tcp://127.0.0.1:3306","ProjectUser","ProjectUser");
    con->setSchema("ohct");
    std::string dictionary_name=hash_type+"_dictionary";
    std::cout<<"dict name="<<dictionary_name<<std::endl;
    std::cout<<"hash = "<<hash<<std::endl;
    std::string plaintext=get_db_result(con,hash,dictionary_name);
    if(plaintext!=""){
        //Successfully found in DB
        std::cout<<"found in db --> "<<plaintext<<std::endl;
        op_fifo_lock.lock();
        write(op_fifo_fd,plaintext.c_str(),plaintext.size());
        close(op_fifo_fd);
        op_fifo_lock.unlock();
        do_timing_lock.lock();
        do_timing=0;
        do_timing_lock.unlock();
        con->close();
        delete con;
        return;
    }
    std::vector<char> symbols={};
    populate_symbols(symbols);
    for(char c : symbols) std::cout<<c;
    std::cout<<std::endl;
    bool do_brute_force=1;
    std::thread bf_thread(brute_force::start_brute_force,
                          std::ref(hash),
                          std::ref(hash_type),
                          std::ref(symbols),
                          std::ref(plaintext),
                          ATTEMPTS,
                          std::ref(do_brute_force),
                          THREADS);
    while(1){
        // std::cout<<"plaintext="<<plaintext<<std::endl;
        do_job_lock.lock();
        bool state=do_job;
        do_job_lock.unlock();
        if(state==0){
            std::cout<<"job thread received signal from timer"<<std::endl;
            do_job_lock.lock();
            do_job=0;
            do_job_lock.unlock();
            bf_thread.join();
            con->close();
            delete con;
            return;
        }
        if(plaintext!=""){
            //brute force successful
            std::cout<<"brute force successful"<<std::endl;
            op_fifo_lock.lock();
            if(fcntl(op_fifo_fd,F_GETFD)!=-1){
                write(op_fifo_fd,plaintext.c_str(),plaintext.size());
                close(op_fifo_fd);
            }
            op_fifo_lock.unlock();
            do_timing_lock.lock();
            do_timing=0;
            do_timing_lock.unlock();
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    bf_thread.join();
    std::cout<<"bf_thread joined"<<std::endl;
    con->close();
    delete con;
    std::cout<<"job_function returned"<<std::endl;
}

std::thread start_timer(std::mutex& do_job_lock,
                        std::mutex& do_timing_lock,
                        std::mutex& op_fifo_lock,
                        bool& do_job,
                        bool& do_timing,
                        int op_fifo_fd){

    return std::thread(timer_function,std::ref(do_job_lock),std::ref(do_timing_lock),std::ref(op_fifo_lock),std::ref(do_job),std::ref(do_timing), op_fifo_fd);
}

std::thread start_job(std::string hash,
                      std::string& hash_type,
                      std::mutex& do_job_lock,
                      std::mutex& do_timing_lock,
                      std::mutex& op_fifo_lock,
                      bool& do_job,
                      bool& do_timing,
                      int op_fifo_fd){

    return std::thread(job_function,hash,std::ref(hash_type),std::ref(do_job_lock),std::ref(do_timing_lock),std::ref(op_fifo_lock),std::ref(do_job),std::ref(do_timing), op_fifo_fd);
}

void start(std::string hash,std::string& hash_type,std::string op_fifo_name){
    std::cout<<"hash="<<hash<<std::endl;
    std::cout<<"hash_type="<<hash_type<<std::endl;
    bool do_job=1;
    bool do_timing=1;
    
    std::mutex do_job_lock;
    std::mutex do_timing_lock;
    std::mutex op_fifo_lock;

    int op_fifo_fd;

    op_fifo_fd=open(("./FIFOs/"+op_fifo_name).c_str(),O_WRONLY);

    std::cout<<"about to spawn timer and job thread"<<std::endl;
    std::thread timer_thread=start_timer(std::ref(do_job_lock),std::ref(do_timing_lock),std::ref(op_fifo_lock),std::ref(do_job),std::ref(do_timing), op_fifo_fd);
    std::thread job_thread=start_job(hash,std::ref(hash_type),std::ref(do_job_lock),std::ref(do_timing_lock),std::ref(op_fifo_lock),std::ref(do_job),std::ref(do_timing), op_fifo_fd);
    timer_thread.join(); job_thread.join();
}
};
