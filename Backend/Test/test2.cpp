#include <bits/stdc++.h>

class substring_details{
	public:
	int st,en;
	substring_details(){	
	}
	void update(int st,int en){
		this->st=st;
		this->en=en;
	}
};

class segment{
	public:
	substring_details details;
	bool is_rotatable; 
	segment(){
	}
	segment(int st,int en){
		details.update(st,en);
		is_rotatable=0;
	}
	std::string get_string(const std::string& str){
		int st=details.st;
		int en=details.en;
		return str.substr(st,en-st);
	}
};

int main(){
    std::vector<segment> arr;
    arr.push_back({1,2});
    arr.push_back({3,4});
    arr.push_back({5,6});
    std::reverse(arr.begin(),arr.end());
    for(auto elm : arr){
        std::cout<<"("<<elm.details.st<<", "<<elm.details.en<<") ";
    }
}