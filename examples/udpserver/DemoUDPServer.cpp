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

#include "DemoUDPServer.hpp"
#include <iostream>
#include <iomanip>


namespace Mach
{

using namespace std;
using std::endl;


void DemoUDPServer::receiveBytes(uint8_t const * data, size_t const dataLen,
		sockaddr_storage const * sender, int const socketFd)
{
	/* -Wall warning removal */
	(void)(socketFd);

	/* Local index for packet arrival order tests */
	unsigned index(0);

	/* Remote end address (human readable form) */
	char remoteAddressString[INET6_ADDRSTRLEN];

	/* Remote end address and port (machine readable) */
	void *specializedAddress(nullptr);
	unsigned short port;

	/* Get the sender's network identity */
	specializedAddress = specializeAddress(
			(sockaddr *)(sender));

	port = specializePort((sockaddr *)(sender));

	/* IPv4 or IPv6 (others are discarded) */
	if(specializedAddress)
	{
		/* Network-to-presentation */
		inet_ntop(sender->ss_family,
			specializedAddress,
			remoteAddressString,
			sizeof(remoteAddressString));

		switch(_mode)
		{
			/* Hexadecimal packet view */
			case HEX:
				cout
				<< remoteAddressString << ":" << port << " ";

				showHex(data, dataLen);
				cout << endl;
			break;

			/* Human-readable packet view */
			case CHARACTER:
				cout
				<< remoteAddressString << ":" << port << " ";

				showChar(data, dataLen);
				cout << endl;
			break;

			/* Only display packet's length */
			case LENGTH:
				cout
				<< remoteAddressString << ":" << port << " ";

				cout
				<< "RX: " << dataLen << "B."
				<< endl;
			break;

			/* Arrival order test mode */
			case INDEX:
				index = *((unsigned*)(data));

				if(index == 0)
					cout
					<< remoteAddressString << ":"
					<< port << " ";

				cout << '.';
				cout.flush();

				if(index != 0 && index != _storedIndex + 1)
				{
					++_errors;
					cout << 'E';
				}

				_storedIndex = index;
			break;

			/* Display hex, char and length */
			case ALL:
				cout
				<< remoteAddressString << ":" << port << " ";

				showChar(data, dataLen);
				cout << " ";
				showHex(data, dataLen);
				cout
				<< " RX: " << dataLen << "B."
				<< endl;
			break;

			/* Some lazy test-writer forgot to implement the new
			 * test mode :-° */
			default:
				cout << "? (blame the lazy programmer!)";
			break;
		}
	}

	/* Update the received packet counter */
	++_packets;
}

/*
 * Print the packet's content as a common character string. Useful when using
 * the demonstration client in "chat mode".
 */
void DemoUDPServer::showChar(uint8_t const * data, size_t const length) const
{
	/* Tmp buffer for local modifications */
	uint8_t * formattedData = new uint8_t[length+1];
	memcpy(formattedData, data, length);

	formattedData[length] = '\0';

	cout << "[" << (char *)(formattedData) << "]";

	delete [] formattedData;
}

/*
 * Print the packet's content as an hexadecimal values vector. Useful for debug
 * purposes.
 */
void DemoUDPServer::showHex(uint8_t const * data, size_t const length) const
{
	cout << "<" << hex;

	for(unsigned i = 0 ; i < length ; ++i)
	{
		cout
		<< setw(2) << setfill('0') << (int)(data[i]) << ";";
	}

	cout << dec << ">";
}

/*
 * Set the demonstration mode
 */
void DemoUDPServer::setMode(DemoMode const mode)
{
	_mode = mode;
}

/*
 * Construct a demonstration UDPServer using the given logfile, minimum
 * priority, listening port number and demonstration mode
 */
DemoUDPServer::DemoUDPServer(string const file, Priority const priority,
		unsigned short const portNumber, DemoMode const mode) :
	UDPServer(portNumber, file, priority),
	_mode(mode),
	_storedIndex(0),
	_packets(0),
	_errors(0)
{}

/*
 * Empty destructor
 */
DemoUDPServer::~DemoUDPServer()
{}

}
