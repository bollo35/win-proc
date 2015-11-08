#include "process.h"

Process::Process(std::string command, std::vector<std::string> args, std::string working_dir) {
	// TODO: split it up so can be called with command in one string
	_command = command;
	_args = args;
	_working_dir = working_dir;
}

Process::~Process() {
  	CloseHandle(_pi.hProcess);
	close_streams();
}
Process::ErrorCode Process::start() {
	Process::ErrorCode error_code {NO_ERR};
	// TODO: make sure pipe variables are not currently in use
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE; // makes sure pipe handles are inherited
	sa.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&_stdin_read, &_stdin_write, &sa, 0)) {
		error_code = STDIN_CREATE_PIPE_FAIL;
		goto pipe_fail;
	}

	// keep the write handle from being inherited (child doesn't need to write to it's input)
	if (!SetHandleInformation(_stdin_write, HANDLE_FLAG_INHERIT, 0)) {
		error_code = STDIN_NOINHERIT_FAIL;
		goto pipe_fail;
	}

	if (!CreatePipe(&_stdout_read, &_stdout_write, &sa, 0)) {
		error_code = STDOUT_CREATE_PIPE_FAIL;
		goto pipe_fail;
	}

	// keep the read handle from being inherited (child doesn't need to read it's own output)
	if (!SetHandleInformation(_stdout_read, HANDLE_FLAG_INHERIT, 0)) {
		error_code = STDOUT_NOINHERIT_FAIL;
		goto pipe_fail;
	}

	if (!CreatePipe(&_stderr_read, &_stderr_write, &sa, 0)) {
		error_code = STDERR_CREATE_PIPE_FAIL;
		goto pipe_fail;
	}

	// keep the read handle from being inherited (child doesn't need to read it's own output)
	if (!SetHandleInformation(_stdout_read, HANDLE_FLAG_INHERIT, 0)) {
		error_code = STDERR_NOINHERIT_FAIL;
		goto pipe_fail;
	}

	error_code = create_process();
	return error_code;

pipe_fail:
	// TODO: clean up pipes if they don't exist any more
	close_streams();
	return error_code;
}

Process::ErrorCode Process::create_process() {
	Process::ErrorCode error_code = NO_ERR;
	STARTUPINFO si;
	BOOL success = FALSE;

	// TODO: check return codes if any
	ZeroMemory(&_pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));

	si.cb = sizeof(STARTUPINFO);
	si.hStdError = _stderr_write;
	si.hStdOutput = _stdout_write;
	si.hStdInput = _stdin_read;
	si.dwFlags |= STARTF_USESTDHANDLES;

	// TODO: handle unicode and ascii?
	//       maybe just always use w_string
	success = CreateProcess(NULL,
			(TCHAR*) _command.c_str(), // FIXME: don't cast
			NULL,
			NULL,
			TRUE,
			DETACHED_PROCESS, // TODO: read up on these flags again
			NULL,
			(TCHAR*) _working_dir.c_str(), // FIXME: don't cast
			&si,
			&_pi);

	if (!success) {
		error_code = CREATE_PROCESS_FAIL;
	}
	return error_code;
}

// TODO
bool Process::ended() {
	return true;
}

// TODO: abstract INFINITE away from user?
// To wait forever set to INFINITE
// true indicates the process finished before the timeout
// false indicates any other condition (error, already finished, etc)
bool Process::wait_until_finished(int time_out_ms) {
	return WaitForSingleObject(_pi.hProcess, (DWORD) time_out_ms) == WAIT_OBJECT_0;
}

size_t Process::write_stdin(std::string input) {
	DWORD written;
	BOOL success = FALSE;

	// TODO: should this be buffered, also read function docs
	//       Error handling?
	success = WriteFile(_stdin_write, input.c_str(), input.size(), &written, NULL);
	return (size_t) written;
}

std::string Process::read_stdout() {
	return read_output_stream(_stdout_read);
}

std::string Process::read_stderr() {
	return read_output_stream(_stderr_read);
}

std::string Process::read_output_stream(HANDLE stream) {
	DWORD read, available;
	BOOL success = FALSE;
	const DWORD BUFSIZE {4096};
	std::string output {};
	TCHAR buf[BUFSIZE];
	while ((available = (DWORD) bytes_available(stream)) > 0) {
		// TODO: read function docs
		success = ReadFile(stream, buf, BUFSIZE, &read, NULL);
		// TODO: Will ReadFile null terminate strings?
		output += std::string(buf);
	}
	return output;
}

size_t Process::bytes_available(HANDLE stream) {
	BOOL success {FALSE};
	DWORD available{};

	// TODO: read docs on function
	success = PeekNamedPipe(stream, NULL, 0, NULL, &available, NULL);
	if (!success)
		available = 0;

	return (size_t) available;
}

Process::ErrorCode Process::close_streams() {
	Process::ErrorCode error_code = NO_ERR;
	bool error = false;
	error = !CloseHandle(_stdin_read);
	error = !CloseHandle(_stdin_write) || error;
	error =	!CloseHandle(_stdout_read) || error;
	error = !CloseHandle(_stdout_write) || error;
	error = !CloseHandle(_stderr_read)  || error;
	error = !CloseHandle(_stderr_write) || error;
	if (error)
		error_code = ERROR_CLOSING_PIPES;

	return error_code;
}

std::string Process::interpret_error(Process::ErrorCode ec) {
	std::string error {};
	switch (ec) {
			case STDIN_CREATE_PIPE_FAIL:
				error = "failed while creating stdin pipe";
				break;
			case STDIN_NOINHERIT_FAIL:
				error = "failed while setting stdin pipe inheritance";
				break;
			case STDOUT_CREATE_PIPE_FAIL:
				error = "failed while creating stdout pipe";
				break;
			case STDOUT_NOINHERIT_FAIL:
				error = "failed while setting stdout pipe inheritance";
				break;
			case STDERR_CREATE_PIPE_FAIL:
				error = "failed while creating stderr pipe";
				break;
			case STDERR_NOINHERIT_FAIL:
				error = "failed while setting stderr pipe inheritance";
				break;
			case CREATE_PROCESS_FAIL:
				error = "failed to create process";
				break;
			case NO_ASSOCIATED_PROCESS:
				error = "no associated process";
				break;
			case ERROR_CLOSING_PIPES:
				error = "unable to close pipes";
				break;
			case NO_ERR:
			default:
				error = "No error";
				break;
	}
	return error;
}
