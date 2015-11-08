#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "process.h"

int main(void) {
	std::string command = "cmd /C dir";
	std::vector<std::string> args;
	std::string working_dir = "C:/";
	Process p(command, args, working_dir);
	Process::ErrorCode ec = p.start();
	std::cout<< "start: "<< Process::interpret_error(ec) << std::endl;
	auto sleep_time = std::chrono::duration<double, std::milli>(100);
	std::this_thread::sleep_for(sleep_time);
	
	std::string output = p.read_stdout();
	std::cout<< output << std::endl;
	return 0;
}
