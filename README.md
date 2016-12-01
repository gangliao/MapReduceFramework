# MapReduce Infrastructure

MapReduce is a programming model and an associated implementation for processing and generating large data sets. Users specify a map function that processes a key/value pair to generate a set of intermediate key/value pairs, and a reduce function that merges all intermediate values associated with the same intermediate key. Programs written in this functional style are automatically parallelized and executed on a large cluster of commodity machines. The run-time system takes care of the details of partitioning the input data, scheduling the program’s execution across a set of machines, handling machine failures, and managing the required inter-machine communication.


## Install Dependency

[How to setup the project](INSTALL.md)

## Source Code Structure

[Code walk through](structure.md)

## How to Run MapReduce Job

**This project can run successfully both on Linux and Mac OS X**

1. First, make sure you already installed gRPC and its dependent `Protocol Buffers v3.0`, check out [Install Dependency](INSTALL.md)
section to find out much more details.
2. Compile code and generate libraries
    - Goto src directory and run `make` command, two libraries would be created in external directory: `libmapreduce.a` and `libmr_worker.a`.
 
        ```bash
            cd src && make
        ```

    - Now goto test directory and run `make` command, two binaries would be created: `mrdemo` and `mr_worker`.

        ```bash
            cd test && make
        ```

3. **Now running the demo, once you have created all the binaries and libraries.**
    - Clear the files if any in the output directory

        ```bash
            rm test/output/*
        ```

    - Start all the worker processes in the following fashion:

        ```bash
            ./mr_worker localhost:50051 & ./mr_worker localhost:50052 & ./mr_worker localhost:50053 & ./mr_worker localhost:50054 & ./mr_worker localhost:50055 & ./mr_worker localhost:50056;
        ```

    - Then start your main map reduce process: `./mrdemo`

        ```bash
            ./mrdemo
        ```

    - Once the ./mrdemo finishes, kill all the worker proccesses you started.
        1. For Mac OS X:

            ```bash
                killall mr_worker
            ```

        2. For Linux:

            ```bash
                killall mr_worker
            ```

    - Check output directory to see if you have the correct results(obviously once you have done the proper implementation of your library

        ```
        .
        ├── output0.txt
        ├── output1.txt
        ├── output2.txt
        ├── output3.txt
        ├── output4.txt
        ├── output5.txt
        ├── output6.txt
        ├── output7.txt
        ├── temp0.txt
        ├── temp1.txt
        ├── temp2.txt
        ├── temp3.txt
        ├── temp4.txt
        ├── temp5.txt
        ├── temp6.txt
        └── temp7.txt

        0 directories, 16 files
        ```

# Reference

Jeffrey Dean and Sanjay Ghemawat, [MapReduce: Simplified Data Processing on Large Clusters](https://static.googleusercontent.com/media/research.google.com/en//archive/mapreduce-osdi04.pdf), 2004