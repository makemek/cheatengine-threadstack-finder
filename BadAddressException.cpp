#include "BadAddressException.hpp"

threadstack::BadAddressException::BadAddressException(const char * msg, unsigned long address)
	: std::exception(msg)
{
	this->address = address;
}

unsigned long threadstack::BadAddressException::getAddress()
{
	return this->address;
}
