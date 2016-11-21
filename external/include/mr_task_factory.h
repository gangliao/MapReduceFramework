/* DON'T MAKE ANY CHANGES IN THIS FILE */

#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <functional>

class Worker;

class BaseMapperInternal;
/* Base Mapper class which provides interface that needs to be implemented by the user for their task type*/
class BaseMapper {

	public:
		BaseMapper();
		virtual ~BaseMapper();
		
		virtual void map(const std::string& input_line) = 0;
		void emit(const std::string& key, const std::string& val);

	private:
		friend class Worker;
		BaseMapperInternal* impl_;
};


class BaseReducerInternal;
/* Base Reducer class which provides interface that needs to be implemented by the user for their task type*/
class BaseReducer {

	public:
		BaseReducer();
		virtual ~BaseReducer();
		
		virtual void reduce(const std::string& key, const std::vector<std::string>& values) = 0;
		void emit(const std::string& key, const std::string& val);

	private:
		friend class Worker;
		BaseReducerInternal* impl_;
};


/* Register user's implementation of the tasks with a user id same as user_id in the config.ini */
bool register_tasks(std::string user_id, std::function<std::shared_ptr<BaseMapper>() >& generate_mapper, 
		std::function<std::shared_ptr<BaseReducer>() >& generate_reducer);
