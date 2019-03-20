#pragma once
#include <vectormath/vectormath.hpp>
#include <vector>

namespace iga
{
	namespace tt
	{
		typedef std::vector<Vector2> Polygon;
		typedef std::vector<uint32> Indices;

		class Triangulation
		{
		public:
			/**
			* Constructor.
			*/
			Triangulation();
			/**
			* Deconstructor.
			*/
			~Triangulation();

			/**
			* Triangulates the given vertex soup and returns and index list of it.
			* @param a_polygon The list if vertices it's going to triangulate.
			* @return Returns a list of indices for drawing order.
			*/
			Indices Triangulate(Polygon a_polygon);

		private:
			/**
			* Returns a sign of the cross product of the given vertices.
			* @param a_p1 Vertex 1.
			* @param a_p2 Vertex 2.
			* @param a_p3 Vertex 3.
			* @return Returns sign value of this cross product.
			*/
			float sign(Vector2 *a_p1, Vector2 *a_p2, Vector2 *a_p3) const;
			/**
			* Returns if the 2nd point of the given triangle is convex or not, in CW order.
			* @param a_triangle The triangle it will check.
			* @return Returns true if the 2nd point of the triangle is convex.
			*/
			bool IsInteriorVertex(Vector2 *a_triangle) const;
			/**
			* Returns true if given point is inside the given triangle.
			* @param a_triangle The triangle the point will check against.
			* @param a_point The point it will check within the triangle.
			* @return Returns true if given point is inside triangle.
			*/
			bool TriangleContains(Vector2 *a_triangle, Vector2 *a_point) const;
		};
	} // End of namespace ~ tt
} // End of namespace ~ iga

