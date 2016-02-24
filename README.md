# LibMach - Lightweight, general purpose C++ library

This is a library project that I'm working on, mainly as a personal training
task but also for later use in other projects of mine. The idea of this code
library came when I found myself rewriting the same pieces of code for many,
many projects (2D points with < operator, logging, exceptions, random numbers)
and with the need for a simple, yet custom UDP sockets wrapper in C++.


## Current features

* UDP multithreaded server (IPv4 + IPv6)
* UDP client
* Generic A\* algorithm (shipped as a class template)
* Logging facility
* Exceptions
* Random numbers generation
* 2D points manipulation


## Compatibility

The code compiles & runs on Linux & Windows platforms (at least those I've
tested, ~= Linux 4.x flavors with g++4.9 & Windows 7 with Visual C++).
No special instructions a needed to build it on your own system (with the
exception of `examples/astar/` which requires [SFML](http://www.sfml-dev.org) in
order to build), any flavor of Visual Studio should do the trick on Windows and
g++ should work well on Linux systems. Finally, `include/AStar.hpp` doesn't need
to be built (`Mach::AStar` is a class template).

I might add some Makefile / VS project files one of these days but I chose to
keep the tree as clean as possible for the moment.


## Documentation

I'm currently planning to add some Doxygen documentation but the class headers,
along with the code comments should be a good starter for anyone wishing to work
with this code.

### AStar GUI

The SFML `Mach::AStar` demonstration GUI has some keyboard & mouse controls
which are explained here:

**Drawing & dragging**

You can drag the starting (green tile) and ending (red tile) points across the
map simply by maintaining a left-click on them and moving around.
You can start drawing some obstacles by left-clicking on an empty point
(colorde in dark-gray), then maintaining the click and moving. The same process
goes for removing obstacles, by clicking on an occupied point this time.

**Setting the speed**
Progress speed can be increased by pressing the <kbd>+</kbd> key and decreased
by pressing <kbd>-</kbd>.

**Running, wiping and cleaning**
<kbd>R</kbd> starts the simulation. Once finished, the open, closed and path
lists can be wiped using the <kbd>W</kbd> key. Finally, obstacles can be cleared
with the <kbd>C</kbd> key.

**Saving & loading**
The current state of the map (obstacles, start & end points) can be saved by
pressing the <kbd>S</kbd> key, resulting in a map.bin file in the execution
path.
You can then load the map later by pressing <kbd>L</kbd>.
