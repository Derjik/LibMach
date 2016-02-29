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

#ifndef ASTAR_HPP_INCLUDED
#define ASTAR_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <vector>


namespace Mach
{

/*
 * Template parameters: <Map type, Coordinates type>
 *
 * Main A* class, exposes a process() method that searches
 * for the best path from given start point to given end.
 * The result is returned as a vector for commodity.
 */
template
<typename Map, typename Coord>
class AStar
{
	public:
		typedef unsigned long (*distanceFunction) (Coord const &, Coord const &);
		typedef unsigned long (*moveCostFunction) (Coord const &, Coord const &);
		typedef unsigned long (*terrainCostFunction) (Map const &, Coord const &);
		typedef std::vector<Coord> (*nearFunction) (Coord const &);

	protected:
		/* Graph node metadata, used while processing the path */
		struct Node
		{
			/* Node's position */
			Coord _position;

			/* Node from which we came */
			Node* _parent;

			/* Indication on the "movement cost" from starting node
			to actual node */
			unsigned long _gCost;

			/* Estimation of the remaining distance from actual node
			to destination */
			unsigned long _hCost;

			/* Sum of G and H costs */
			unsigned long _fCost;

			
			/* Node constructor (useless alone, see
			AStar::makeNode(...) instead) */
			Node
			(	
				Coord position,
				Node* parent,
				unsigned long _gCost,
				unsigned long _hCost
			)
			:
				_position(position),
				_parent(parent),
				_gCost(_gCost),
				_hCost(_hCost),
				_fCost(_gCost + _hCost)
			{}
		};

		/* External environment data */
		Map const & _map;
		Coord const _source;
		Coord const _destination;

		distanceFunction _distance;
		moveCostFunction _moveCost;
		terrainCostFunction _terrainCost;
		nearFunction _near;

		/* Internal processing data */
		std::map<Coord, Node*> _openList;
		std::map<Coord, Node*> _closedList;

		std::vector<Coord> _path;

		/* Internal processing methods */
		Node* makeNode(	Coord position, Node* parent=nullptr);
		inline bool tryShortcut(Node* currentNode, Node* neighbor);
		inline Node* bestOpenNode();
		inline void completePath();

	public:
		/* Constructor & destructor */
		AStar
		(
			Map const & m,
			Coord const & src,
			Coord const & dst, 
			distanceFunction distance,
			moveCostFunction moveCost,
			terrainCostFunction terrainCost,
			nearFunction near
		)
		:
			_map(m),
			_source(src),
			_destination(dst),
			_distance(distance),
			_moveCost(moveCost),
			_terrainCost(terrainCost),
			_near(near)
		{}

		virtual ~AStar()
		{
			for(auto& p : _openList)
				delete p.second;

			for(auto& p : _closedList)
				delete p.second;
		}

		/* Main interface */
		virtual std::vector<Coord> run();
		std::vector<Coord> path()
		{
			return _path;
		}
};

/*
 * Main processing method, directly translated from the
 * known algorithm.
 * Current implementation uses std::map, might be replaced
 * with heaps (binary or Fibonacci ones) later.
 * « Premature optimization is the root of all evil »,
 * said the wise man...
 */
template <typename Map, typename Coord>
std::vector<Coord>
AStar<Map, Coord>::
run()
{
	std::vector<Coord> neighbors;

	bool destinationReached(false),
	     inClosedList(false);

	Node *currentNode(nullptr),
	     *startingNode(makeNode(_source));


	_openList.insert(std::pair<Coord, Node*>(startingNode->_position, startingNode));

	/* Iterate until a path is found OR the _openList becomes empty */
	while(!_openList.empty() && !destinationReached)
	{
		currentNode = bestOpenNode();

		destinationReached = (currentNode->_position == _destination);

		_openList.erase(currentNode->_position);
		_closedList.insert(std::make_pair(currentNode->_position, currentNode));

		neighbors = (*_near)(currentNode->_position);

		/* For each neighbor from the current node */
		for(Coord& neighbor : neighbors)
		{
			unsigned long tCost = (*_terrainCost)(_map, neighbor);

			/* If the node isn't walkable, skip it */
			if(tCost == 0)
				continue;

			inClosedList = (_closedList.find(neighbor) != _closedList.end());

			/* If the node is already in the closed list, skip it */
			if(inClosedList)
				continue;

			/* From now on we can assume the node is valid and traversable */

			auto const & inOpenList = _openList.find(neighbor);

			if(inOpenList != _openList.end())
				tryShortcut(currentNode, inOpenList->second);
			else
				_openList.insert(std::pair<Coord, Node*>(neighbor, makeNode(neighbor, currentNode)));
		}
	}

	if(destinationReached)
		completePath();

	return _path;
}

/*
 * Get the best Node available from the open list
 */
template <typename Map, typename Coord>
typename AStar<Map, Coord>::Node*
AStar<Map, Coord>::
bestOpenNode()
{
	/* Best candidate is chosen (criterion: F cost) */
	auto minElementIterator =
		min_element
		(
			_openList.begin(),
			_openList.end(),
			[](std::pair<Coord const, Node*>& a, std::pair<Coord const, Node*>& b)
			{
				return a.second->_fCost < b.second->_fCost;
			}
		);

	return minElementIterator->second;
}

/*
 * Test a shortcut, if one is found, apply it.
 */
template <typename Map, typename Coord>
bool
AStar<Map, Coord>::
tryShortcut(Node* currentNode, Node* neighbor)
{
	/* Compute the new G cost using the current path */
	unsigned long newGCost = currentNode->_gCost + (*_moveCost)(currentNode->_position, neighbor->_position);

	/* If this new G cost is smaller than the original */
	if(newGCost < neighbor->_gCost)
	{
		neighbor->_parent = currentNode;

		neighbor->_gCost = newGCost;

		neighbor->_fCost = neighbor->_gCost + neighbor->_hCost;

		return true;
	}
	else
		return false;
}

/*
 * Copies the current open list content to the path vector
 * and reverse order for easier use.
 */
template <typename Map, typename Coord>
void
AStar<Map, Coord>::
completePath()
{
	/* Fill the resulting path vector with the corresponding points */
	Node* currentElement = _closedList[_destination];
	_path.push_back(currentElement->_position);

	do
	{
		currentElement = _closedList[currentElement->_parent->_position];
		_path.push_back(currentElement->_position);
	}while(currentElement->_position != _source);

	/* Reverse the vector (makes more sense) */
	reverse(_path.begin(), _path.end());
}


/*
 * Intelligent Node Factory:
 * Since I don't want this code to pollute the main algorithm,
 * I put it in a distinct, "smart" constructor that just gets
 * the work done.
 */
template <typename Map, typename Coord>
typename AStar<Map, Coord>::Node*
AStar<Map, Coord>::
makeNode(Coord position, Node* parentNode)
{
	unsigned long _gCost=0, _hCost=0;

	/* If the new node has a parent node */
	if(parentNode)
	{
		/* Compute G cost from parent's + moving cost */
		_gCost = parentNode->_gCost + (*_moveCost)(parentNode->_position, position);
	}

	/* Compute H cost using given heuristic */
	_hCost = (*_distance)(position, this->_destination);

	return new Node(position, parentNode, _gCost, _hCost);
}

}

#endif // ASTAR_HPP_INCLUDED
