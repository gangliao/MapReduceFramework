/* DON'T MAKE ANY CHANGES IN THIS FILE */

#include <mr_task_factory.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <numeric>

class UserMapper : public BaseMapper {

	public:
		virtual void map(const std::string& input_line) override {
			char * c_input = new char [input_line.length()+1];
			std::strcpy (c_input, input_line.c_str());
			static const char* delims = " ,.\"'";
			char *start = strtok (c_input, delims);
			while (start != NULL) {
				emit(start, "1");
			    start = strtok (NULL, delims);
			}
		}
};


class UserReducer : public BaseReducer {

	public:
		virtual void reduce(const std::string& key, const std::vector<std::string>& values) override {
			std::vector<int> counts;
			std::transform(values.cbegin(), values.cend(), std::back_inserter(counts), [](const std::string numstr){ return atoi(numstr.c_str()); });
			emit(key, std::to_string(std::accumulate(counts.begin(), counts.end(), 0)));
		}

};


static std::function<std::shared_ptr<BaseMapper>() > my_mapper = 
		[] () { return std::shared_ptr<BaseMapper>(new UserMapper); };

static std::function<std::shared_ptr<BaseReducer>() > my_reducer = 
		[] () { return std::shared_ptr<BaseReducer>(new UserReducer); };


namespace {
	bool register_tasks_and_check() {
		
		const std::string user_id = "cs6210";
		if (!register_tasks(user_id, my_mapper, my_reducer)) {
			std::cout << "Failed to register user_id: " << user_id << std::endl;	
			exit (EXIT_FAILURE);
		}
		return true;
	}
}


bool just_store = register_tasks_and_check();
