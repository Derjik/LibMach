#ifndef UDPCLIENT_HPP_INCLUDED
#define UDPCLIENT_HPP_INCLUDED

#include "NetComponent.hpp"
#include <string>


namespace Mach
{

/*
 * UDP client
 *
 * This client connects to an UDP server on the given host and port, and is able
 * to send and receive data from it.
 * The API is very straightforward as we're working with UDP, therefore MTU and
 * packet loss issues should be addressed on a higher level.
 * You can basicaly achieve this by inheriting this class and adding some
 * clever custom mechanisms around the send & receive methods.
 */
class UDPClient : public NetComponent
{
	protected:
		/* Remote server connection socket */
		int _socket;

	public:
		/* Constructors & destructor */
		UDPClient();
		virtual ~UDPClient();

		/* Copy & assignation are forbidden */
		UDPClient(UDPClient const &) = delete;
		UDPClient & operator = (UDPClient const &) = delete;

		/* Connect to the given host:port */
		void connectTo(std::string const, unsigned short const);

		/* Disconnect from server */
		void disconnect();

		/* Send some bytes to the remote server */
		void sendBytes(uint8_t const *, size_t const) const;

		/* Receive some bytes from the remote server */
		void receiveBytes(uint8_t *, size_t const) const;
};

}

#endif // UDPCLIENT_HPP_INCLUDED
