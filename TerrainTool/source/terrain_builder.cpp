#include <TerrainTool\terrain_builder.h>
#include <TerrainTool\terrain_collider.h>
#include <TerrainTool\terrain_mesh.h>
#include <TerrainTool\terrain_outliner.h>
#include <TerrainTool\terrain_debugger.h>

namespace iga
{
	namespace tt
	{
		TerrainBuilder::TerrainBuilder(Vector3 a_world_pos, b2World * a_physics_world, std::vector<Vector3>* a_mesh_points, bool a_closed)
		{
			terrain_collider_ = new TerrainCollider(a_world_pos, a_physics_world);
			terrain_mesh_ = new TerrainMesh(a_world_pos);
			terrain_outiner_ = new TerrainOutliner(a_world_pos, terrain_collider_, terrain_mesh_, a_mesh_points, a_closed);
#ifdef EDITOR
			terrain_debugger_ = new TerrainDebugger(terrain_outiner_);
#endif
		}

		TerrainBuilder::~TerrainBuilder()
		{
#ifdef EDITOR
			delete terrain_debugger_; 
#endif
			delete terrain_outiner_;
			delete terrain_collider_;
			delete terrain_mesh_;
		}
		void TerrainBuilder::SetOwnerPosition(Vector3 a_world_pos)
		{
			terrain_collider_->SetOwnerPosition(a_world_pos);
			terrain_mesh_->SetOwnerPosition(a_world_pos);
#ifdef EDITOR
			terrain_outiner_->SetOwnerPosition(a_world_pos);
#endif // EDITOR
		}
	}
}
