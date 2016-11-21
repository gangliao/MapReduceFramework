/* DON'T MAKE ANY CHANGES IN THIS FILE */

#include <mr_task_factory.h>
#include <iostream>


class UserMapper : public BaseMapper {

	public:
		virtual void map(const std::string& input_line) override {
			std::cout << "UserMapper, I 'm not ready yet" << std::endl;
			emit("some_key_from_map", "some_val_from_map");
		}

};


class UserReducer : public BaseReducer {

	public:
		virtual void reduce(const std::string& key, const std::vector<std::string>& values) override {
			std::cout << "UserReducer, I 'm not ready yet" << std::endl;
			emit("some_key_from_reduce", "some_val_from_reduce");
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
