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

#include <iostream>
#include <string>
#include <Mach/Exception.hpp>
#include "DemoUDPClient.hpp"

using namespace std;
using namespace Mach;


int main(int argc, char* argv[])
{
	UDPClient::startWSA();

	/* Our demonstration client */
	DemoUDPClient client;

	/* Check command-line arguments */
	if(argc != 4 && argc != 3 && argc != 5)
	{
		cerr
		<< "Usage: " << endl
		<< "./udpclient <IP> <port>" << endl
		<< "./udpclient <IP> <port> <amount>" << endl
		<< "./udpclient <IP> <port> <0xbyte> <number>" << endl;

		exit(-1);
	}

	try
	{
		/* Try binding the client to the remote server using the hints
		 * given from command-line */
		client.connectTo(argv[1], stoi(argv[2]));

		/* Act depending on the number of arguments:
		 * 3 arguments means "let's chat with server <IP>:<port>",
		 * while 5 arguments means "let's send <number> bytes set to
		 * <0xbyte> to server <IP>:<port>" */
		if(argc == 3)
			client.chat();
		else if(argc == 4)
			client.sendIndexes(stoi(argv[3]), 10);
		else if(argc == 5)
			client.sendPattern(stoi(argv[3], nullptr, 16),
					stoi(argv[4]));

		/* Disconnect from server (=close socket) */
		client.disconnect();
	}
	catch(Exception * e)
	{
		cerr << e->message() << endl;
		exit(-1);
	}

	UDPClient::stopWSA();

	return 0;
}
