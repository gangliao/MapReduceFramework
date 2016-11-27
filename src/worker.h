#pragma once

#include <grpc++/grpc++.h>
#include <mr_task_factory.h>
#include <iostream>
#include <fstream>

#include "mr_tasks.h"
#include "masterworker.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using masterworker::MasterWorker;
using masterworker::MasterQuery;
using masterworker::WorkerReply;
using masterworker::ShardInfo;

/* CS6210_TASK: Handle all the task a Worker is supposed to do.
	This is a big task for this project, will test your understanding of map reduce */
class Worker {

	public:
		/* DON'T change the function signature of this constructor */
		Worker(std::string ip_addr_port);
		~Worker() {
			server_->Shutdown();
    		// Always shutdown the completion queue after the server.
    		cq_->Shutdown();
		}

		/* DON'T change this function's signature */
		bool run();

	private:
		/* NOW you can add below, data members and member functions as per the need of your implementation*/
		// Class encompasing the state and logic needed to serve a request.
		friend class CallData;
		// This can be run in multiple threads if needed.
		void HandleRpcs();

		std::unique_ptr<ServerCompletionQueue> cq_;
  		MasterWorker::AsyncService service_;
  		std::unique_ptr<Server> server_;
};


class CallData {

	public:
		// Take in the "service" instance (in this case representing an asynchronous
		// server) and the completion queue "cq" used for asynchronous communication
		// with the gRPC runtime.
		CallData(MasterWorker::AsyncService* service, ServerCompletionQueue* cq)
			: service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
			// Invoke the serving logic right away.
			Proceed();
		}

		void Proceed();

	private:
		void MapProceed();
		void ReduceProceed();
		
		// The means of communication with the gRPC runtime for an asynchronous
		// server.
		MasterWorker::AsyncService* service_;
		// The producer-consumer queue where for asynchronous server notifications.
		ServerCompletionQueue* cq_;
		// Context for the rpc, allowing to tweak aspects of it such as the use
		// of compression, authentication, as well as to send metadata back to the
		// client.
		ServerContext ctx_;

		// What we get from the client.
		MasterQuery request_;
		// What we send back to the client.
		WorkerReply reply_;

		// The means to get back to the client.
		ServerAsyncResponseWriter<HelloReply> responder_;

		// Let's implement a tiny state machine with the following states.
		enum CallStatus { CREATE, PROCESS, FINISH };
		CallStatus status_;  // The current serving state.
};


void CallData::Proceed() {
	if (status_ == CREATE) {
		// Make this instance progress to the PROCESS state.
		status_ = PROCESS;

		// As part of the initial CREATE state, we *request* that the system
		// start processing SayHello requests. In this request, "this" acts are
		// the tag uniquely identifying the request (so that different CallData
		// instances can serve different requests concurrently), in this case
		// the memory address of this CallData instance.
		service_->RequestmapReduce(&ctx_, &request_, &responder_, cq_, cq_, this);
	} else if (status_ == PROCESS) {
		// Spawn a new CallData instance to serve new clients while we process
		// the one for this CallData. The instance will deallocate itself as
		// part of its FINISH state.
		new CallData(service_, cq_);

		// The actual processing.
		{
			if(request_.is_map()) {
				// map function
				MapProceed();
			} else {
				// reduce function
				ReduceProceed();
			}
			std::string prefix("Hello ");
			reply_.set_message(prefix + request_.name());
		}

		// And we are done! Let the gRPC runtime know we've finished, using the
		// memory address of this instance as the uniquely identifying tag for
		// the event.
		status_ = FINISH;
		responder_.Finish(reply_, Status::OK, this);
	} else {
		GPR_ASSERT(status_ == FINISH);
		// Once in the FINISH state, deallocate ourselves (CallData).
		delete this;
	}
}

void CallData::MapProceed() {
	// init output_num for hash the key into R regions
	BaseMapperInternal::output_num_ = request_.output_num();

	// find the corresponding map function
	auto mapper = get_mapper_from_task_factory(request.user_id());

	int size = request_.shard_size();
	for (int i = 0; i < size; ++i) {
		// read shard info from proto
		ShardInfo shard_info = request_.shard(i);
		std::string filename = shard_info.filename();
		std::streampos off_start = shard_info.off_start();
		std::streampos off_end = shard_info.off_end();
		
		std::ifstream myfile(filename, std::ios::binary);
		if (myfile.is_open()) {
			// find file shard: begin offset
			myfile.seekg(off_start);
			std::string line;
			while (getline(myfile, line)) {	
				mapper->map(line);
				if (off_end == myfile.tellg()) {
					// reach file shard: end offset
					break;
				}
			}
			myfile.close();
		} else {
			std::cerr << "Failed to open file " << filename << std::endl;
			exit(-1);
		}
	}
}

void CallData::ReduceProceed() {
	std::string filename = request_.location();
	std::ifstream myfile(filename, std::ios::binary);

	std::unorder_map<std::string, std::vector<std::string> > kv_store;

	if (myfile.is_open()) {
		std::string line;
		while (getline(myfile, line)) {
			char key[100];
			int value;
			sscanf(line,c_str(), "%s %d", key, &value);
			kv_store[key].push_back(std::to_string(value));
		}
		myfile.close();
	} else {
		std::cerr << "Failed to open file " << filename << std::endl;
		exit(-1);
	}

	// sort all key : list value pairs

	// reducer function
	auto reducer = get_reducer_from_task_factory(request.user_id());

	for (auto& kv : kv_store) {
		reducer->reduce(kv.first, kv.second);
	}
}

/* CS6210_TASK: ip_addr_port is the only information you get when started.
	You can populate your other class data members here if you want */
Worker::Worker(std::string ip_addr_port) {
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(ip_addr_port, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    cq_ = builder.AddCompletionQueue();
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << ip_addr_port << std::endl;
}

void Worker::HandleRpcs() {
	// Spawn a new CallData instance to serve new clients.
	new CallData(&service_, cq_.get());
	void* tag;  // uniquely identifies a request.
	bool ok;
	while (true) {
		// Block waiting to read the next event from the completion queue. The
		// event is uniquely identified by its tag, which in this case is the
		// memory address of a CallData instance.
		// The return value of Next should always be checked. This return value
		// tells us whether there is any kind of event or cq_ is shutting down.
		GPR_ASSERT(cq_->Next(&tag, &ok));
		GPR_ASSERT(ok);
		static_cast<CallData*>(tag)->Proceed();
	}
}

extern std::shared_ptr<BaseMapper> get_mapper_from_task_factory(const std::string& user_id);
extern std::shared_ptr<BaseReducer> get_reducer_from_task_factory(const std::string& user_id);

/* CS6210_TASK: Here you go. once this function is called your woker's job is to keep looking for new tasks 
	from Master, complete when given one and again keep looking for the next one.
	Note that you have the access to BaseMapper's member BaseMapperInternal impl_ and 
	BaseReduer's member BaseReducerInternal impl_ directly, 
	so you can manipulate them however you want when running map/reduce tasks*/
bool Worker::run() {
	/*  Below 5 lines are just examples of how you will call map and reduce
		Remove them once you start writing your own logic */
	
	// Proceed to the server's main loop.
    HandleRpcs();

	return true;
}
