#include "../include/Mach/NetComponent.hpp"
#include "../include/Mach/Exception.hpp"
#include <sstream>


namespace Mach
{

using namespace std;


/* WSA flag */
bool NetComponent::_wsaEnabled(false);

/*
 * Returns the last error issued by a network-related system call on Windows
 * or Linux
 */
string NetComponent::lastError(string const call)
{
	ostringstream str;

#if defined(_WIN32) || defined(_WIN64)

        int lastError = WSAGetLastError();
		str << call << ": " << lastError
			<< " (check MSDN for more information)";

#elif defined(__gnu_linux__)

	str << call << ": " << strerror(errno);

#endif

	return str.str();
}

/*
 * Close a socket (using its file descriptor)
 */
void NetComponent::closeSocket(int const socket)
{
#if defined(__gnu_linux__)
	shutdown(socket, SHUT_RDWR);
	close(socket);
#elif defined(_WIN32) || defined(_WIN64)
	shutdown(socket, SD_BOTH);
	closesocket(socket);
#endif
}

/*
 * Initialize the WinSockAPI
 * Those calls are required for the netcode to compile and run on
 * Windows operating systems.
 */
void NetComponent::startWSA()
{
#if defined(_WIN32) || defined(_WIN64)
	if(!_wsaEnabled)
	{
		ostringstream str;

		WSADATA wsaData;
		if(WSAStartup(MAKEWORD(1,1), &wsaData) != 0)
		{
			str << "An error occured while starting the WinSockAPI."
			<< endl;
			throw Exception(str.str());
		}

		_wsaEnabled = true;
	}
#endif
}

/*
 * Shut the WinSockAPI down
 */
void NetComponent::stopWSA()
{
#if defined(_WIN32) || defined(_WIN64)
	if(_wsaEnabled)
		WSACleanup();
#endif
}

/*
 * Extracts the appropriate address (IPv4 or IPv6) into its correct pointer
 * type from the generic input sockaddr
 */
void * specializeAddress(sockaddr const * sa)
{
	switch(sa->sa_family)
	{
		/* IPv4 */
		case AF_INET:
			return &(((sockaddr_in *)(sa))->sin_addr);

		/* IPv6 */
		case AF_INET6:
			return &(((sockaddr_in6 *)(sa))->sin6_addr);

		/* Unknown family */
		default:
			return nullptr;
	}
}

/*
 * Extracts the appropriate port number into its (host) unsigned short
 * form
 */
unsigned short specializePort(sockaddr const * sa)
{
	switch(sa->sa_family)
	{
		/* IPv4 */
		case AF_INET:
			return ntohs(((sockaddr_in *)(sa))->sin_port);

		/* IPv6 */
		case AF_INET6:
			return ntohs(((sockaddr_in6 *)(sa))->sin6_port);

		/* Unknown family */
		default:
			return 0;
	}
}

/*
 * Extract a (human-readable) IP address from the given struct addrinfo
 */
string extractIP(addrinfo const * ai)
{
	char resultBuffer[INET6_ADDRSTRLEN];
	string resultString("");

	sockaddr_in *ipv4(nullptr);
	sockaddr_in6 *ipv6(nullptr);

	switch(ai->ai_family)
	{
		/* IPv4 */
		case AF_INET:
			ipv4 = (sockaddr_in *)(ai->ai_addr);
#if defined(_WIN32) || defined(_WIN64)
			InetNtopA(ai->ai_family, &(ipv4->sin_addr),
					(PSTR)resultBuffer,
					sizeof(resultBuffer));
#elif defined(__gnu_linux__)
			inet_ntop(ai->ai_family, &(ipv4->sin_addr),
					resultBuffer, sizeof(resultBuffer));
#endif
			resultString = resultBuffer;
		break;
		
		/* IPv6 */
		case AF_INET6:
			ipv6 = (sockaddr_in6 *)(ai->ai_addr);
#if defined(_WIN32) || defined(_WIN64)
			InetNtopA(ai->ai_family, &(ipv6->sin6_addr),
				(PSTR)resultBuffer, sizeof(resultBuffer));
#elif defined(__gnu_linux__)
			inet_ntop(ai->ai_family, &(ipv6->sin6_addr),
					resultBuffer, sizeof(resultBuffer));
#endif
			resultString = resultBuffer;
		break;

		/* Unknown family */
		default:
		break;
	}

	return resultString;
}

/*
 * Extract a (human-readable) port from the given struct addrinfo
 */
unsigned short extractPort(addrinfo const * ai)
{
	unsigned short port(0);

	sockaddr_in *ipv4(nullptr);
	sockaddr_in6 *ipv6(nullptr);
	
	switch(ai->ai_family)
	{
		/* IPv4 */
		case AF_INET:
			ipv4 = (sockaddr_in *)(ai->ai_addr);
			port = ntohs(ipv4->sin_port);
		break;

		/* IPv6 */
		case AF_INET6:
			ipv6 = (sockaddr_in6 *)(ai->ai_addr);
			port = ntohs(ipv6->sin6_port);
		break;

		/* Unknown family */
		default:
		break;
	}

	return port;
}

/*
 * Endianness detection constants (meant to be compared with the uint32_t value
 * from the HOST_ORDER union below)
 */
enum : uint32_t
{
	E_LITTLE_ENDIAN = 0x03020100ul,
	E_BIG_ENDIAN = 0x00010203ul
};

/*
 * Endianness test based on an union
 */
static const union
{
	unsigned char bytes[4];
	uint32_t value;
} HOST_ORDER = {{0, 1, 2, 3}};

/*
 * Used when host and network endianness are the same
 */
uint64_t same(uint64_t const & src)
{
	return src;
}

/*
 * Used when host and network endianness are reversed
 */
uint64_t reverse(uint64_t const & src)
{
	uint64_t result = 0x0000000000000000;

	// 0xAABBCCDDEEFF1122
	//
	// 0x2211FFEEDDCCBBAA

	result |= (src & 0x00000000000000FF)<<56;
	result |= (src & 0x000000000000FF00)<<40;
	result |= (src & 0x0000000000FF0000)<<24;
	result |= (src & 0x00000000FF000000)<<8;
	result |= (src & 0x000000FF00000000)>>8;
	result |= (src & 0x0000FF0000000000)>>24;
	result |= (src & 0x00FF000000000000)>>40;
	result |= (src & 0xFF00000000000000)>>56;

	return result;

//	/* Byte swap */
//	return
//		(uint64_t)(
//		(ptr[0] << 56) |
//		(ptr[1] << 48) |
//		(ptr[2] << 40) |
//		(ptr[3] << 32) |
//		(ptr[4] << 24) |
//		(ptr[5] << 16) |
//		(ptr[6] << 8)  |
//		(ptr[7]));
}

/*
 * Used when we can't handle the host's current endianness
 */
uint64_t unknown(uint64_t const & src)
{
	(void)(src);
	throw Exception("Unsupported endianness detected!");
	return 0;
}

/*
 * Compares the endianness detection constants with the effective value stored
 * in the HOST_ORDER union, then returns the appropriate endianness switching
 * implementation
 */
endianness setEndianness()
{
	switch(HOST_ORDER.value)
	{
		case E_LITTLE_ENDIAN:
			return reverse;
		break;

		case E_BIG_ENDIAN:
			return same;
		break;

		default:
			return unknown;
		break;
	}
}

/*
 * Sets the correct host to network long long and network to host long long
 * implementations
 */
endianness htonll = setEndianness();
endianness ntohll = setEndianness();

}
