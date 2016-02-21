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

#ifndef MAP_HPP_INCLUDED
#define MAP_HPP_INCLUDED

#include <array>	// std::array
#include <vector>	// std::vector
#include <string>	// std::string
#include <exception>	// std::exception
#include <Mach/Point.hpp>	// Point


using Mach::Point;

/*
 * 2D Direction:
 * Used to show parenting relation while
 * displaying open and closed list members
 */
enum Direction
{
	UP_LEFT,
	UP,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT
};

/*
 * Represents a possible state for
 * one tile on a 2D Map
 */
enum Tile
{
	WALKABLE,
	UNWALKABLE,
	NONEXISTENT
};

/*
 * Basic exceptions thrown by the Map::loadFrom(std::string)
 * method.
 */
class SaveFileException : public std::exception
{
	private:
		std::string _msg;

	public:
		virtual const char * what()
		{
			return _msg.c_str();
		}

		SaveFileException(std::string s) : _msg(s)
		{}
};

/*
 * Simple 2D Map used to test pathfinders.
 */
class Map
{
	private:
		/* Map dimensions */
		unsigned _width;
		unsigned _height;

		/* Map content */
		Tile **_matrix;

	public:
		/* Constructors */
		Map(unsigned const w, unsigned const h);
		Map(Map const &);

		/* Destructor */
		virtual ~Map();

		/* Getters */
		unsigned width() const { return _width; }
		unsigned height() const { return _height; }

		Tile operator () (Point const &) const;

		/* Setters */
		void set(Point const & p, Tile const & c);

		/* File persistence */
		void saveTo(std::string path);
		void loadFrom(std::string path);
};

/*
 * Classic distance, moveCost &cie implementations.
 * Made for use with A*.
 */
unsigned long distance(Point const & start, Point const & end);
unsigned long moveCost(Point const & start, Point const & end);
unsigned long terrainCost(Map const &, Point const &);
std::vector<Point> near(Point const & p);

#endif // MAP_HPP_INCLUDED
