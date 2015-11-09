#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "process.h"

int main(void) {
	std::string command = "cmd /C dir";
	std::string working_dir = "C:/";
	Process p;
	Process::ErrorCode ec = p.start(command, working_dir);
	std::cout<< "start: "<< Process::interpret_error(ec) << std::endl;
	// "cmd /C dir" runs the command and then exits, so it's ok to use wait_until_finished(INFINITE)
	// for a long running process, you probably wouldn't do this
	if (p.wait_until_finished(INFINITE)) {
		std::string output = p.read_stdout();
		std::cout<< output << std::endl;
	} else {
		std::cout<<"error while waiting!"<<std::endl;
	}
	
	return 0;
}
