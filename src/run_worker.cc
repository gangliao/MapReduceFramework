#include "worker.h"


int main(int argc, char** argv) {
	std::string ip_addr_port;
		if (argc == 2) {
			ip_addr_port = std::string(argv[1]);
		}
		else {
			std::cerr << "Correct usage: [$binary_name $ip_addr_port], example: [./mr_worker localhost:50051]" << std::endl;
			return EXIT_FAILURE;
		}

	Worker worker(ip_addr_port);
	return worker.run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
