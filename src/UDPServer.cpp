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

#include "../include/Mach/UDPServer.hpp"
#include "../include/Mach/Exception.hpp"
#include <iostream>


namespace Mach
{

using namespace std;


/*
 * Construct a base UDPServer
 * The logPath and priority are used to setup the internal Logger
 * The port shall be a valid port number (!= 0) to listen on
 */
UDPServer::UDPServer(unsigned short const port,
			string const logPath,
			Priority const prio)
	:
	_listening(false),
	_log(logPath, prio)
{
	int getaddrinfoError(0);
	addrinfo hints, *results(nullptr);

	memset(&hints, 0, sizeof(addrinfo));

	hints.ai_family = AF_UNSPEC;	/* Use IPv4 and IPv6 indifferently */
	hints.ai_flags = AI_PASSIVE;	/* Bind to every available address */
	hints.ai_socktype = SOCK_DGRAM; /* Use datagram sockets (UDP) */

	/* Get available adresses list */
	getaddrinfoError = getaddrinfo(nullptr, to_string(port).c_str(),
			&hints, &results);

	if(getaddrinfoError)
	{
		freeaddrinfo(results);

		throw Exception(gai_strerror(getaddrinfoError));
	}

	/* Try binding to every resulting address info */
	for(	addrinfo* currentAddress = results ;
		currentAddress != nullptr ;
		currentAddress = currentAddress->ai_next)
	{
		try
		{
			bindTo(currentAddress);
		}
		catch(Exception e)
		{
			_log.error
			<< "Failed to bind to "
			<< extractIP(currentAddress) << ":"
			<< extractPort(currentAddress) << "!"
			<< endl;
			_log.error
			<< "Error was: " << e.message() << "."
			<< endl;
		}
	}

	/* Clean da carpet */
	freeaddrinfo(results);
}

/*
 * Delete the UDPServer instance, stopping its listening threads if necessary
 */
UDPServer::~UDPServer()
{
	if(_listening)
		stopListening();
}

/*
 * Bind to an individual addrinfo
 */
void UDPServer::bindTo(addrinfo const * addr)
{
#if defined(_WIN32) || defined(_WIN64)
	char yes(1);
#elif defined(__gnu_linux__)
	int yes(1);
#endif

	/* Will host a copy of the successfully bound socket's addrinfo */
	addrinfo aiCopy = *addr;

	/* Various temporary variables */
	int sock(0), sockoptError(0), bindError(0);
	unsigned short port;
	string ipstr;

	aiCopy.ai_next = nullptr;

	/* Extract human-readable IP address and port */
	ipstr = extractIP(addr);
	port = extractPort(addr);

	_log.info
	<< "Trying to bind to " << ipstr << ":" << port
	<< endl;

	/* Get the socket */
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if(sock == -1)
		throw Exception(lastError("socket"));

	/* Set options to avoid binding errors on restart */
	sockoptError = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int));
	if(sockoptError)
		throw Exception(lastError("setsockopt"));

	/* Let's bind! */
	bindError = ::bind(sock, addr->ai_addr, addr->ai_addrlen);
	if(bindError)
		throw Exception(lastError("bind"));

	/* Register the socket */
	_sockets.push_back(make_pair(sock, aiCopy));

	_log.info
	<< "Bound to " << ipstr << ":" << port
	<< endl;
}

/*
 * Launch the listening threads (one per socket)
 * Note: we can't catch exceptions thrown from a thread (or I don't know how to
 * do this, if you do, please send a mail or poke us on IRC!), so as of now
 * their alone in space and if they die we have to find it out then restart'em.
 * How? Dunno. Later.
 */
void UDPServer::startListening()
{
	if(!_listening)
	{
		/* Create & start the threads */
		for(pair<int, addrinfo> socket : _sockets)
			_threads.push_back(thread(&UDPServer::listener,
						this, socket));

		_listening = true;
	}
	else
		_log.warn
		<< "The server is already listening! "
		<< "Ignoring call to UDPServer::listen()."
		<< endl;
}

/*
 * Stop networking threads by closing their respective listening sockets
 */
void UDPServer::stopListening()
{
	if(_listening)
	{
		_log.info
		<< _sockets.size() << " sockets to close..."
		<< endl;

		for (pair<int, addrinfo>& socket : _sockets)
		{
			_log.info
			<< "Closing socket " << socket.first
			<< endl;

			closeSocket(socket.first);
		}

		for(thread& t : _threads)
			t.join();

		_threads.clear();

		_listening = false;
	}
	else
		_log.warn
		<< "The server isn't listening! "
		<< "Ignoring call to UDPServer::stopListening()."
		<< endl;
}

/*
 * Listen on the given socket until an error is encountered
 */
void UDPServer::listener(pair<int, addrinfo> const socket)
{
	uint8_t bytes[PACKET_SIZE];

	/* Remote end address and port (machine readable form) */
	sockaddr_storage remoteSockaddr;
	socklen_t addrLen(sizeof(remoteSockaddr));

	int length(0);

#if defined(__gnu_linux__)
	/* Rename thread (Linux-only feature, 'coz Windows blows) */
	switch(socket.second.ai_family)
	{
		/* IPv4 */
		case AF_INET:
			prctl(PR_SET_NAME, "IPv4listener\0",0,0,0);
		break;

		/* IPv6 */
		case AF_INET6:
			prctl(PR_SET_NAME, "IPv6listener\0",0,0,0);
		break;

		/* Unknown family */
		default:
		break;
	}
#endif

	/* Main listening loop */
	do
	{
		/* Try receiving some bytes from the socket (UDP, blocking) */
		length = recvfrom(socket.first,
				(char *)(bytes), PACKET_SIZE-1,
				0, (sockaddr *)(&remoteSockaddr),
				&addrLen);

		/* If something has been received, forward it to the internal
		 * datagram processing method */
		if(length > 0)
			receiveBytes(bytes,
				length,
				&remoteSockaddr,
				socket.first);
	} while(length != -1);

	return;	/* End of thread */
}

/*
 * Send given data to remote sockaddr using given socket
 */
void UDPServer::sendBytes(uint8_t const * data, size_t length,
		sockaddr const * remoteSockaddr,
		int const socketFd)
{
	/* Trivial UDP send operation */
	if(sendto(
		socketFd,			/* Source socket */
		(char *)(data),			/* Data to send */
		length,				/* Data length */
		0,				/* Additional flags */
		remoteSockaddr,			/* Recipient info */
		sizeof(sockaddr_storage))	/* Recipient info size */
		== -1)
		/* If an error occurs while sending... */
		throw Exception(lastError("sendto"));
}

}
