#include "../include/Mach/Point.hpp"
#include <sstream>


namespace Mach
{

using namespace std;


/*
 * Point to string conversion used for easy-to-read display
 */
Point::operator string() const
{
	ostringstream s;
	s << "[" << x << " , " << y << "]";

	return s.str();
}

/*
 * Manhattan distance between two Points
 */
unsigned distance(Point const & a, Point const & b)
{
	unsigned d(0);

	d += (b.x - a.x) * (b.x - a.x);
	d += (b.y - a.y) * (b.y - a.y);

	return d;
}

}
