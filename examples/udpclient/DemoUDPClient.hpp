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

#ifndef DEMOUDPCLIENT_HPP_INCLUDED
#define DEMOUDPCLIENT_HPP_INCLUDED

#include <Mach/UDPClient.hpp>



namespace Mach
{

/*
 * Demonstration UDP Client
 *
 * This dummy UDP client shows some capabilities of the base class UDPClient,
 * and serves as an usage example.
 */
class DemoUDPClient : public UDPClient
{
	public:
		/* Constructors & destructor */
		DemoUDPClient(std::string const, unsigned short const);
		virtual ~DemoUDPClient();

		/* Begin a chat sequence with the remote server */
		void chat() const;

		/* Send the given byte N times */
		void sendPattern(uint8_t const, size_t const) const;

		/* Send N indexed packets */
		void sendIndexes(unsigned const, unsigned const) const;
};

}

#endif // DEMOUDPCLIENT_HPP_INCLUDED
