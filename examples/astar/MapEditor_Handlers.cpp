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

#include <iostream>

using namespace std;
using namespace sf;


/*
 * Mouse pressed event handler
 */
void MapEditor::handleMousePressed(Event const & e)
{
	int	x = e.mouseButton.x /_tileSize,
		y = e.mouseButton.y /_tileSize;

	switch(e.mouseButton.button)
	{
		case Mouse::Left:
			if(_start == Point(x, y))
			{
				_drag = DRAG_START;
			}
			else if(_stop == Point(x, y))
			{
				_drag = DRAG_STOP;
			}
			else
			{
				switch(_map(Point(x, y)))
				{
					case WALKABLE:
						toggle(Point(x, y));
						_draw = DRAW_UNWALKABLE;
					break;

					case UNWALKABLE:
						toggle(Point(x, y));
						_draw = DRAW_WALKABLE;
					break;

					default:
					break;
				}
			}
		break;

		default:
		break;
	}
}

/*
 * Mouse released event handler
 */
void MapEditor::handleMouseReleased(Event const & e)
{
	switch(e.mouseButton.button)
	{
		case Mouse::Left:
			_drag = DRAG_NONE;
			_draw = DRAW_NONE;
		break;

		default:
		break;
	}
}

/*
 * Mouse moved event handler
 */
void MapEditor::handleMouseMoved(Event const & e)
{
	int	x = e.mouseMove.x /_tileSize,
		y = e.mouseMove.y /_tileSize;

	switch(_drag)
	{
		case DRAG_START:
			if((x != _start.x || y != _start.y) && _stop != Point(x, y) && _map(Point(x, y)) == WALKABLE)
			{
				_start = Point(x, y);

				drawMap();
			}
		break;

		case DRAG_STOP:
			if((x != _stop.x || y != _stop.y) && _start != Point(x, y) && _map(Point(x, y)) == WALKABLE)
			{
				_stop = Point(x, y);

				drawMap();
			}
		break;

		default:
		case DRAG_NONE:
		break;
	}

	switch(_draw)
	{
		case DRAW_WALKABLE:
			if((x != _drawBuffer.x || y != _drawBuffer.y) && _stop != Point(x, y) && _start != Point(x, y) && _map(Point(x, y)) == UNWALKABLE)
			{
				_drawBuffer = Point(x, y);
				_map.set(_drawBuffer, WALKABLE);
				clearListsForTile(_drawBuffer);

				drawMap();
			}
		break;

		case DRAW_UNWALKABLE:
			if((x != _drawBuffer.x || y != _drawBuffer.y) && _stop != Point(x, y) && _start != Point(x, y) && _map(Point(x, y)) == WALKABLE)
			{
				_drawBuffer = Point(x, y);
				_map.set(_drawBuffer, UNWALKABLE);
				clearListsForTile(_drawBuffer);

				drawMap();
			}
		break;

		default:
		case DRAW_NONE:
		break;
	}
}

/*
 * Key pressed event handler
 */
void MapEditor::handleKeyboardEvent(Event const & e)
{
	switch(e.key.code)
	{
		case Keyboard::Q:
			if(!_searchThread.joinable())
				_window.close();
		break;

		case Keyboard::R:
			run();
		break;

		case Keyboard::W:
			clearResult();
		break;

		case Keyboard::C:
			clearMap();
			clearResult();
		break;

		case Keyboard::S:
			try
			{
				_map.saveTo("map.bin");
			}
			catch(SaveFileException e)
			{
				cout << e.what() << endl;
			}
		break;

		case Keyboard::L:
			try
			{
				_map.loadFrom("map.bin");
			}
			catch(SaveFileException e)
			{
				cout << e.what() << endl;
			}
		break;

		case Keyboard::Add:
			if(_delay > 0)
				_delay -= 5;
		break;

		case Keyboard::Subtract:
			if(_delay < 250)
				_delay += 5;
		break;

		default:
		break;
	}
}

/*
 * Create a Window according to the current loaded map's size
 */
void MapEditor::open()
{
	unsigned width(0),
		 height(0);

	VideoMode m = VideoMode::getDesktopMode();

	width = _tileSize * _map.width();
	height = _tileSize * _map.height();

	if(width > m.width)
		width = m.width;
	if(height > m.height)
		height = m.height;

	_window.create(VideoMode(width, height, 32), "A* Map Editor");

	drawMap();

	Clock c;
	Time t = c.restart();

	/* Event loop */
	while(_window.isOpen())
	{
		Event e;

		while(_window.pollEvent(e))
		{
			switch(e.type)
			{
				case Event::Closed:
					if(_searchThread.joinable())
						_searchThread.join();

					_window.close();
				break;

				case Event::LostFocus:
				case Event::GainedFocus:
				case Event::Resized:
					drawMap();
				break;

				case Event::MouseButtonPressed:
					handleMousePressed(e);
				break;

				case Event::MouseButtonReleased:
					handleMouseReleased(e);
				break;

				case Event::MouseMoved:
					handleMouseMoved(e);
				break;

				case Event::KeyPressed:
					handleKeyboardEvent(e);
				break;

				default:
				break;
			}

			if(e.type == Event::Closed)
				_window.close();
		}

		if(_path.valid() && _path.wait_for(chrono::seconds(0)) == future_status::ready)
		{
			for(auto& p : _path.get())
				_result.insert(p);

			if(_searchThread.joinable())
				_searchThread.join();
		}

		drawMap();


		t = c.restart();
		t = milliseconds(16.67) - t;
		sleep(t);
	}
}
