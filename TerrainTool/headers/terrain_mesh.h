#pragma once
#include <vectormath/vectormath.hpp>
#include <vector>
/**
* @file terrain_mesh.h
* @author Geert Cocu
* @date 2 mar 2018
* @brief TerrainMesh class header.
*
* The TerrainMesh is used to generate the visible mesh of the terrain with given materials.
*/
namespace iga
{
	class Mesh;
	namespace tt
	{
		/**
		*   Enum of the edge type
		*/
		enum class EdgeType
		{
			FILL = -1,
			TOP,
			SIDE,
			BOTTOM
		};
		/**
		*   Struct that holds the needed data for edges
		*/
		struct EdgeData
		{
			Vector3 pos;
			EdgeType type;
			Mesh* mesh;
			float angle;
		};
		class Triangulation;
		class TerrainMesh
		{
		public:
			/**
			* Constructor.
			*/
			TerrainMesh(Vector3 a_world_pos);
			/**
			* Deconstructor.
			*/
			~TerrainMesh();

			/**
			* Generates the visual mesh for the terrain.
			* @param a_closed Bool to generate a closd or open mesh.
			* @param a_nodes Vector of nodes it will use to generate the mesh.
			* @returns Returns true if the generation is successful.
			*/
			bool GenerateMesh(const bool a_closed, const std::shared_ptr<std::vector<Vector3>> a_nodes);
			/**
			* Returns the mesh used for drawing.
			* @return Returns the mesh used for drawing.
			*/
			Mesh *GetMesh();
			/**
			* Returns the edge meshes used for drawing.
			* @return Returns the edge meshes used for drawing.
			*/
			std::shared_ptr<std::vector<EdgeData*>> GetEdges();
			/**
			* Returns true if the mesh is not created yet
			*/
			bool IsEmpty();

			/**
			* Sets its entity position reference
			* @param a_world_pos The entity position.
			*/
			void SetOwnerPosition(Vector3 a_world_pos);

			/**
			* Sets the scale of the texture
			* @param a_scale The scale.
			*/
			void SetTextureScale(float a_scale);

			/**
			* Returns the scale of the texture
			* @return The scale.
			*/
			float GetTextureScale() { return texture_scale_; }

			/**
			* Sets the maximum angle in Radians to allow top edges
			* angles higher than this turn into sides
			* @param a_angle The angle you want to set it to in Degrees
			*/
			void SetMaxAngleTop(float a_angle);
			/**
			* Sets the maximum angle in Radians to allow bottom edges
			* angles lower than this turn into sides
			* @param a_angle The angle you want to set it to in Degrees
			*/
			void SetMaxAngleBot(float a_angle);

			/**
			* Sets height of the edges
			* @param a_height Height of the edges.
			*/
			void SetEdgeHeight(float a_height);
			/**
			* Gets height of the edges
			* @return returns the height of the edges.
			*/
			float GetEdgeHeight();

		private:
			Mesh *terrain_mesh_ = nullptr;							/** < Mesh that will store the inside of the terrain. */
			Triangulation* triangulator_ = nullptr;					/** < Triangulator class to triangulate the inside. */
			Vector3 world_pos_;										/** < position of the owner entity. */
			bool is_enabled_;										/** < boolean if the mesh is enabled. */
			float texture_scale_ = 1;								/** < Scale value for how big the tiled texture will be. */
			std::shared_ptr<std::vector<EdgeData*>> terrain_edges_;	/** < Array of the edge information of all the edges. */
			float edge_height_ = 1.f;								/** < Height of the edges */
			float max_angle_top_ = 45.f;							/** < Max angle for top edge to be displayed, an edge with an angle lower that this will turn into a side edge. */
			float max_angle_bot_ = 160.f;							/** < Max angle for bot edge to be displayed, an edge with an angle higher that this will turn into a side edge. */
			std::vector<uint32> edge_indices_{ 0,1,2,0,2,3 };			/** < Default set of indices that edges will use. */

			/*
			* Generates the edges for the terrain
			* @param is_closed If true it will also create an edge between front and back.
			* @param polygon The polygon data that it will use to generate the edges
			*/
			void GenerateEdges(bool is_closed, std::vector<Vector2> *polygon);

			/**
			* Calculates position in the middle of 2 given points.
			* @param point_a Point A.
			* @param point_b Point B.
			* @return returns position exactly between point A and B.
			*/
			Vector3 CalcPointBetween(const Vector3 point_a, const Vector3 point_b) const;
		};
	} // End of namespace ~ tt
} // End of namespace ~ iga