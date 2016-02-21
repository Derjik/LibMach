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

#ifndef ASTARGRAPH_HPP_INCLUDED
#define ASTARGRAPH_HPP_INCLUDED

#include <Mach/AStar.hpp>	// Base AStar template
#include "MapEditor.hpp"	// MapEditor
#include <thread>		// std::this_thread, std::sleep_for
#include <chrono>		// std::chrono


/*
 * Specialized AStar with MapEditor graphical hooks:
 * shows open and closed lists progress in "realtime"
 * while processing the map.
 */
template
<typename Map, typename Coord>
class AStarGraph : public Mach::AStar<Map, Coord>
{
	protected:
		MapEditor & _gui;
		unsigned _delay;

	public:
		AStarGraph
		(
			Map const & m,
			Coord const & src,
			Coord const & dst, 
			unsigned long (*distance) (Coord const &, Coord const &),
			unsigned long (*moveCost) (Coord const &, Coord const &),
			unsigned long (*terrainCost) (Map const & map, Coord const &),
			std::vector<Coord> (*near) (Coord const &),
			MapEditor & gui
		)
		:
			AStar<Map, Coord>(m, src, dst, distance, moveCost, terrainCost, near),
			_gui(gui),
			_delay(0)
		{}

		virtual std::vector<Coord> run();
		void setDelay(unsigned const d)
		{
			_delay = d;
		}
};

/*
 * Modified version of the main running procedure,
 * including calls to the GUI.
 */
template <typename Map, typename Coord>
std::vector<Coord>
AStarGraph<Map, Coord>::
run()
{
	std::vector<Coord> neighbors;

	bool destinationReached(false),
	     inClosedList(false);

	typename AStar<Map, Coord>::Node	*currentNode(nullptr),
						*startingNode(AStar<Map, Coord>::makeNode(AStar<Map, Coord>::_source));


	AStar<Map, Coord>::_openList.insert(std::pair<Coord, typename AStar<Map, Coord>::Node*>(startingNode->_position, startingNode));

	// Iterate until a path is found OR the _openList becomes empty
	while(!AStar<Map, Coord>::_openList.empty() && !destinationReached)
	{
		currentNode = AStar<Map, Coord>::bestOpenNode();

		destinationReached = (currentNode->_position == AStar<Map, Coord>::_destination);

		AStar<Map, Coord>::_openList.erase(currentNode->_position);
		AStar<Map, Coord>::_closedList.insert(std::make_pair(currentNode->_position, currentNode));

		if(currentNode->_parent != nullptr)
			_gui.addClosedNode(currentNode->_position, currentNode->_parent->_position);

		neighbors = (*AStar<Map, Coord>::_near)(currentNode->_position);

		// For each neighbor from the current node
		for(Coord& neighbor : neighbors)
		{
			unsigned long tCost = (*AStar<Map, Coord>::_terrainCost)(AStar<Map, Coord>::_map, neighbor);

			if(tCost == 0)		// If the node isn't walkable,
				continue;	// skip it

			inClosedList = (AStar<Map, Coord>::_closedList.find(neighbor) != AStar<Map, Coord>::_closedList.end());

			if(inClosedList)	// If the node is already in the closed list,
				continue;	// skip it

			/* From now on we can assume the node is valid and traversable */

			auto const & inOpenList = AStar<Map, Coord>::_openList.find(neighbor);

			if(inOpenList != AStar<Map, Coord>::_openList.end())
			{
				if(AStar<Map, Coord>::tryShortcut(currentNode, inOpenList->second))
					_gui.updateParent(inOpenList->first, currentNode->_position);
			}
			else
			{
				AStar<Map, Coord>::_openList.insert(std::pair<Coord, typename AStar<Map, Coord>::Node*>(neighbor, AStar<Map, Coord>::makeNode(neighbor, currentNode)));
				_gui.addOpenNode(neighbor, currentNode->_position);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(_delay));
		}
	}

	if(destinationReached)
		AStar<Map, Coord>::completePath();

	return AStar<Map, Coord>::_path;
}

#endif // ASTARGRAPH_HPP_INCLUDED
