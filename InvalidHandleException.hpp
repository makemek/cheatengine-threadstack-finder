#ifndef INVALID_HANDLE_EXCEPTION_HPP
#define INVOIDE_HANDLE_EXCEPTION_HPP

#include <exception>

class InvalidHandleException: std::exception {

public:
	explicit InvalidHandleException(const char* msg, long errorCode);
	long getErrorCode();

private:
	int errCode;
};

#endif
