/*
 * Copyright (c) 2016 Julien "Derjik" Laurent
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

#ifndef DEMOUDPSERVER_HPP_INCLUDED
#define DEMOUDPSERVER_HPP_INCLUDED

#include <Mach/UDPServer.hpp>


namespace Mach
{

/*
 * As various demonstration modes are available, the user can use this enum
 * to choose one with DemoUDPServer::setMode()
 */
enum DemoMode
{
	HEX,
	CHARACTER,
	LENGTH,
	INDEX,
	ALL
};

/*
 * Demonstration UDP Server
 *
 * This server demonstrates the capabilities of the UDPServer class, serving as
 * an usage example.
 */
class DemoUDPServer : public UDPServer
{
	private:
		/* Current demonstration mode */
		DemoMode _mode;

		/* Last received packet index */
		unsigned _storedIndex;

		/* Packets received (total) */
		unsigned _packets;

		/* Detected inconsistencies */
		unsigned _errors;

	protected:
		/* Displays information about the received datagram */
		virtual void receiveBytes(uint8_t const *, size_t const,
				sockaddr_storage const *, int const);

		/* Show packet's content as character string */
		void showChar(uint8_t const *, size_t const) const;

		/* Show packet's content as raw hex data */
		void showHex(uint8_t const *, size_t const) const;

	public:
		/* Constructors & destructor */
		DemoUDPServer(std::string const, Priority const,
				unsigned short const, DemoMode const);
		virtual ~DemoUDPServer();

		/* Change the current handling mode */
		void setMode(DemoMode const mode);

		/* Various getters */
		unsigned packets() const
		{
			return _packets;
		}
		unsigned errors() const
		{
			return _errors;
		}
};

}

#endif // DEMOUDPSERVER_HPP_INCLUDED
