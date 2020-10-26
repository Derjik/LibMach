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

#include "DemoUDPClient.hpp"
#include <iostream>
#include <thread>
#include <chrono>


namespace Mach
{

using namespace std;


/*
 * Main test procedure for the client: this one takes input from the keyboard
 * and sends the messages to the server using the main sockets (assuming a
 * server has been connected before)
 */
void DemoUDPClient::chat() const
{
	/* Input message */
	char message[1500];

	/* User input reading loop */
	do
	{
		/* Get the message to send from stdin */
		cin.getline(message, 1500, '\n');

		/* If user input exceeds 1499 characters */
		if(cin.fail() && !cin.eof())
		{
			cerr << "Error: the message is too long." << endl;

			/* Avoid looping infinitely */
			cin.clear();
		}
		/* If we're hitting the EOF character (a.k.a Ctrl + D on
		 * UNIX-like operating systems) */
		else if(!cin.eof())
			/* Send it raw to the server (size+1 to include \0 ) */
			sendBytes((uint8_t*)(message), strlen(message)+1);
	} while(!cin.eof() && !cin.bad()); /* Read stdin until EOF is sent */
}

/*
 * Sends the given byte 'length' times to the remote server (useful to generate
 * some really big datagrams, while testing IP fragmentation for example).
 */
void DemoUDPClient::sendPattern(uint8_t const byte, size_t const length) const
{
	/* Temporary buffer hosting the data to be sent */
	uint8_t * data = new uint8_t[length];

	/* Repeat the template byte on the whole buffer */
	memset(data, byte, length);

	/* Send those bytes through the user-friendly procedure */
	sendBytes(data, length);

	/* Clean da carpet */
	delete [] data;
}

/*
 * Send an indexed stream of packets. Useful to test reception order and detect
 * packet loss, along with the DemoUDPServer's indexed mode.
 */
void DemoUDPClient::sendIndexes(unsigned const number,
		unsigned const msDelay) const
{
	/* Send successive indexes and pause between each one */
	for(unsigned i=0 ; i < number ; ++i)
	{
		sendBytes((uint8_t*)(&i), sizeof(i));
		this_thread::sleep_for(chrono::milliseconds(msDelay));
	}
}

/*
 * Usual constructor, forwarding parameters to the mother class
 */
DemoUDPClient::DemoUDPClient() : UDPClient()
{}

/*
 * Empty destructor
 */
DemoUDPClient::~DemoUDPClient()
{}

}
