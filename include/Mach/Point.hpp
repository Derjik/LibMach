#ifndef POINT_HPP_INCLUDED
#define POINT_HPP_INCLUDED

#include <string>
#include <ostream>


namespace Mach
{

/*
 * Basic 2D Point definition
 *
 * This is a very basic implementation for the classical 2D cartesian point (or
 * vector), fitted with some useful operators like sum, difference and
 * comparison.
 * < operator makes it usable as an ordered container's key.
 */

class Point
{
	public:
		/* Coordinates */
		int x;
		int y;

		/* Constructors & destructor */
		explicit Point(int const x = 0, int const y = 0) : x(x), y(y)
		{}

		virtual ~Point()
		{}

		/* Relational operators */
		bool operator < (Point const & p) const
		{
			return y < p.y || (y == p.y && x < p.x);
		}
		bool operator == (Point const & p) const
		{
			return ((x==p.x)&&(y==p.y));
		}
		bool operator != (Point const & p) const
		{
			return !((*this) == p);
		}

		/* Arithmetic operators */
		Point operator + (Point const & p) const
		{
			return Point(x + p.x , y + p.y);
		}
		Point operator - (Point const & p) const
		{
			return Point(x - p.x , y - p.y);
		}

		/* Point-to-string conversion */
		operator std::string () const;
};

/*
 * Operator << overload for Point, usable with any "string capable" output
 * stream.
 * Note: this is necessary for most common output stream classes, as
 * their << operator is usually a template and user-defined conversions
 * (here, std::string) are not considered by the compiler while deducting
 * template arguments.
 */
template <typename T>
T& operator << (T& output, Point const & p)
{
	output << std::string(p);
	return output;
}

/* Manhattan distance between two Points */
unsigned distance(Point const & a, Point const & b);

}

#endif // POINT_HPP_INCLUDED
