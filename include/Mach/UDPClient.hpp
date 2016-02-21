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
		UDPClient(std::string const, unsigned short const);
		virtual ~UDPClient();

		/* Send some bytes to the remote server */
		void sendBytes(uint8_t const *, size_t const) const;

		/* Receive some bytes from the remote server */
		void receiveBytes(uint8_t *, size_t const) const;
};

}

#endif // UDPCLIENT_HPP_INCLUDED
