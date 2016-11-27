CXX = g++
CPPFLAGS += -I/usr/local/include -pthread
CXXFLAGS += -std=c++11 -g
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./

vpath %.proto $(PROTOS_PATH)

MAPREDUCE_LIB = ../external/lib/libmapreduce.a
MR_WORKER_LIB = ../external/lib/libmr_worker.a

all: system-check $(MAPREDUCE_LIB) $(MR_WORKER_LIB)

$(MAPREDUCE_LIB): masterworker.pb.o masterworker.grpc.pb.o mapreduce_impl.o mapreduce.o
	ar -rcs $@ masterworker.pb.o masterworker.grpc.pb.o mapreduce_impl.o mapreduce.o
	ranlib $@

$(MR_WORKER_LIB): masterworker.pb.o masterworker.grpc.pb.o mr_task_factory.o run_worker.o
	ar -rcs $@ masterworker.pb.o masterworker.grpc.pb.o mr_task_factory.o run_worker.o
	ranlib $@

%.o: %.cc
	$(CXX) -c $< -I../external/include $(CXXFLAGS)

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	chmod 544 *.grpc.pb.* || true
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
	chmod 444 *.grpc.pb.*

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	chmod 544 *.pb.* || true
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
	chmod 444 *.pb.*

clean:
	rm -f *.o *.pb.cc *.pb.h $(MAPREDUCE_LIB) $(MR_WORKER_LIB)


# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif
