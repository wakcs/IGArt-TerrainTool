#include <TerrainTool\terrain_debugger.h>
#include <TerrainTool\terrain_outliner.h>
#include <TerrainTool\terrain_mesh.h>
#include <TerrainTool\terrain_collider.h>
#include <ig_art_engine.h>

namespace iga
{
	namespace tt
	{
		TerrainDebugger::TerrainDebugger(TerrainOutliner * terrain_outliner)
		{
			assert(terrain_outliner);
			terrain_outliner_ = terrain_outliner;
			outline_nodes_ = terrain_outliner_->GetMeshPoints();
#ifdef EDITOR
			RVX::RendererD3D12* dx_renderer = static_cast<RVX::RendererD3D12*>(iga::Application::renderer_);
			assert(dx_renderer);
			debug_renderer_ = &dx_renderer->GetDebugRenderer();
			assert(debug_renderer_);
#endif // EDITOR
		}

		TerrainDebugger::~TerrainDebugger()
		{
#ifdef EDITOR
			for (int i = 0; i < debug_lines_.size(); ++i)
			{
				debug_renderer_->RemoveLine(&debug_lines_[i]);
			}
			debug_lines_.clear();
#endif // EDITOR
		}

		void TerrainDebugger::DrawDebug(Vector3 a_owner_pos)
		{
#ifdef EDITOR
			if (!is_enabled_ || GetGame()->IsPlaying())
			{
				if(debug_lines_.size()) {
					for(int i = 0; i < debug_lines_.size(); ++i) {
						debug_renderer_->RemoveLine(&debug_lines_[i]);
					}
					debug_lines_.clear();
				}
				return;
			}

			for (int i = 0; i < debug_lines_.size(); ++i)
			{
				debug_renderer_->RemoveLine(&debug_lines_[i]);
			}
			debug_lines_.clear();
			//draw outline lines
			if (terrain_outliner_)
			{
				Vector3 s;
				Vector3 e;
				for (int i = 0; i < outline_nodes_->size(); ++i)
				{
					if (i < outline_nodes_->size() - 1)
					{
						s = a_owner_pos + outline_nodes_->at(i);
						e = a_owner_pos + outline_nodes_->at(i + 1);
						iga::DebugLine l = iga::DebugLine(s.getX(), s.getY(), s.getZ(), e.getX(), e.getY(), e.getZ());
						debug_lines_.push_back(l);
					}
				}
				if (terrain_outliner_->GetIsClosed())
				{
					s = a_owner_pos + outline_nodes_->back();
					e = a_owner_pos + outline_nodes_->front();
					iga::DebugLine l = iga::DebugLine(s.getX(), s.getY(), s.getZ(), e.getX(), e.getY(), e.getZ());
					debug_lines_.push_back(l);
				}
				for (int i = 0; i < debug_lines_.size(); ++i)
				{
					debug_renderer_->AddLine(&debug_lines_[i]);
				}
			}
#endif // EDITOR
		}

		bool TerrainDebugger::GetIsEnabled()
		{
			return is_enabled_;
		}

		void TerrainDebugger::SetIsEnabled(bool is_enabled)
		{
			is_enabled_ = is_enabled;
		}
	}
}
