#pragma once
#include <vectormath/vectormath.hpp>
#include <vector>
#include <memory>

/**
* @file terrain_outliner.h
* @author Geert Cocu
* @date 2 mar 2018
* @brief TerrainOutliner class header.
*
* The TerrainOutliner is used to define the outline of the terrain, and modify this outline.
*/

class b2World;

namespace iga
{
	namespace tt
	{
		class TerrainCollider;
		class TerrainMesh;

		class TerrainOutliner
		{
		public:
			/**
			* Constructor.
			* @param owner The owner of the component it's attached to.
			*/
			TerrainOutliner(Vector3 a_world_pos, TerrainCollider* a_collider, TerrainMesh* a_mesh, std::vector<Vector3>* a_mesh_points = nullptr, bool a_closed = false);
			/**
			* Deconstructor.
			*/
			~TerrainOutliner();

			/**
			* Inserts a mesh point to the vector at the given index.
			* Also calls InsertAddPoints().
			* @param index The index where the point will be added to.
			* @param position The position of the new point in worldspace.
			*/
			void InsertMeshPoint(int a_index, Vector3 a_position);

			/**
			* Removes a mesh point from the vector at the given index.
			* Also calls RemoveAddPoints().
			* @param index The index where the point will be removed from.
			*/
			void RemoveMeshPoint();

			/**
			* Updates the active point to a given location, if active_point is set.
			* @param position Position in worldspace the active point is being set to.
			*/
			bool UpdateActiveMeshPoint(Vector3 a_position);

			/**
			* Returns if the terrain is closed or open.
			* @return returns the value of is_closed_.
			*/
			bool GetIsClosed();
			/**
			* Sets the bool of the terrain is closed or open.
			* And generates the line and Add point between the beginning and end.
			* @param is_closed True if closed False if open.
			*/
			void SetIsClosed(bool a_closed);
			/**
			* Returns all the points of the terrain
			* @return returns all the points of the terrain.
			*/
			std::shared_ptr<std::vector<Vector3>> GetMeshPoints();
			/**
			* Returns all the add points of the terrain
			* @return returns all the add points of the terrain.
			*/
			std::shared_ptr<std::vector<Vector3>> GetAddPoints();
			/**
			* Returns the index of the active point, -1 if non is set.
			* @return returns the index.
			*/
			int GetActivePointIndex();
			/**
			* Returns the active point, 0 if none is set.
			* @return returns the point.
			*/
			Vector3 GetActivePoint();
			/**
			* Sets the active point of the index.
			*/
			void SetActivePoint(int a_active_point);

			/**
			* @return Returns it's collider
			*/
			TerrainCollider* GetCollider();
			/**
			* @return Returns it's mesh
			*/
			TerrainMesh* GetMesh();
			/**
			* Sets its entity position reference
			* @param a_world_pos The entity position.
			*/
			void SetOwnerPosition(Vector3 a_world_pos);

			bool GotModified() { return got_modified_; };
			void ResetModified() { got_modified_ = false; };

		private:
			TerrainCollider * terrain_collider_ = nullptr;	/** < pointer to the collision component. */
			TerrainMesh * terrain_mesh_ = nullptr;			/** < pointer to the mesh component. */
			std::shared_ptr<std::vector<Vector3>> mesh_points_;				/** < vector of the mesh points, used to draw lines and generate collision and stuff. */
			Vector3 world_pos_;								/** < position of the owner entity. */
			bool is_closed_ = false;						/** < boolean whether the terrain is closed or open. */
			std::shared_ptr<std::vector<Vector3>> add_points_;				/** < vector of points that can be added to the mesh points. */
			int active_point_ = -1;							/** < index of the active point. */
			bool got_modified_ = true;						/** < boolean that turns true when the shape gets modified. */

			/**
			* Generates addpoints around the point from the given index.
			* @param index Index of the point it needs to generate around.
			*/
			void InsertAddPoints(int a_index);
			/**
			* Removes addpoints around the point from the given index.
			* @param index Index of the point it removes around.
			*/
			void RemoveAddPoints(int a_index);

			/**
			* Calculates position in the middle of 2 given points.
			* @param point_a Point A.
			* @param point_b Point B.
			* @return returns position exactly between point A and B.
			*/
			Vector3 CalcPointBetween(Vector3 a_point_a, Vector3 a_point_b);
			/**
			* Generates the collider and the texture of the terrain.
			*/
			void GenerateTerrain();
		};
	} // End of namespace ~ tt
} // End of namespace ~ iga