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

#include "../include/Mach/UDPClient.hpp"
#include "../include/Mach/Exception.hpp"
#include <iostream>


namespace Mach
{

using namespace std;


/*
 * Construct a base UDPClient
 * host and port describe the remote server information (thus, port shall be
 * != 0 and host should be a VALID FQDN or IP address)
 */
UDPClient::UDPClient(string const host, unsigned short const port) : _socket(-1)
{
	int getaddrinfoError(0), connectError(0);
	addrinfo hints, *remote(nullptr);

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	/* Use IPv4 and IPv6 indifferently */
	hints.ai_socktype = SOCK_DGRAM;	/* Use datagram sockets (UDP) */

	/* Get available adresses list */
	getaddrinfoError = getaddrinfo(host.c_str(),
			to_string(port).c_str(), &hints, &remote);

	if(getaddrinfoError)
	{
		freeaddrinfo(remote);

		throw Exception(gai_strerror(getaddrinfoError));
	}

	/* Try preparing the socket */
	_socket = socket(remote->ai_family, remote->ai_socktype,
			remote->ai_protocol);

	if(_socket == -1)
		throw Exception(lastError("socket"));

	/* Try connecting to the remote server */
	connectError = connect(_socket, remote->ai_addr, remote->ai_addrlen);

	if(connectError)
		throw Exception(lastError("connect"));

	/* Clean da carpet */
	freeaddrinfo(remote);
}

/*
 * Properly close the socket before deleting the UDPClient instance
 */
UDPClient::~UDPClient()
{
	closeSocket(_socket);
}

/*
 * Try sending a message to the remote server using the internal socket
 */
void UDPClient::sendBytes(uint8_t const * bytes, size_t const length) const
{
	if(send(_socket, (char *)(bytes), length, 0) == -1)
		throw Exception(lastError("send"));
}

/*
 * Wait for a message from the remote server on the internal socket
 */
void UDPClient::receiveBytes(uint8_t * bytes, size_t const max) const
{
	if(recv(_socket, (char *)bytes, max, 0) == -1)
		throw Exception(lastError("recv"));
}

}
