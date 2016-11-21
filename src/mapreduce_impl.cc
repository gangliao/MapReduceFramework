#include <iostream>

#include "mapreduce_impl.h"
#include "master.h"


/* DON'T touch this function */
bool MapReduceImpl::run(const std::string& config_filename) {
    
    if(!read_and_validate_spec(config_filename)) {
        std::cerr << "Spec not configured properly." << std::endl;
        return false;
    }

    if(!create_shards()) {
        std::cerr << "Failed to create shards." << std::endl;
        return false;
    }

    if(!run_master()) {
        std::cerr << "MapReduce failure. Something didn't go well!" << std::endl;
        return false;
    }

    return true;
}


/* DON'T touch this function */
bool MapReduceImpl::read_and_validate_spec(const std::string& config_filename) {
    return read_mr_spec_from_config_file(config_filename, mr_spec_) && validate_mr_spec(mr_spec_);
}


/* DON'T touch this function */
bool MapReduceImpl::create_shards() {
    return shard_files(mr_spec_, file_shards_);
}


/* DON'T touch this function */
bool MapReduceImpl::run_master() {
    Master master(mr_spec_, file_shards_);
    return master.run();
}
