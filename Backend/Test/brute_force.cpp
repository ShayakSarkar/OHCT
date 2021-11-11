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

void make_nth_new(std::vector<char>& symbols,int n,std::vector<char>& word_list){
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

};


// int main(){
//     std::vector<char> symbols={'a','b','c','d'};
//     for(int i=0;i<50;i++){
//         std::cout<<get_nth(symbols,i)<<std::endl;
//     }
// }

int main(){
    std::vector<char> symbols={'1','2','3','4','5','6','7','8','9'};
    std::vector<int> array={};
    for(int i=1;i<1000;i++){
        std::string str=brute_force::get_nth(symbols,i);
        std::cout<<str<<std::endl;
        array.push_back(std::stoi(str));
    }
    for(int i=1;i<array.size();i++){
        std::cout<<array[i]<<std::endl;
        if(array[i]!=array[i-1]+1){
            std::cout<<"failed"<<std::endl;
            return 1;
        }
    }
    std::cout<<"success"<<std::endl;
}