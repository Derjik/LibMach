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

typedef uint64_t (*htonllImpl) (uint64_t const &);
typedef uint64_t (*ntohllImpl) (uint64_t const &);

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

		/* IP-agnostic sockaddr & port getters */
		static void * specializeAddress(sockaddr const *);
		static unsigned short specializePort(sockaddr const *);

		/* Human-readable IP and port extractors */
		static std::string extractIP(addrinfo const *);
		static unsigned short extractPort(addrinfo const *);

		/* Properly close given socked */
		static void closeSocket(int const);

	public:
		/* Windows Socket API init and cleanup methods */
		static void startWSA();
		static void stopWSA();
		static htonllImpl htonll;
		static ntohllImpl ntohll;
};

enum : uint32_t
{
	LITTLE_ENDIAN = 0x03020100ul,
	BIG_ENDIAN = 0x00010203ul
};

static const union
{
	unsigned char bytes[4];
	uint32_t value;
} HOST_ORDER = {{0, 1, 2, 3}};

uint64_t htonllBigEndian(uint64_t const &);
uint64_t htonllLittleEndian(uint64_t const &);
uint64_t htonllUnknownEndian(uint64_t const &);

uint64_t ntohllBigEndian(uint64_t const &);
uint64_t ntohllLittleEndian(uint64_t const &);
uint64_t ntohllUnknownEndian(uint64_t const &);

htonllImpl setHtonll();
ntohllImpl setNtohll();


}

#endif // NETCOMPONENT_HPP_INCLUDED
