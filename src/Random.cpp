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
