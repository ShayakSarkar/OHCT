#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

#include <fstream>

#include <bits/stdc++.h>

int get_seek_value_for_line_no(std::ifstream& file,int line_no){
    int nl_count=0;
    int seek_val=0;
    while(nl_count<line_no-1){
        std::string word;
        getline(file,word);
        // std::cout<<word<<std::endl;
        seek_val+=word.size()+1;
        nl_count++;
    }
    return seek_val;
}

int main(){
    sql::Driver* driver;
    sql::Connection* con;
    try{
        driver=get_driver_instance();
        con=driver->connect("tcp://127.0.0.1:3306","ProjectUser","ProjectUser");
        con->setSchema("ohct");
        int last_line_no=0;
        std::ifstream word_file("./Dictionary/common_password.txt");
        std::ifstream hash_file("./HashFiles/md5_hash.txt");
        int seek_value_word_file=get_seek_value_for_line_no(word_file,last_line_no+1);
        std::cout<<"seek="<<seek_value_word_file<<std::endl;
        int seek_value_hash_file=get_seek_value_for_line_no(hash_file,last_line_no+1);
        word_file.seekg(seek_value_word_file);
        hash_file.seekg(seek_value_hash_file);
        std::string word,hash;
        getline(word_file,word);
        getline(hash_file,hash);
        int tot=999999;
        int done=0;
        int current_perc=(int)((done*1.0/tot)*100);
        std::cout<<"done "<<current_perc<<"%"<<std::endl;
        while(getline(word_file,word) && getline(hash_file,hash)){
            // std::cout<<word<<" = "<<hash<<std::endl;
            std::string query="insert into md5_dictionary (word,md5) values (\""+word+"\",\""+hash+"\");";
            sql::Statement* stmt=con->createStatement();
            stmt->execute(query);
            done++;
            if((int)((done*1.0/tot)*100)!=current_perc){
                current_perc=(int)((done*1.0/tot)*100);
                std::cout<<"done "<<current_perc<<"%"<<std::endl;
            }
            // std::cout<<word<<" "<<hash;
            // {int t;std::cin>>t;}
        }
        // std::cout<<word<<" "<<hash<<std::endl;

    }
    catch(sql::SQLException& e){
        std::cout<<"sql exception caught"<<std::endl;
        std::cout<<e.what()<<std::endl;
    }
}