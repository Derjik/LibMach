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
UDPClient::UDPClient() : _socket(-1)
{}

/*
 * Properly close the socket before deleting the UDPClient instance
 */
UDPClient::~UDPClient()
{
	disconnect();
}

/*
 * Try connecting to the given host:port (Exceptions are thrown on errors)
 */
void UDPClient::connectTo(string const host, unsigned short const port)
{
	int getaddrinfoError(0), connectError(0);
	addrinfo hints, *remote(nullptr);

	if(_socket != -1)
		throw Exception("UDPClient is already connected!");

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	/* Use IPv4 and IPv6 indifferently */
	hints.ai_socktype = SOCK_DGRAM;	/* Use datagram sockets (UDP) */

	/* Get available adresses list */
	getaddrinfoError = getaddrinfo(host.c_str(),
			to_string(port).c_str(), &hints, &remote);

	if(getaddrinfoError)
	{
		freeaddrinfo(remote);

#if defined(_WIN32) || defined(_WIN64)
		throw Exception(gai_strerrorA(getaddrinfoError));
#elif defined(__gnu_linux__)
		throw Exception(gai_strerror(getaddrinfoError));
#endif
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
 * Disconnect from the current server (if any)
 */
void UDPClient::disconnect()
{
	if(_socket != -1)
	{
		closeSocket(_socket);
		_socket = -1;
	}
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
