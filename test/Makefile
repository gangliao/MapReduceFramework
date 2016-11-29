HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)

CXX = g++
CPPFLAGS += -I/usr/local/include -pthread
CXXFLAGS += -std=c++11 -g

ifeq ($(SYSTEM), Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -lgrpc++_reflection \
           -lprotobuf -lpthread -ldl
else
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl
endif

MAPREDUCE_LIB = -L../external/lib -lmapreduce 
MR_WORKER_LIB = -L../external/lib -lmr_worker

all: mrdemo mr_worker

mrdemo: main.o
	$(CXX) $^ -o $@ $(LDFLAGS) $(MAPREDUCE_LIB) $(LDFLAGS)

mr_worker: user_tasks.o
	$(CXX) $^ -o $@ $(LDFLAGS) $(MR_WORKER_LIB) $(LDFLAGS)

%.o: %.cc
	$(CXX) -c $< -I../external/include $(CXXFLAGS)

clean:
	rm -f *.o *.pb.cc *.pb.h mrdemo mr_worker