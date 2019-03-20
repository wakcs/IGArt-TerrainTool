#pragma once
#include <vectormath/vectormath.hpp>
#include <defines.h>

class b2World;
namespace iga
{
	namespace tt 
	{
		class TerrainOutliner;
		class TerrainCollider;
		class TerrainMesh;
		class TerrainDebugger;

		class TerrainBuilder
		{
		public:
			TerrainBuilder(Vector3 a_world_pos, b2World* a_physics_world, std::vector<Vector3>* a_mesh_points = nullptr, bool a_closed = false);
			~TerrainBuilder();
			/**
			* Sets its entity position reference
			* @param a_world_pos The entity position.
			*/
			void SetOwnerPosition(Vector3 a_world_pos);

			TerrainCollider* terrain_collider_ = nullptr;
			TerrainMesh* terrain_mesh_ = nullptr;
			TerrainOutliner* terrain_outiner_ = nullptr;
#ifdef EDITOR
			TerrainDebugger* terrain_debugger_ = nullptr;
#endif // EDITOR
		};
	} // End of namespace ~ tt
} // End of namespace ~ iga

