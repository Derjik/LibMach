/*
 * Copyright (c) 2016 Julien "Derjik" Laurent <julien.laurent@engineer.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "../include/Mach/NetComponent.hpp"
#include "../include/Mach/Exception.hpp"
#include <sstream>


namespace Mach
{

using namespace std;


/* WSA flag */
bool NetComponent::_wsaEnabled(false);

htonllImpl NetComponent::htonll = setHtonll();
ntohllImpl NetComponent::ntohll = setNtohll();
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
			<< " (please contact <julien.laurent@engineer.com>)";

#elif defined(__gnu_linux__)

	str << call << ": " << strerror(errno);

#endif

	return str.str();
}

/*
 * Extracts the appropriate address (IPv4 or IPv6) into its correct pointer
 * type from the generic input sockaddr
 */
void * NetComponent::specializeAddress(sockaddr const * sa)
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
unsigned short NetComponent::specializePort(sockaddr const * sa)
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
string NetComponent::extractIP(addrinfo const * ai)
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
			InetNtop(ai->ai_family, &(ipv4->sin_addr),
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
			InetNtop(ai->ai_family, &(ipv6->sin6_addr),
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
unsigned short NetComponent::extractPort(addrinfo const * ai)
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
 * Shutdown the WinSockAPI
 */
void NetComponent::stopWSA()
{
#if defined(_WIN32) || defined(_WIN64)
	if(_wsaEnabled)
		WSACleanup();
#endif
}

uint64_t htonllBigEndian(uint64_t const & src)
{
	return src;
}

uint64_t htonllLittleEndian(uint64_t const & src)
{
	uint8_t* ptr((uint8_t*)(&src));

	return
		(uint64_t)(
		(ptr[0] << 56) |
		(ptr[1] << 48) |
		(ptr[2] << 40) |
		(ptr[3] << 32) |
		(ptr[4] << 24) |
		(ptr[5] << 16) |
		(ptr[6] << 8)  |
		(ptr[7]));
}

uint64_t htonllUnknownEndian(uint64_t const & src)
{
	throw Exception("Unsupported endianness detected!");
	return 0;
}

uint64_t ntohllBigEndian(uint64_t const & src)
{
	return src;
}

uint64_t ntohllLittleEndian(uint64_t const & src)
{
	uint8_t* ptr((uint8_t*)(&src));

	return
		(uint64_t)(
		(ptr[0] << 56) |
		(ptr[1] << 48) |
		(ptr[2] << 40) |
		(ptr[3] << 32) |
		(ptr[4] << 24) |
		(ptr[5] << 16) |
		(ptr[6] << 8)  |
		(ptr[7]));
}

uint64_t ntohllUnknownEndian(uint64_t const & src)
{
	throw Exception("Unsupported endianness detected!");
	return 0;
}

htonllImpl setHtonll()
{
	switch(HOST_ORDER.value)
	{
		case LITTLE_ENDIAN:
			return htonllLittleEndian;
		break;

		case BIG_ENDIAN:
			return htonllBigEndian;
		break;

		default:
			return htonllUnknownEndian;
		break;
	}
}

ntohllImpl setNtohll()
{
	switch(HOST_ORDER.value)
	{
		case LITTLE_ENDIAN:
			return ntohllLittleEndian;
		break;

		case BIG_ENDIAN:
			return ntohllBigEndian;
		break;

		default:
			return ntohllUnknownEndian;
		break;
	}
}

}
