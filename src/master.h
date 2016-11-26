#pragma once

#include "mapreduce_spec.h"
#include "file_shard.h"
#include "thread_pool.h"
#include "smart_ptrs.h"

enum WORKER_STATUS {
	AVAILABLE,
	BUSY,
};

/* CS6210_TASK: Handle all the bookkeeping that Master is supposed to do.
	This is probably the biggest task for this project, will test your understanding of map reduce */
class Master {

	public:
		/* DON'T change the function signature of this constructor */
		Master(const MapReduceSpec&, const std::vector<FileShard>&);

		/* DON'T change this function's signature */
		bool run();

	private:
		inline std::string selectIdleWorker() {
			for (auto& work_addr : mr_spec_.workerAddrs) {
				if (worker_status_[work_addr] == AVAILABLE) {
					worker_status_[work_addr] = BUSY;
					return work_addr;
				}
			}
			return NULL;
		}

		/* NOW you can add below, data members and member functions as per the need of your implementation*/
		MapReduceSpec mr_spec_;
		std::unordered_map<std::string, WORKER_STATUS> worker_status_;
		std::vector<FileShard> file_shards_;
		std::unique_ptr<ThreadPool> thread_pool_;
		std::mutex mutex_;
};


/* CS6210_TASK: This is all the information your master will get from the framework.
	You can populate your other class data members here if you want */
Master::Master(const MapReduceSpec& mr_spec, const std::vector<FileShard>& file_shards) {
	// register built-in thread pool for master
	// for simplicity, thread and worker connection is one-to-one map
	thread_pool_ = make_unique<ThreadPool>(mr_spec);
	mr_spec_ = mr_spec;
	file_shards_ = std::move(file_shards);

	for (auto& work_addr : mr_spec.workerAddrs) {
		worker_status_[work_addr] = AVAILABLE;
	}
}


/* CS6210_TASK: Here you go. once this function is called you will complete whole map reduce task and return true if succeeded */
bool Master::run() { 
	std::string idleWorker;
	for (int i = 0; i < file_shards_.size(); ++i) {
		thread_pool_->AddTask([&]() {
			// find an idle worker
			do {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				{
					std::lock_guard<std::mutex> lock(mutex_);
					idleWorker = selectIdleWorker();
				}
			} while(idleWorker.empty());

			// map function ...


			// recover server to available
			worker_status_[idleWorker] = AVAILABLE; 
		});
	}
	return true;
}