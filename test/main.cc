/* DON'T MAKE ANY CHANGES IN THIS FILE */

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <mapreduce.h>

#define PATH_MAX 200

int main(int argc, char **argv) {

	char* cwd;
    char buff[PATH_MAX + 1];
    cwd = getcwd( buff, PATH_MAX + 1 );
    if( cwd == NULL ) {
        std::cerr << "Failed to retrieve the current directory." << std::endl;
        return EXIT_FAILURE;
    }
	const std::string filename = std::string(cwd) + "/config.txt";
	
	MapReduce job;
	return job.run(filename) ? EXIT_SUCCESS : EXIT_FAILURE;
}