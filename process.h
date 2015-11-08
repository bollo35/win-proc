#ifndef process_h
#define process_h

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>
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

		Process(std::string command, std::vector<std::string> args, std::string working_dir);
		~Process();

		Process::ErrorCode start();
		bool ended();

		size_t write_stdin(std::string input);
		std::string read_stdout();
		std::string read_stderr();
		static std::string interpret_error(Process::ErrorCode);

	private:
		Process::ErrorCode            create_process();
		std::string                  read_output_stream(HANDLE stream);
		size_t                       bytes_available(HANDLE stream);
		Process::ErrorCode           close_streams();
		std::string                  _command;
		std::vector<std::string>     _args;
		std::string                  _working_dir;
		HANDLE                       _stdin_read;
		HANDLE                       _stdin_write;
		HANDLE                       _stdout_read;
		HANDLE                       _stdout_write;
		HANDLE                       _stderr_read;
		HANDLE                       _stderr_write;
};


#endif
