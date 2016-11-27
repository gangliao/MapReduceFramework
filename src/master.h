#pragma once

#include <grpc++/grpc++.h>
#include "masterworker.grpc.pb.h"
#include "mapreduce_spec.h"
#include "file_shard.h"
#include "thread_pool.h"
#include "smart_ptrs.h"

using masterworker::MasterWorker;
using masterworker::MasterQuery;
using masterworker::WorkerReply;
using masterworker::ShardInfo;

namespace {
	enum WORKER_STATUS {
		AVAILABLE,
		BUSY,
	};
}

/* CS6210_TASK: Handle all the bookkeeping that Master is supposed to do.
	This is probably the biggest task for this project, will test your understanding of map reduce */
class Master {

	public:
		/* DON'T change the function signature of this constructor */
		Master(const MapReduceSpec&, const std::vector<FileShard>&);

		/* DON'T change this function's signature */
		bool run();

	private:
		bool runMapProc();
		bool runReduceProc();
		bool remoteCallMap(const std::string& ip_addr_port, const FileShard& file_shard);
		bool remoteCallReduce(const std::string& ip_addr_port, const std::string& file_name);
		std::string selectIdleWorker();

		/* NOW you can add below, data members and member functions as per the need of your implementation*/
		MapReduceSpec mr_spec_;
		std::unordered_map<std::string, WORKER_STATUS> worker_status_;
		std::vector<FileShard> file_shards_;
		std::unordered_set<std::string> temp_filename_;
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

inline std::string Master::selectIdleWorker() {
	for (auto& work_addr : mr_spec_.workerAddrs) {
		if (worker_status_[work_addr] == AVAILABLE) {
			worker_status_[work_addr] = BUSY;
			return work_addr;
		}
	}
	return NULL;
}

bool Master::runMapProc() {
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
			GRPC_ASSERT(remoteCallMap(idleWorker, file_shards_[i]));
		});
	}
	return true;
}

bool Master::remoteCallMap(const std::string& ip_addr_port, const FileShard& file_shard) {
	std::unique_ptr<MasterWorker::Stub> stub_ = MasterWorker::NewStub(
		grpc::CreateChannel(ip_addr_port, grpc::InsecureChannelCredentials())
	);

	// 1. set grpc query parameters
	MasterQuery query;
	qurey.set_is_map(true);
	query.set_output_num(mr_spec_.outputNums);
	query.set_location(nullptr);

	for (auto& shardmap : file_shard.shardsMap) {
		ShardInfo* shard_info = query.add_shard();
		shard_info->set_filename(shardmap.first);
		shard_info->set_off_start(shardmap.second.first);
		shard_info->set_off_end(shardmap.second.second);
	}

	// 2. set async grpc service
	WorkerReply reply;
	grpc::ClientContext context;
	grpc::CompletionQueue cq;
	grpc::Status status;
	
	std::unique_ptr<grpc::ClientAsyncResponseReader<WorkerReply> > rpc(
		stub_->AsyncmapReduce(&context, query, &cq));

	rpc->Finish(&reply, &status, (void*)1);
	void* got_tag;
	bool ok = false;
	GPR_ASSERT(cq.Next(&got_tag, &ok));
	GPR_ASSERT(got_tag == (void*)1);
	GPR_ASSERT(ok);

	if (!status.ok()) {
		std::cout << status.error_code() << ": " << status.error_message()
				<< std::endl;
		return false;
	}

	// 3. finish grpc, master receive intermediate file names
	temp_filename_.insert(reply.location());

	// 4. recover server to available
	worker_status_[ip_addr_port] = AVAILABLE;

	return true;
}

bool Master::runMapProc() {
	std::string idleWorker;
	for(auto& temp_input : temp_filename_) {
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
			GRPC_ASSERT(remoteCallReduce(idleWorker, temp_input));
		});
	}
	return true;
}


bool Master::remoteCallReduce(const std::string& ip_addr_port, const std::string& file_name) {
	std::unique_ptr<MasterWorker::Stub> stub_ = MasterWorker::NewStub(
		grpc::CreateChannel(ip_addr_port, grpc::InsecureChannelCredentials())
	);

	// 1. set grpc query parameters
	MasterQuery query;
	qurey.set_is_map(false);	// reduce procedure
	query.set_location(file_name);

	// 2. set async grpc service
	WorkerReply reply;
	grpc::ClientContext context;
	grpc::CompletionQueue cq;
	grpc::Status status;
	
	std::unique_ptr<grpc::ClientAsyncResponseReader<WorkerReply> > rpc(
		stub_->AsyncmapReduce(&context, query, &cq));

	rpc->Finish(&reply, &status, (void*)1);
	void* got_tag;
	bool ok = false;
	GPR_ASSERT(cq.Next(&got_tag, &ok));
	GPR_ASSERT(got_tag == (void*)1);
	GPR_ASSERT(ok);

	if (!status.ok()) {
		std::cout << status.error_code() << ": " << status.error_message()
				<< std::endl;
		return false;
	}

	// 3. finish grpc
	if (reply.is_done()) {
		std::cout << "map reduce job finished ..." << std::endl;
	}

	// 4. recover server to available
	worker_status_[ip_addr_port] = AVAILABLE;

	return true;
}

/* CS6210_TASK: Here you go. once this function is called you will complete whole map reduce task and return true if succeeded */
bool Master::run() {
	GPR_ASSERT(runMapProc());
	// for simplicity, once all map tasks done, reduce will start to execution
	GRP_ASSERT(runReduceProc());
	return true;
}