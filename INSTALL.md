# Setup
Clone this repository:
`git clone https://github.gatech.edu/akumar401/cs6210Project4.git`

# Dependencies 
**This is same as project 3. So you can skip the rest of this page, if you have the same system on which you developed the previous project**
  1. `grpc` [How to Install](https://github.com/grpc/grpc/blob/master/INSTALL.md)
  2. `protocol buffer` [How to Install](https://github.com/google/protobuf/blob/master/src/README.md) 
    - You need version 3.0 of protoc to be able to generate code from the given proto files.
    - I would suggest the best place to install this version is from the grpc repository itself. (`grpc/third_party/protobuf/`)
    - In the instructions for installing protoc 3.0, `make check` might fail. continue installing even after that. If compilation works fine with this new protoc, all set for the project.
  3. You need to be able to compile c++11 code on your Linux system

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

# Keeping your code upto date
Although you are going to submit your solution through t-square only, after the fork followed by clone, we recommend creating a branch and developing your code on that branch:

`git checkout -b develop`

(assuming develop is the name of your branch)

Should the TAs need to push out an update to the assignment, commit (or stash if you are more comfortable with git) the changes that are unsaved in your repository:

`git commit -am "<some funny message>"`

Then update the master branch from remote:

`git pull origin master`

This updates your local copy of the master branch. Now try to merge the master branch into your development branch:

`git merge master`

(assuming that you are on your development branch)

There are likely to be merge conflicts during this step. If so, first check what files are in conflict:

`git status`

The files in conflict are the ones that are "Not staged for commit". Open these files using your favourite editor and look for lines containing `<<<<` and `>>>>`. Resolve conflicts as seems best (ask a TA if you are confused!) and then save the file. Once you have resolved all conflicts, stage the files that were in conflict:

`git add -A .`

Finally, commit the new updates to your branch and continue developing:

`git commit -am "<I did it>"`
