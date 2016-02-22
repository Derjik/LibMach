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

#include "MapEditor.hpp"

#include <fstream>
#include <iostream>
#include <chrono>

#include "AStarGraph.hpp"

using namespace sf;
using namespace std;


/* Main constructor */
MapEditor::MapEditor(Map m) :
_map(m),
_start(0, 0),
_stop(1, 0),
_tileSize(25),
_delay(10),
_drag(DRAG_NONE),
_draw(DRAW_NONE),
_drawBuffer(0, 0)
{}

/* Clear all simulation containers (open, closed & result lists) */
void MapEditor::clearResult()
{
	_result.clear();
	_openList.clear();
	_closedList.clear();

	drawMap();
}

/* Run the simulation with current map state */
void MapEditor::run()
{
	if(!_searchThread.joinable())
	{
		clearResult();

		/* Path-finder initialization */
		AStarGraph<Map, Point> algorithm(_map, _start, _stop, distance, moveCost, terrainCost, near, (*this));
		algorithm.setDelay(_delay);

		/* Task initialization */
		packaged_task < vector<Point> (AStarGraph<Map, Point>) > pathFinder (&AStarGraph<Map, Point>::run);
		_path = pathFinder.get_future();

		/* Thread initialization */
		_searchThread = thread(move(pathFinder), algorithm);
	}
	else
		cout << "Cannot start search: another search thread is still running!" << endl;
}

/* Compute 8-state direction between two points */
Direction MapEditor::computeDirection(Point const & src, Point const & dst)
{
	Direction d(DOWN);

	int deltaX = dst.x - src.x,
	    deltaY = dst.y - src.y;

	if(deltaX == 0)
	{
		if(deltaY > 0)
			d = DOWN;
		else if(deltaY < 0)
			d = UP;
	}
	else if(deltaX > 0)
	{
		if(deltaY > 0)
			d = DOWN_RIGHT;
		else if(deltaY == 0)
			d = RIGHT;
		else /* deltaY < 0 */
			d = UP_RIGHT;
	}
	else /* deltaX < 0 */
	{
		if(deltaY > 0)
			d = DOWN_LEFT;
		else if(deltaY == 0)
			d = LEFT;
		else /* deltaY < 0 */
			d = UP_LEFT;
	}

	return d;
}

/*
 * Add a given node to the graphical open list, using
 * the given parent to compute the displayed arrow
 */
void MapEditor::addOpenNode(Point const & node, Point const & parent)
{
	_openList.insert(make_pair(node, computeDirection(node, parent)));
}

/*
 * Update a Node's parent in the graph
 */
void MapEditor::updateParent(Point const & node, Point const & parent)
{
	_openList[node] = computeDirection(node, parent);
}

/*
 * Switch the given Node from the graphical open list
 * to the closed one.
 */
void MapEditor::addClosedNode(Point const & node, Point const & parent)
{
	_closedList.insert(make_pair(node, computeDirection(node, parent)));
}

/*
 * Clear all walls &stuff from the currently loaded
 * Map.
 */
void MapEditor::clearMap()
{
	for(unsigned i = 0 ; i < _map.height() ; ++i)
		for(unsigned j = 0 ; j < _map.width() ; ++j)
			_map.set(Point(j, i), WALKABLE);
}

/*
 * Toggle the given Node to the next possible state
 */
void MapEditor::toggle(Point const & p)
{
	switch(_map(p))
	{
		case WALKABLE:
			_map.set(p, UNWALKABLE);
			clearListsForTile(p);
		break;

		case UNWALKABLE:
			_map.set(p, WALKABLE);
			clearListsForTile(p);
		break;

		default:
		break;
	}

	drawMap();
}

/*
 * Clear the open and closed lists from display
 * for the given point on map
 */
void MapEditor::clearListsForTile(Point const & p)
{
	auto it1 = _openList.find(p);
	auto it2 = _closedList.find(p);
	auto it3 = _result.find(p);

	if(it1 != _openList.end())
		_openList.erase(it1);

	if(it2 != _closedList.end())
		_closedList.erase(it2);

	if(it3 != _result.end())
		_result.erase(it3);
}

/*
 * Draw the whole map on display
 */
void MapEditor::drawMap()
{
	View v(FloatRect(0, 0, _window.getSize().x, _window.getSize().y));

	_window.setView(v);
	_window.clear(Color::Black);

	for(unsigned i = 0 ; i < _map.height() ; ++i)
	{
		for(unsigned j = 0 ; j <_map.width() ; ++j)
		{
			drawTile(Point(j , i));
		}
	}

	_window.display();
}

/*
 * Draw the given direction arrow on the given tile
 */
void MapEditor::drawDirection(Point const & p, Direction const & d)
{
	Color c(96, 64, 0);

	RectangleShape dot(Vector2f(_tileSize/4, _tileSize/4));
	dot.setFillColor(c);
	dot.setPosition(p.x * _tileSize + _tileSize / 4 + _tileSize / 8, p.y * _tileSize + _tileSize / 4 + _tileSize / 8);

	Vertex arrow[2];
	arrow[0] = Vertex(Vector2f(p.x * _tileSize + _tileSize / 2, p.y * _tileSize + _tileSize / 2));

	switch(d)
	{
		case DOWN:
			arrow[1] = Vertex(Vector2f(p.x * _tileSize + _tileSize / 2, (p.y + 1) * _tileSize));
		break;

		case UP:
			arrow[1] = Vertex(Vector2f(p.x * _tileSize + _tileSize / 2, p.y * _tileSize));
		break;

		case LEFT:
			arrow[1] = Vertex(Vector2f(p.x * _tileSize, p.y * _tileSize + _tileSize / 2));
		break;

		case RIGHT:
			arrow[1] = Vertex(Vector2f((p.x + 1) * _tileSize, p.y * _tileSize + _tileSize / 2));
		break;

		case UP_RIGHT:
			arrow[1] = Vertex(Vector2f((p.x + 1) * _tileSize, p.y * _tileSize));
		break;

		case UP_LEFT:
			arrow[1] = Vertex(Vector2f(p.x * _tileSize, p.y * _tileSize));
		break;

		case DOWN_RIGHT:
			arrow[1] = Vertex(Vector2f((p.x + 1) * _tileSize, (p.y + 1) * _tileSize));
		break;

		case DOWN_LEFT:
			arrow[1] = Vertex(Vector2f(p.x * _tileSize, (p.y + 1) * _tileSize));
		break;
	}

	arrow[0].color = arrow[1].color = c;

	_window.draw(dot);
	_window.draw(arrow, 2, Lines);
}

/*
 * Draw the given tile on display using the corresponding
 * map data
 */
void MapEditor::drawTile(Point const & p)
{
	Color c;
	bool inLists(false);
	Direction d(UP);

	if(_start == p)
		c = Color(0, 128, 0);
	else if(_stop == p)
		c = Color(128, 0, 0);
	else if(_result.find(p) != _result.end())
	{
		c = Color(0, 64, 128);
		d = _closedList[p];
		inLists = true;
	}
	else if(_closedList.find(p) != _closedList.end())
	{
		c = Color(0, 32, 96);
		d = _closedList[p];
		inLists = true;
	}
	else if(_openList.find(p) != _openList.end())
	{
		c = Color(16, 16, 64);
		d = _openList[p];
		inLists = true;
	}
	else
		switch(_map(p))
		{
			case WALKABLE:
				c = Color(16, 16, 16);
			break;

			case UNWALKABLE:
				c = Color(64, 64, 64);
			break;

			default:
				c = Color::Black;
			break;
		}

	RectangleShape r(Vector2f(_tileSize,_tileSize));

	r.setFillColor(c);
	r.setOutlineThickness(1);
	r.setOutlineColor(Color::Black);
	r.setPosition(_tileSize * p.x,_tileSize * p.y);

	_window.draw(r);
	if(inLists)
		drawDirection(p, d);
}
