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
#include <Mach/Exception.hpp>
#include "DemoUDPServer.hpp"

using namespace std;
using namespace Mach;


int main(void)
{
        UDPServer::startWSA();

	/* Menu choice */
	int choice(0);
	/* Our test server */
	DemoUDPServer * serv(nullptr);

	try
	{
		/* Try building a demo server with some basic parameters */
		serv = new DemoUDPServer("log/server.log", LOG_DEBUG, 1777, ALL);
	}
	catch(Exception * e)
	{
		cerr << e->message() << endl;
		exit(-1);
	}

	/* Start listening */
	serv->startListening();

	do
	{
		cout
		<< "Select handling mode:" << endl
		<< "1. Display packet's content as char string" << endl
		<< "2. Display packet's content as rax hex values" << endl
		<< "3. Display packet's length" << endl
		<< "4. Display all information above" << endl
		<< "5. Index mode" << endl
		<< "0. Exit" << endl;
		cin >> choice;

		switch(choice)
		{
			case 0:
			break;

			case 1:
				serv->setMode(CHARACTER);
			break;

			case 2:
				serv->setMode(HEX);
			break;

			case 3:
				serv->setMode(LENGTH);
			break;

			case 4:
				serv->setMode(ALL);
			break;

			case 5:
				serv->setMode(INDEX);
			break;

			default:
				cout << "Error: no such mode." << endl;
			break;
		}
	} while (choice != 0);

	cout
	<< "RX: " << serv->packets() << " p ; E: " << serv->errors()
	<< endl;

	delete serv;

        UDPServer::stopWSA();

	return 0;
}
