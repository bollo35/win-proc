#ifndef process_h
#define process_h
// these headers must be included in this order.
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

class Process {
	public:
		enum ErrorCode 
		{
			NO_ERR,
			STDIN_CREATE_PIPE_FAIL,
			STDIN_NOINHERIT_FAIL,
			STDOUT_CREATE_PIPE_FAIL,
			STDOUT_NOINHERIT_FAIL,
			STDERR_CREATE_PIPE_FAIL,
			STDERR_NOINHERIT_FAIL,
			CREATE_PROCESS_FAIL,
			NO_ASSOCIATED_PROCESS,
			ERROR_CLOSING_PIPES,
		};

		Process();
		~Process();

		Process::ErrorCode start(std::string command, std::string working_dir="");
		bool ended();
		bool wait_until_finished(uint32_t time_out_ms);
		std::string command();
		std::string working_directory();
		// can't see a way to actually do this, it would amount
		// to just pausing and hoping it has started
		// i think waiting around ~30ms would work, since 
		// the windows scheduler timeslice seems to be around 15ms
		//bool wait_until_started(int time_out_ms);

		size_t write_stdin(std::string input);
		std::string read_stdout();
		std::string read_stderr();
		static std::string interpret_error(Process::ErrorCode);

	private:
		Process::ErrorCode           create_process();
		std::string                  read_output_stream(HANDLE stream);
		size_t                       bytes_available(HANDLE stream);
		Process::ErrorCode           close_streams();
		std::string                  _command;
		std::string                  _working_dir;
		HANDLE                       _stdin_read;
		HANDLE                       _stdin_write;
		HANDLE                       _stdout_read;
		HANDLE                       _stdout_write;
		HANDLE                       _stderr_read;
		HANDLE                       _stderr_write;
		PROCESS_INFORMATION          _pi;
};


#endif
