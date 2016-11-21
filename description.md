### Big picture
  - In this project, you are going to implement a simplied version of Map Reduce infrastructure. Please read [MapReduce](https://static.googleusercontent.com/media/research.google.com/en//archive/mapreduce-osdi04.pdf) paper, before you start.
  - This is going to be a long project, **Start late at your own risk** 

![Meme](https://memecrunch.com/meme/1HMYR/should-have-started-earlier/image.jpg?w=500&c=1)

### About MapReduce

MapReduce is a programming model and an associated implementation for processing and generating large data sets. Users specify a map function that processes a key/value pair to generate a set of intermediate key/value pairs, and a reduce function that merges all intermediate values associated with the same intermediate key. Programs written in this functional style are automatically parallelized and executed on a large cluster of commodity machines. The run-time system takes care of the details of partitioning the input data, scheduling the program’s execution across a set of machines, handling machine failures, and managing the required inter-machine communication.

### Learning outcomes
  - MapReduce Infrastructure

### What's simplified here (comparing from the original paper)
1. **`The MapReduce library in the user program first splits the input files into M pieces of typically 16 megabytes to 64 megabytes (MB) per piece (controllable by the user via an optional parameter). It then starts up many copies of the program on a cluster of machines.`**
 - Instead of writing new split files, you will just store the offsets from the original file as the file shards.
 - **Sharding**: Your should divide your input into M number of shards, whereas M is number of file shards. They should be roughly of the same size specified in the config file. For example, if `map_kilobytes`(shard size) in the config file is 16 and you are given 3 input files of sizes 20kb, 15kb, 20kb then your **calculated** `number of shards M` should be `ceil(55/16) = 4`, and the shards will approximately look like:
         - shard 1 - ((file: file1, offsets: 0-16kb))
         - shard 2 - ((file: file1, offsets: 16-20kb), (file: file2, offsets: 0-12kb))
         - shard 3 - ((file: file2, offsets: 12-15kb), (file: file3, offsets: 0-13kb))
         - shard 4 - ((file: file3, offsets: 13-20kb))

 - BUT, you want your input shards to have complete record entries, for example, when your framework is running a word count program, then you should not be splitting in the middle of a word. For this reason and to make it simple, you should align your shard on '\n', i.e. new line which is roughly closest to the shard's end offset, calculated by above logic. You are free to choose to bring your end offset to the previous '\n' or the next '\n'.
 - Instead of the map reduce library statring worker processes, you will be manually starting them up before running the main binary.
 - Instead of running worker programs on different machines, you will start your development on a single machine by starting multiple instances of your woker program as processes on the same machine listening to different ports. 


2. **`One of the copies of the program is special – the master. The rest are workers that are assigned work by the master. There are M map tasks and R reduce tasks to assign. The master picks idle workers and assigns each one a map task or a reduce task.`**
 - Instead of running your master as a different process, you will be simply making a function call from the framework itself to the master to run and return back(already done for you in the code base).
 - Once you have M input shards created, your master is supposed to assign each shard to one of the available workers.
 - Your master can read the worker process addresses (ip:port) from the MapReduce specification structure provided by the framework.
 - Your master will essentially be maintaining a worker pool, taking care of various things such as tracking how many workers are there in the system, what is the state of each worker: `AVAIALABLE, BUSY(doing map task/doing reduce task, etc.)`, when to assign what task to a worker, knowing when a worker is done.
 - `The communication of relevant instructions/data/results to/from the workers will be done through GRPC calls`. For example, the master can inform a mapper worker about the shard that it will be processing by passing the shard info in a grpc message. Who will be a client and who will be the server? What kind of message signature you will use? You will need to define your own proto files and master-worker (GRPC client-server) interfaces here.
  - Your master is also given the number R, i.e., number of output files from the config. For simplicity, you can `start your reduce phase when ALL of the map tasks are done`.
 
 
3. **`A worker who is assigned a map task reads the contents of the corresponding input split. It parses key/value pairs out of the input data and passes each pair to the user-defined Map function. The intermediate key/value pairs produced by the Map function are buffered in memory.`**
 - Once a worker is assigned a 'map' task by the master, how should it process the input shard assigned to it? 
 - Your application logic (Word count - counting the number of occurences for each word in the input, or finding mean-max temperature for each month from the given record of temperatures, etc.) should be written in BaseMapper's implementation(UserMapper: see the provided code for more details).
 - You should call 'map' from the user's mapper (UserMapper's map()) on each record one-by-one. We will assume that the records are separated by '\n'.
 - What is the output of map function? A `key, value` pair. 
 - How will you pass these key-value pairs to the reducers? You should create intermediate files on the disk. 
 - How many intermediate files should you create per mapper worker? Why should this matter? 
 - How will you decide which `key, value` pair is written to which intermediate file? Do you need to hash the keys?


4. **`Periodically, the buffered pairs are written to local disk, partitioned into R regions by the partitioning function. The locations of these buffered pairs on the local disk are passed back to the master, who is responsible for forwarding these locations to the reduce workers.`**
 - Once the mappers have writen the output key, value pairs to the intermediate files, they need to pass the following information back to the master: `Indicate that the map task is done`, `Where the intermediate files are located`.
 - Your master is then supposed to start the reducer workers and then assign the respective intermediate files to each of the reducer worker. Which files should get assigned to which reducer?
 
  
5. **`When a reduce worker is notified by the master about these locations, it uses remote procedure calls to read the buffered data from the local disks of the map workers. When a reduce worker has read all intermediate data, it sorts it by the intermediate keys so that all occurrences of the same key are grouped together. The sorting is needed because typically many different keys map to the same reduce task.`**
 - Theoretically, in real distributed environment, the intermediate files lie on local disks of mapper workers and reducers need to make remote file reads to get the data into their own local memory. However, for simplicity, you can have the intermediate files on the same file system as of your reducer worker, and hence you can read them through local file read system calls.
 - You have to take user's reducer logic (UserReducer's reduce()) in consideration, similar to how you handled user's mapper logic/algorithm.
 - You have one more constraint on the final output though. You have to make sure that the final output is sorted on its keys.


6. **`The reduce worker iterates over the sorted intermediate data and for each unique intermediate key encountered, it passes the key and the corresponding set of intermediate values to the user’s Reduce function. The output of the Reduce function is appended to a final output file for this reduce partition.`**
 - This would be done the same way in your implementation. Make sure each reducer has its own file(create in the user given output directory) to which it appends its results.


7. **`When all map tasks and reduce tasks have been completed, the master wakes up the user program. At this point, the MapReduce call in the user program returns back to the user code.`**
 - This would be done the same way in your implementation. Waking up is simply the return from the function call.


### How You Are Going to Implement It
- [Code walk through](structure.md)

### Grading
This project is not performance oriented, we will only test the functionality.
The Rubric is:

- **5.0 points** - `Framework & Master`: Handling config files, sharding, managing worker pool, assigning tasks to the workers, tracking worker progress, handling stragglers.
- **2.5 points** - `Mapper`: Reading input shard, passing data to the BaseMapper's implentation(UserMapper class), receiving output key, value pairs from the it and writig them to correct intermediate files.
- **2.5 points** - `Reducer`: Reading respective intermediate files, passing the key, value pairs to the BaseReducer's implementation(UserReducer class), writing the output key, value pairs from it to the output files, ensuring that output is sorted on the output keys.

### Deliverables
1. Please follow the instructions carefully. **You can handle the whole src folder as it is.** Specficially the folder you hand in must contain the following:
  - `README` - Optional text file containing anything specific about the project that you want to tell the TAs.
  - `Makefile` - It is already given to you working for the files mentioned below. You might need to change it if you add more source files.
  - `Proto file` - **masterworker.proto** containing the grpc specification between master and worker.
  - `Master source files` - **master.h** containing the source code for master management. Note that you can add optional supporting files for master if you want.
  - `Worker source files` - **worker.h** containing the source code for worker management. Note that you can add optional supporting files for master if you want.
  - `MR tasks source file` - **mr_tasks.h** containing the source code for Mapper/Reducer Internal implementation. All your data structure and logic for internal mapper/reducer should be in this file only, no supporting files.
  - `File Sharding source file` - **file_shard.h** containing the source code for file splitting logic. All your data structure and logic for file sharding should be in this file only, no supporting files.
  - `MapReduce Specification source file` - **mapreduce_spec.h** containing the source code for map reduce specification. All your data structure and logic for map reduce specification should be in this file only, no supporting files.
  - Again, if you have used additional supporting source files, don't forget to submit them.
2. Hand in your folder as a zip file through [T-Square](t-square.gatech.edu)

