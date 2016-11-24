#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "mapreduce_spec.h"

#define FILE_NAME_MAX_LEN   100      

/* CS6210_TASK: Create your own data structure here, where you can hold information about file splits,
     that your master would use for its own bookkeeping and to convey the tasks to the workers for mapping */
struct FileShard {
    std::unordered_map<std::string, std::pair<std::streampos, std::streampos> > shardsMap;
    size_t currentSize;
};

/// return total input files size in bytes
inline size_t get_totoal_size(const MapReduceSpec& mr_spec) {
    size_t totalSize = 0;
    for (auto& input : mr_spec.inputFiles) {
        std::ifstream myfile (input, std::ios::binary);
        myfile.seekg(0, std::ios::beg);
        std::streampos begin = myfile.tellg();
        myfile.seekg(0, std::ios::end);
        std::streampos end = myfile.tellg();
        totalSize += (end - begin);
    }
    return totalSize;
} 

/// return input file size in bytes
inline size_t get_input_size(std::ifstream& myfile) {
    myfile.seekg(0, std::ios::beg);
    std::streampos begin = myfile.tellg();

    myfile.seekg(0, std::ios::end);
    std::streampos end = myfile.tellg();
    return (end - begin);
} 

/* CS6210_TASK: Create fileshards from the list of input files, map_kilobytes etc. using mr_spec you populated  */ 
inline bool shard_files(const MapReduceSpec& mr_spec, std::vector<FileShard>& fileShards) {
    size_t totalSize = get_totoal_size(mr_spec);
    size_t shardNums = std::ceil((float)totalSize / (float)mr_spec.mapSize) + 1;
    fileShards.reserve(shardNums);

    for (int i = 0; i < shardNums; ++i) {
        FileShard temp;
        temp.currentSize = 0;
        fileShards.push_back(std::move(temp));
    }
    for (auto& input : mr_spec.inputFiles) {
        std::ifstream myfile (input, std::ios::binary);
        float fileSize = get_input_size(myfile);

        std::streampos offset = 0;
        while (fileSize > 0) {
            // find offset begin for a shard
            myfile.seekg(offset, std::ios::beg);
            std::streampos begin = myfile.tellg();

            // find offset end for a shard
            offset += mr_spec.mapSize;
            myfile.seekg(offset, std::ios::beg);
            std::streampos end = myfile.tellg();
            // if offset exceed size, set its end position
            if (end == -1) {
                myfile.seekg(0, std::ios::end);
                end = myfile.tellg();
            }

            size_t chunkSize = (end - begin);
            fileSize -= chunkSize;
            for (auto& shard : fileShards) {
                size_t restSize = mr_spec.mapSize - shard.currentSize;
                if (restSize >= chunkSize) {
                    shard.currentSize += chunkSize;
                    shard.shardsMap[input] =
                        std::make_pair<std::streampos, std::streampos>(begin, end);
                    break;
                } else {
                    std::streampos chunk_beg = begin;
                    std::streampos chunk_end = begin;
                    chunk_end += restSize;

                    shard.currentSize += restSize; 
                    shard.shardsMap[input] =
                        std::make_pair<std::streampos, std::streampos>(chunk_beg, chunk_end);

                    chunkSize -= restSize;
                    begin += (restSize + 1);
                }
            }
            offset = end;
            offset += 1;
        }
        myfile.close();
    }
	return true;
}
