#ifndef RANDOM_HPP_INCLUDED
#define RANDOM_HPP_INCLUDED

#include <random>


namespace Mach
{

/*
 * Random Number Generation utility
 *
 * This is a basic class for all random numbers-related tasks: setting the
 * "true" random source up, and generating random integers and real numbers.
 * Warning: as mentioned beneath, Random::clean() must be called somewhere
 * before the end of the program to avoid any memory leak.
 */
class Random
{
	private:
		/* Random Number Generator (used to feed distributions) */
		static std::default_random_engine * _rng;

	public:
		/* Init and clean methods (note: Random::clean() MUST be called at
		 * the end of the program to avoid memory leaks) */
		static void init();
		static void clean();

		/* Roll methods (inclusive) */
		static int integer(int const min, int const max);
		static double real(double const min, double const max);
};

}

#endif // RANDOM_HPP_INCLUDED
