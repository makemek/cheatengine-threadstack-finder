#ifndef BAD_ADDRESS_EXCEPTION_HPP
#define BAD_ADDRESS_EXCEPTION_HPP

#include <exception>

namespace threadstack {
	class BadAddressException : std::exception {

	public:
		explicit BadAddressException(const char* msg, unsigned long address);
		unsigned long getAddress();

	private:
		unsigned long address;
	};
}
#endif
