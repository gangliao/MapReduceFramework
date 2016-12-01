# Setup

Clone this repository:

`git clone https://github.com/gangliao/MapReduceFramework.git`

# Dependencies 

1. `grpc` [How to Install](https://github.com/grpc/grpc/blob/master/INSTALL.md)
2. `protocol buffer` [How to Install](https://github.com/google/protobuf/blob/master/src/README.md) 
  - You need version 3.0 of protoc to be able to generate code from the given proto files.
  - I would suggest the best place to install this version is from the grpc repository itself. (`grpc/third_party/protobuf/`)
  - In the instructions for installing protoc 3.0, `make check` might fail. continue installing even after that. If compilation works fine with this new protoc, all set for the project.
3. You need to be able to compile c++11 code on your Linux system or Mac OS X

# One way to get the dependencies
1. [sudo] apt-get install build-essential autoconf libtool
2. Create a new directory somewhere where you will pull code from github to install grpc and protobuf.
     Then: `cd  $this_new_dir`
2. git clone --recursive `-b $(curl -L http://grpc.io/release)` https://github.com/grpc/grpc
3. cd  grpc/third_party/protobuf
4. sudo apt-get install autoconf automake libtool curl make g++ unzip
5. ./autogen.sh (it might fail, try further steps anyhow, might not create problems)
6. ./configure
7. sudo make
8. make check (it might fail, try further steps anyhow, might not create problems)
9. sudo make install
10. sudo ldconfig
11. cd ../../
12. make
13. sudo make install 

