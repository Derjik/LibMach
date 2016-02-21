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

#ifndef MAPEDITOR_HPP_INCLUDED
#define MAPEDITOR_HPP_INCLUDED

#include <map>			// std::map
#include <set>			// std::set
#include <thread>		// std::thread
#include <future>		// std::future
#include <SFML/System.hpp>	// SFML core
#include <SFML/Graphics.hpp>	// sf::RenderWindow...
#include "Map.hpp"		// Map

/*
 * Drag flags:
 * Set to NONE when no dragging is being done,
 * set to the corresponding block otherwise.
 */
enum Drag
{
	DRAG_NONE,
	DRAG_START,
	DRAG_STOP
};

/*
 * Draw flags:
 * set to NONE when no drawing is being done,
 * set to the corresponding block otherwise.
 */
enum Draw
{
	DRAW_NONE,
	DRAW_WALKABLE,
	DRAW_UNWALKABLE
};

/*
 * Generic Map Editor:
 * Opens an SFML RenderWindow, shows the given
 * Map (or creates a default new one) and
 * allows the user to change blocks by different
 * means (mostly drag and drop).
 */
class MapEditor
{
	private:
		/* Content */
		Map _map;
		Point _start;
		Point _stop;

		/* Display */
		sf::RenderWindow _window;
		unsigned _tileSize;
		unsigned _delay;

		/* Asynchronous display */
		std::thread _searchThread;
		std::future<std::vector<Point>> _path;

		/* Dragging */
		Drag _drag;

		/* Drawing */
		Draw _draw;
		Point _drawBuffer;

		/* Simulation content */
		std::map<Point const, Direction> _openList;
		std::map<Point const, Direction> _closedList;
		std::set<Point> _result;

	protected:
		/* Drawing macros */
		inline void drawTile(Point const & p);
		void drawMap();
		inline void drawDirection(Point const & p, Direction const & d);

		/* Editor */
		void toggle(Point const & p);
		void clearMap();
		void clearListsForTile(Point const & p);

		/* Handlers */
		inline void handleMousePressed(sf::Event const & e);
		inline void handleMouseReleased(sf::Event const & e);
		inline void handleMouseMoved(sf::Event const & e);
		inline void handleKeyboardEvent(sf::Event const & e);

		/* Internal helper */
		static inline Direction computeDirection(Point const & src, Point const & dst);

		/* Simulator */
		void clearResult();
		void run();

	public:
		/* Constructors & destructor */
		MapEditor(Map m);
		virtual ~MapEditor() {}

		/* Main loop starter */
		void open();

		/* Simulation hooks */
		void addOpenNode(Point const & node, Point const & parent);
		void updateParent(Point const & node, Point const & parent);
		void addClosedNode(Point const & node, Point const & parent);
};

#endif // MAPEDITOR_HPP_INCLUDED
