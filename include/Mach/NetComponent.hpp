#ifndef NETCOMPONENT_HPP_INCLUDED
#define NETCOMPONENT_HPP_INCLUDED

#include <string>

/* Common network-related headers */
#include <sys/types.h>
#include <string.h>

/* Linux-specific bits */
#if defined(__gnu_linux__)
#include <sys/prctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

/* Windows-specific bits */
#if defined(_WIN32) || defined(_WIN64)
#pragma comment(lib,  "ws2_32.lib")
#ifndef WINVER
#define WINVER 0x0600
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06000000
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Time.h>
#include <cinttypes>
#endif

#include <limits.h>
#include <stdint.h>


namespace Mach
{

/*
 * Network component
 *
 * Base class for network-related components (mainly servers and clients),
 * featuring the core network toolkit.
 * This code is supposedly cross-platform, tested on Linux 4.x.x flavours and
 * Windows 7 as of today.
 */
class NetComponent
{
	private:
		/* Indicates whether Windows Socket API has been loaded */
		static bool _wsaEnabled;

	protected:
		/* Error display wrapper */
		static std::string lastError(std::string const);

		/* Properly close given socked */
		static void closeSocket(int const);

	public:
		/* Windows Socket API init and cleanup methods */
		static void startWSA();
		static void stopWSA();
};

/* IP-agnostic sockaddr & port getters */
void * specializeAddress(sockaddr const *);
unsigned short specializePort(sockaddr const *);

/* Human-readable IP and port extractors */
std::string extractIP(addrinfo const *);
unsigned short extractPort(addrinfo const *);

/* Endianness implementation function type */
typedef uint64_t (*endianness) (uint64_t const &);

/* Custom (and runtime defined) host to network byte-order translators */
extern endianness htonll;
extern endianness ntohll;

}

#endif // NETCOMPONENT_HPP_INCLUDED
