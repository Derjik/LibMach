#ifndef POINT_HPP_INCLUDED
#define POINT_HPP_INCLUDED

/*
 * 2D Point:
 * Represents a discrete 2D point, used
 * by most classic matrix.
 */
class Point
{
	public:
		/* Coordinates */
		int x;
		int y;

		/* Constructors */
		Point(int x, int y) : x(x), y(y) { }

		/* Operators */
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
			return (p.x != x || p.y != y);
		}
};

#endif // POINT_HPP_INCLUDED
