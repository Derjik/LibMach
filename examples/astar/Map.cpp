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

#include "Map.hpp"
#include <fstream>

using namespace std;


/*** Map members ***/

/* Main constructor */
Map::Map(unsigned const w, unsigned const h) : _width(w), _height(h)
{
	_matrix = new Tile* [_height];

	for(unsigned i = 0 ; i < _height ; ++i)
	{
		_matrix[i] = new Tile [_width];

		for(unsigned j = 0 ; j < _height ; ++j)
		{
			_matrix[i][j] = WALKABLE;
		}
	}
}

/* Copy constructor */
Map::Map(Map const & m) : _width(m._width), _height(m._height)
{
	_matrix = new Tile* [_height];

	for(unsigned i = 0 ; i < _height ; ++i)
	{
		_matrix[i] = new Tile [_width];

		for(unsigned j = 0 ; j < _height ; ++j)
		{
			_matrix[i][j] = m._matrix[i][j];
		}
	}
}

/* Destructor */
Map::~Map()
{
	for(unsigned i = 0 ; i < _height ; ++i)
	{
		delete [] _matrix[i];
	}

	delete [] _matrix;
}

/* Sets given point to given state */
void Map::set(Point const & p, Tile const & c)
{
	if((*this)(p) == NONEXISTENT)
		return;
	else
		_matrix[p.y][p.x] = c;
}

/* Gets given point's state */
Tile Map::operator () (Point const & p) const
{
	if(p.x < 0 || p.y < 0 || p.x >= int(_width) || p.y >= int(_height))
		return NONEXISTENT;
	else
		return _matrix[p.y][p.x];
}


/*** Non-member functions ***/

/* Gets given point's terrain cost on given Map */
unsigned long terrainCost(Map const & m, Point const & p)
{
	switch(m(p))
	{
		case WALKABLE:
			return 1;

		default:
		case UNWALKABLE:
		case NONEXISTENT:
			return 0;
	}

	return 0;
}

/* Manhattan 2D distance */
unsigned long distance(Point const & start, Point const & end)
{
	int xDist = (end.x - start.x)*10;
	int yDist = (end.y - start.y)*10;

	return (xDist>=0? xDist : -xDist) + (yDist>=0? yDist : -yDist);
}

/* 2D move cost */
unsigned long moveCost(Point const & start, Point const & end)
{
	if(end.x != start.x && end.y != start.y)
		return 14; /* cheap sqrt(2) */
	else
		return 10;
}

/* 2D neighborhood solver */
vector<Point> near(Point const & p)
{
	vector<Point> v;

	for(int i = p.y - 1 ; i <= p.y + 1 ; ++i )
	{
		for(int j = p.x - 1 ; j <= p.x +1 ; ++j)
		{
			if(i == p.y && j == p.x)
				continue;
			//if(i != p.y && j != p.x)
				//continue;
			/* Diagonal movements permitted */

			v.push_back(Point(j, i));
		}
	}

	return v;
}

/* Save the current Map's content on disk */
void Map::saveTo(string path)
{
	ofstream file(path, ios_base::binary | ios_base::trunc);

	if(file.is_open())
	{
		file.write((char*)(&_width), sizeof(unsigned));
		file.write((char*)(&_height), sizeof(unsigned));

		for(unsigned i = 0 ; i < _height ; ++i)
			for(unsigned j = 0 ; j < _width ; ++j)
				file.write((char*)(&_matrix[i][j]), sizeof(Tile));

		file.close();
	}
	else
		throw SaveFileException("Couldn't save data to " + path + ": failed to open file.");
}

/* Load content from a disk file */
void Map::loadFrom(string path)
{
	unsigned w(0),
		 h(0),
		 actualLength(0),
		 desiredLength(0);

	ifstream file(path, ios_base::binary);

	if(file.is_open())
	{
		/* Read header and compute payload size */
		file.read((char*)(&w), sizeof(unsigned));
		file.read((char*)(&h), sizeof(unsigned));
		desiredLength = w * h * sizeof(Tile);

		/* Payload size check */
		file.seekg(0, file.end);
		actualLength = unsigned(file.tellg()) - 2 * sizeof(unsigned);
		if(actualLength != desiredLength)
		{
			file.close();
			throw SaveFileException("Couldn't load data from " + path + ": incorrect header or corrupted data.");
		}

		/* Back to payload beginning */
		file.seekg(2 * sizeof(unsigned), file.beg);

		if(w != _width || h != _height)
		{
			for(unsigned i = 0 ; i < _height ; ++i)
				delete _matrix[i];

			delete _matrix;

			_matrix = nullptr;
		}

		if(_matrix == nullptr)
		{
			_width = w;
			_height = h;

			_matrix = new Tile* [_height];

			for(unsigned i = 0 ; i < _height ; ++i)
				_matrix[i] = new Tile [_width];
		}

		for(unsigned i = 0 ; i < _height ; ++i)
			for(unsigned j = 0 ; j < _width ; ++j)
				file.read((char*)(&_matrix[i][j]), sizeof(Tile));

		file.close();
	}
	else
		throw SaveFileException("Couldn't open " + path + ": file not found.");
}
