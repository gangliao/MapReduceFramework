#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>

/* CS6210_TASK Implement this data structureas per your implementation.
		You will need this when your worker is running the map task*/

struct BaseMapperInternal {

	/* DON'T change this function's signature */
	BaseMapperInternal();

	/* DON'T change this function's signature */
	void emit(const std::string& key, const std::string& val);
	std::string hash2key(const std::string& key);

	/* NOW you can add below, data members and member functions as per the need of your implementation*/
	std::mutex mutex_;
	std::string output_dir_;
	std::unordered_set<std::string> temp_files_;
	int output_num_;
};


/* CS6210_TASK Implement this function */
inline BaseMapperInternal::BaseMapperInternal() {}

inline std::string BaseMapperInternal::hash2key(const std::string& key) {
	std::hash<std::string> hash_func;
	std::string temp_file = "output/temp" +
		std::to_string(hash_func(const_cast<std::string&>(key)) % output_num_) + ".txt";
	return temp_file;
}

/* CS6210_TASK Implement this function */
inline void BaseMapperInternal::emit(const std::string& key, const std::string& val) {
	// periodically, write results of lines into intermediate files.
	std::lock_guard<std::mutex> lock(mutex_);
	std::string filename = hash2key(key);
	std::ofstream myfile(filename, std::ios::app);
	if(myfile.is_open()) {
		myfile << key << " " << val << std::endl;
		myfile.close();
	} else {
		std::cerr << "Failed to open file " << filename << std::endl;
		exit(-1);
	}
	temp_files_.insert(filename);
}


/*-----------------------------------------------------------------------------------------------*/


/* CS6210_TASK Implement this data structureas per your implementation.
		You will need this when your worker is running the reduce task*/
struct BaseReducerInternal {

	/* DON'T change this function's signature */
	BaseReducerInternal();

	/* DON'T change this function's signature */
	void emit(const std::string& key, const std::string& val);

	/* NOW you can add below, data members and member functions as per the need of your implementation*/
	int file_number_;
	std::mutex mutex_;
	std::string output_dir_;
};


/* CS6210_TASK Implement this function */
inline BaseReducerInternal::BaseReducerInternal() {}


/* CS6210_TASK Implement this function */
inline void BaseReducerInternal::emit(const std::string& key, const std::string& val) {
	std::lock_guard<std::mutex> lock(mutex_);

	std::string filename = "output/output" + std::to_string(file_number_) + ".txt"; 
	std::ofstream myfile(filename, std::ios::app);
	if(myfile.is_open()) {
		myfile << key << " " << val << std::endl;
		myfile.close();
	} else {
		std::cerr << "Failed to open file " << filename << std::endl;
		exit(-1);
	}
}
