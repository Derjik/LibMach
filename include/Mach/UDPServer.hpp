#ifndef UDPSERVER_HPP_INCLUDED
#define UDPSERVER_HPP_INCLUDED

#include "NetComponent.hpp"
#include "Logger.hpp"
#include <string>
#include <vector>
#include <utility>
#include <thread>

/* Arbitrary datagram max buffer size */
#define PACKET_SIZE 65535


namespace Mach
{

/*
 * UDP server
 *
 * This server is able to listen on both IPv4 and IPv6 addresses and receives
 * datagrams using separate listening threads (on per socket).
 * Users of this class shall inherit from it and implement the
 * receiveBytes(...) method in order to instantiate a concrete server.
 * It uses a threaded model, so any implementation of UDPServer::receiveBytes
 * should be thought in a thread-safe way: using std::mutex for queuing
 * incoming messages into a common data structure might be a good idea,
 * for example.
 */
class UDPServer : public NetComponent
{
	private:
		/* Is the server currently listening ? */
		bool _listening;

		/* Socket vector <file descriptor , address information> */
		std::vector< std::pair< int, addrinfo > > _sockets;

		/* Currently running listener threads */
		std::vector< std::thread > _threads;

		/* Error & misc information logger */
		Logger _log;

	protected:
		/* Try binding to the given addrinfo (or any if nullptr is given) */
		void bindTo(addrinfo const *);

		/* Listen on the given socket as long as it is valid */
		void listener(std::pair< int, addrinfo >);

		/* Send some data to destination using the given socket */
		void sendBytes(uint8_t const *, size_t, sockaddr const *,
				int const);

		/* Handle an incoming datagram */
		virtual void receiveBytes(uint8_t const *, size_t const,
				sockaddr_storage const *, int const) = 0;

	public:
		/* Constructor & destructor */
		UDPServer(unsigned short const port,
				std::string const logPath = "UDPServer.log",
				Priority const prio = LOG_ERROR);
		virtual ~UDPServer();

		/* Copy & assignation are forbidden */
		UDPServer(UDPServer const &) = delete;
		UDPServer & operator = (UDPServer const &) = delete;

		/* Start listening on all bound sockets */
		void startListening();
		
		/* Close bound sockets & stop listening */
		void stopListening();
};

}

#endif // UDPSERVER_HPP_INCLUDED
