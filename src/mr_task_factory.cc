/* DON'T MAKE ANY CHANGES IN THIS FILE */

#include <utility>
#include <unordered_map>
#include <functional>
#include "mr_tasks.h"
#include <mr_task_factory.h>


BaseMapper::BaseMapper() : impl_(new BaseMapperInternal) {}

BaseMapper::~BaseMapper() {}

void BaseMapper::emit(const std::string& key, const std::string& val) {
	impl_->emit(key, val);	
}


BaseReducer::BaseReducer() : impl_(new BaseReducerInternal) {}

BaseReducer::~BaseReducer() {}

void BaseReducer::emit(const std::string& key, const std::string& val) {
	impl_->emit(key, val);	
}


namespace {

	class TaskFactory
	{
		public:
		  static TaskFactory& instance();

		  std::shared_ptr<BaseMapper> get_mapper(const std::string& user_id);
		  std::shared_ptr<BaseReducer> get_reducer(const std::string& user_id);

	 	  std::unordered_map<std::string, std::function<std::shared_ptr<BaseMapper>()> > mappers_;
		  std::unordered_map<std::string, std::function<std::shared_ptr<BaseReducer>()> > reducers_;

		private:
		  TaskFactory();

	};


	TaskFactory& TaskFactory::instance() {

		static TaskFactory *instance = new TaskFactory();
		return *instance;
	}


	TaskFactory::TaskFactory() {}


	std::shared_ptr<BaseMapper> TaskFactory::get_mapper(const std::string& user_id) {
		auto itr = mappers_.find(user_id);
		if (itr == mappers_.end())
			return nullptr;
		return itr->second();
	}


	std::shared_ptr<BaseReducer> TaskFactory::get_reducer(const std::string& user_id) {
		auto itr = reducers_.find(user_id);
		if (itr == reducers_.end())
			return nullptr;
		return itr->second();
	}
}


bool register_tasks(std::string user_id,  std::function<std::shared_ptr<BaseMapper>() >& generate_mapper,
		std::function<std::shared_ptr<BaseReducer>() >& generate_reducer) {
	TaskFactory& factory = TaskFactory::instance();
	return factory.mappers_.insert(std::make_pair(user_id, generate_mapper)).second 
		&& factory.reducers_.insert(std::make_pair(user_id, generate_reducer)).second;
}

std::shared_ptr<BaseMapper> get_mapper_from_task_factory(const std::string& user_id) {
	return TaskFactory::instance().get_mapper(user_id);
}


std::shared_ptr<BaseReducer> get_reducer_from_task_factory(const std::string& user_id) {
	return TaskFactory::instance().get_reducer(user_id);
}
