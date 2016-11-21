/* DON'T MAKE ANY CHANGES IN THIS FILE */

#include <mapreduce.h>
#include "mapreduce_impl.h"


MapReduce::MapReduce() 
	: impl_(new MapReduceImpl())
	{}	


MapReduce::~MapReduce() {
	delete impl_;
}	


bool MapReduce::run(const std::string& config_filename) {
	return impl_->run(config_filename);
}
