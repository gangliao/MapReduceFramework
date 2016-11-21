#pragma once

#include <string>


class MapReduceImpl;

class MapReduce {
    
    public:
        MapReduce();
        ~MapReduce();
    
        // the filename here will be in absolute path
        bool run(const std::string& config_filename);

    private:
        MapReduceImpl * impl_;

};
