#include "./HashLibrary/md5.h"
#include "./HashLibrary/sha256.h"

#include <bits/stdc++.h>

namespace brute_force{

int get_length(int n,int syms){
    int length=0;
    int counted=0;
    while(n>counted){
        length++;
        counted+=(int)std::pow(syms,length);
    }
    return length;
}

std::string wordify(std::vector<char>& word_list){
    std::string s="";
    for(char c : word_list){
        s+=c;
    }
    return s;
}

int get_new_n(int n,int syms){
    int length=0;
    int counted=0;
    int last_counted=0;
    while(n>counted){
        last_counted=counted;
        length++;
        counted+=(int)std::pow(syms,length);
    }
    return n-last_counted;
}

void make_nth_new(std::vector<char>& symbols,
                  int n,
                  std::vector<char>& word_list){

    n--;
    int inc_ctr=1;
    for(int pos=0;pos<word_list.size();pos++){
        word_list[pos]=symbols[(n/inc_ctr)%symbols.size()];
        inc_ctr*=symbols.size();
    }
    std::reverse(word_list.begin(),word_list.end());
}

std::string get_nth(std::vector<char> symbols,int n){
    int length=get_length(n,symbols.size());
    std::vector<char> word_list(length);
    n=get_new_n(n,symbols.size());
    make_nth_new(symbols,n,word_list);
    return wordify(word_list);
}

void brute_force_thread(std::vector<char>& symbols,
                        std::string& hash,
                        std::string& hash_type,
                        int start,
                        int end,
                        bool& do_brute_force,
                        std::string& plaintext){

    int checked=0;
    MD5 md5;
    SHA256 sha256;
    for(int i=start;i<=end;i++){
        // std::cout<<"trying "<<i<<"th attempt"<<std::endl;
        if(checked==20){
            if(!do_brute_force){
                std::cout<<"thread terminating last attempt#="<<i<<std::endl;
                return;
            }
            checked=0;
        }
        std::string ith_word=get_nth(symbols,i);
        std::string ith_word_hash="";
        if(hash_type=="md5"){
            ith_word_hash=md5(ith_word);
        }
        else if(hash_type=="sha256"){
            ith_word_hash=sha256(ith_word);
        }
        if(hash==ith_word_hash){
            do_brute_force=false;
            plaintext=ith_word;
            std::cout<<"brute force found = "<<plaintext<<std::endl;
            return;
        }
        checked++;
    }
    std::cout<<"thread did not find result in search space"<<std::endl;
}

void start_brute_force(std::string& hash,
                       std::string& hash_type,
                       std::vector<char>& symbols,
                       std::string& plaintext,
                       int attempts,
                       bool& do_brute_force,
                       int threads){

    std::vector<int> starts;
    std::vector<int> ends;
    if(attempts==-1){    //default number of attempts
        attempts=10000;
    }
    int inc=attempts/threads;
    for(int i=0;i<threads;i++){
        starts.push_back(1+i*inc);
        ends.push_back((i+1)*inc);
    }
    for(int i=0;i<threads;i++){
        std::cout<<"thread "<<(i+1)<<" --> ["<<starts[i]<<","<<ends[i]<<"]"<<std::endl;
    }
    std::vector<std::thread> spawned_threads;
    for(int i=0;i<threads;i++){
        std::thread th(brute_force_thread,std::ref(symbols),std::ref(hash),std::ref(hash_type),starts[i],ends[i],std::ref(do_brute_force),std::ref(plaintext));
        std::cout<<"brute force thread "<<(i+1)<<" spawned"<<std::endl;
        spawned_threads.push_back(std::move(th));
    }
    std::cout<<spawned_threads.size()<<" threads spawned"<<std::endl;
    for(auto& thread : spawned_threads){
        thread.join();
    }
    std::cout<<spawned_threads.size()<<" threads joined"<<std::endl; 
}

};

// int main(){
//     std::vector<char> symbols{'0','1','2','3','4','5','6','7','8','9'};
//     std::string plaintext;
//     std::string hash="5919c46bc1c27ff8e17eeb7f4f14da6c";
//     std::string hash_type="md5";
//     bool terminate=0;
//     std::cout<<brute_force::get_nth(symbols,100000)<<std::endl;
//     std::thread th(brute_force::start_brute_force,std::ref(hash),std::ref(hash_type),std::ref(symbols),std::ref(plaintext),100000,std::ref(terminate),5);
//     th.join();
//     std::cout<<plaintext<<std::endl;
// }
