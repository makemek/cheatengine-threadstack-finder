#include "InvalidHandleException.hpp"

InvalidHandleException::InvalidHandleException(const char * msg, long errorCode)
	:std::exception(msg)
{
	errCode = errorCode;
}

long InvalidHandleException::getErrorCode()
{
	return errCode;
}
