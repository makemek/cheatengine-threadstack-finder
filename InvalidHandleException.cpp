#include "InvalidHandleException.hpp"

threadstack::InvalidHandleException::InvalidHandleException(const char * msg, long errorCode)
	:std::exception(msg)
{
	errCode = errorCode;
}

long threadstack::InvalidHandleException::getErrorCode()
{
	return errCode;
}
