#include <TerrainTool\Triangulation.h>
#include <algorithm>
#include <iostream>
#include <defines.h>

namespace iga
{
	namespace tt
	{
		Triangulation::Triangulation()
		{
		}


		Triangulation::~Triangulation()
		{
		}

		Indices Triangulation::Triangulate(Polygon a_polygon)
		{
			Indices indices;

			//Concave shapes
			size_t p, c, n;
			std::vector<size_t> ignore;
			Vector2 tri[3];
			size_t polycount = a_polygon.size();
			size_t index = 1;
			uint32 loops = 0;

			while ((polycount - ignore.size()) > 2)
			{
			hell:
				if (loops > 2u) {
					// Cannot make shape, stop.
					break;
				}
				//check if index is within range and not in ignore list
				if (index >= polycount)
				{
					if (ignore.size() == 0) { break; }
					++loops;
					index = 0;
				}
				while (std::find(ignore.begin(), ignore.end(), index) != ignore.end())
				{
					++index;
					if (index >= polycount) { index = 0; ++loops; }
				}
				c = index;

				//check if previous index is within range and not in ignore list
				p = index - 1;
				if (p > 0u-1) { p = polycount - 1; }
				while (std::find(ignore.begin(), ignore.end(), p) != ignore.end())
				{
					--p;
					if (p > 0u-1) { p = polycount - 1; }
				}

				//check if next index is within range and not in ignore list
				n = index + 1;
				if (n >= polycount) { n = 0; }
				while (std::find(ignore.begin(), ignore.end(), n) != ignore.end())
				{
					++n;
					if (n >= polycount) { n = 0; }
				}

				//cannot make a triangle with only 2 points or less
				if (p == c || p == n || c == n) {
					continue;
				}

				//get vertices from indices
				tri[0] = a_polygon[p];
				tri[1] = a_polygon[c];
				tri[2] = a_polygon[n];

				//Check if vertex is concave
				if (!IsInteriorVertex(tri)) {
					++index;
					continue;
				}

				//Check if triangle contains any of the other vertices
				for (size_t i = 0; i < polycount; ++i)
				{
					if (i != p && i != c && i != n)
					{
						if (TriangleContains(tri, &a_polygon[i])) {
							++index;
							goto hell;
						}
					}
				}

				//Add triangle to list
				indices.push_back(uint32(p));
				indices.push_back( uint32( c));
				indices.push_back( uint32( n));
				ignore.push_back(index);

				++index;
			}
			return indices;
		}

		bool Triangulation::IsInteriorVertex(Vector2 *a_triangle) const
		{
			Vector2 point_a = a_triangle[0] - a_triangle[1];
			Vector2 point_b = a_triangle[2] - a_triangle[1];
			Vector2 point_n = Vector2(point_a.getY(), -point_a.getX());
			float mag = length(point_n)*length(point_b);
			float ang = acos(dot(point_n, point_b) / mag);
			ang *= 180.f / float(M_PI);
			return (ang > 90.f);
		}

		float Triangulation::sign(Vector2 *a_p1, Vector2 *a_p2, Vector2 *a_p3) const
		{
			return (a_p1->getX() - a_p3->getX())*(a_p2->getY() - a_p3->getY()) - (a_p2->getX() - a_p3->getX())*(a_p1->getY() - a_p3->getY());
		}

		bool Triangulation::TriangleContains(Vector2 *a_triangle, Vector2 *a_point) const
		{
			bool b1, b2, b3;
			b1 = sign(a_point, &a_triangle[0], &a_triangle[1]) < 0.f;
			b2 = sign(a_point, &a_triangle[1], &a_triangle[2]) < 0.f;
			b3 = sign(a_point, &a_triangle[2], &a_triangle[0]) < 0.f;

			return ((b1 == b2) && (b2 == b3));
		}
	}
}
