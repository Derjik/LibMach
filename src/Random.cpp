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

#include "../include/Mach/Random.hpp"


namespace Mach
{

using namespace std;


/* Initialize the random engine pointer to null value */
default_random_engine * Random::_rng(nullptr);

/*
 * Initialize the RNG engine
 */
void Random::init()
{
	/* Prevent overwriting an already initialized RNG */
	/* (and consequent memory leak) */
	if(_rng == nullptr)
	{
		/* Get a "genuine random" source */
		random_device rd;
		/* Seed the PRNG with it */
		_rng = new default_random_engine(rd());
	}
}

/*
 * Clean the RNG engine
 */
void Random::clean()
{
	delete _rng;
	_rng = nullptr;
}

/*
 * Get a random int
 */
int Random::integer(int const min, int const max)
{
	/* Auto init */
	if(_rng == nullptr)
		init();

	uniform_int_distribution<int> d(min, max);
	return d(*_rng);
}

/*
 * Get a random float
 */
double Random::real(double const min, double const max)
{
	/* Auto init */
	if(_rng == nullptr)
		init();

	uniform_real_distribution<double> d(min, max);
	return d(*_rng);
}

}
