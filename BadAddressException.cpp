#include "BadAddressException.hpp"

BadAddressException::BadAddressException(const char * msg, unsigned long address)
	: std::exception(msg)
{
	this->address = address;
}

unsigned long BadAddressException::getAddress()
{
	return this->address;
}
