#pragma once
#include <vectormath/vectormath.hpp>
#include <vector>
#include <Box2D\Box2D.h>

/**
* @file terrain_collider.h
* @author Geert Cocu
* @date 2 mar 2018
* @brief TerrainCollider class header.
*
* The TerrainCollider is used to generate the collision polygon of the terrain
*/
namespace iga
{
	namespace tt
	{
		class TerrainCollider
		{
		public:
			/**
			* Constructor.
			*/
			TerrainCollider(Vector3 a_world_pos, b2World* a_physics_world);
			/**
			* Deconstructor.
			*/
			~TerrainCollider();

			/**
			* Generates the collider polygon for the terrain.
			* @param is_closed Bool to generate a closed or open collider.
			* @param points Vector of points it will use to generate the collider.
			* @returns returns true if generation is successful.
			*/
			bool GenerateCollider(const bool a_closed, const std::shared_ptr<std::vector<Vector3>> a_nodes);

			/**
			* Returns the shape.
			* @return Returns the shape.
			*/
			b2ChainShape* GetShape();

			/**
			* Sets its entity position reference
			* @param a_world_pos The entity position.
			*/
			void SetOwnerPosition(Vector3 a_world_pos);

			bool IsEmpty();

			bool GetEnabled() { return is_enabled_; }
			void SetEnabled(bool a_enabled);
		
		private:
			bool is_enabled_ = true; /** < boolean if the collider is enabled. */
			b2BodyDef body_def_;	 /** < body def, used for positioning, friction ect. */
			b2Body* body_;			 /** < body, holds a reference to the body that is placed in the world. */
			b2ChainShape shape_;	 /** < body shape, used to define the shape of the body. */
			Vector3 world_pos_;		 /** < position of the owner entity. */
		};
	} // End of namespace ~ tt
} // End of namespace ~ iga