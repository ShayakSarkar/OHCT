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

void print_seg_map(std::unordered_map<int,std::vector<int>>& mapping){
	for(auto& elm : mapping){
		std::cout<<elm.first<<" --> [";
		for(auto en : elm.second){
			std::cout<<en<<" ";
		}
		std::cout<<"]"<<std::endl;
	}
}

std::unordered_map<int,std::vector<int>> get_seg_map(const std::string& str,
													 const std::unordered_set<std::string>& words){
	std::unordered_map<int,std::vector<int>> seg_map; 
	for(int st=0;st<str.size();st++){
		seg_map[st]={};
		for(int en=st+1;en<=str.size();en++){
			if(words.find(str.substr(st,en-st))!=words.end()){
				seg_map[st].push_back(en);
			}
		}
	}
	return seg_map;
}

std::vector<std::vector<segment>> get_segmentations_helper(std::string& str,
														   int st,
														   std::unordered_map<int,std::vector<int>>& seg_map){
	for(int en : seg_map[st]){
		auto segmentations=get_segmentations_helper(str,en,seg_map);
	}
}

std::vector<std::vector<segment>> get_segmentations(std::string& str,
													std::unordered_map<int,std::vector<int>>& seg_map){
	return get_segmentations_helper(str,0,seg_map);
}

int main(){
	std::unordered_set<std::string> words={"s","sh","ayak","aya"};
	std::string str="shayak";
	std::unordered_map<int,std::vector<int>> seg_map=get_seg_map(str,words);
	std::vector<std::vector<segment>> result=get_segmentations(str,seg_map);
}


