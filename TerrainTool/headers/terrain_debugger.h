#pragma once
#include <vectormath/vectormath.hpp>

#ifdef EDITOR
#include <Graphics\dx_12\DebugRenderer.h>
#endif // EDITOR

/**
* @file terrain_debugger.h
* @author Geert Cocu
* @date 9 mar 2018
* @brief TerrainDebugger class header.
*
* The TerrainDebugger is used to debug the terrain, mainly with debuglines.
*/
namespace iga
{
	namespace tt
	{
		class TerrainOutliner;
		class TerrainCollider;
		class TerrainMesh;

		class TerrainDebugger
		{
		public:
			/**
			* Constructor.
			* @param terrain_outliner The terrain outliner.
			* @param terrain_collider The terrain collider.
			* @param terrain_mesh The terrain mesh.
			*/
			TerrainDebugger(TerrainOutliner* a_terrain_outliner);
			/**
			* Deconstructor.
			*/
			~TerrainDebugger();

			/**
			* Function to draw debug lines
			* @param owner_pos The position of the owner.
			*/
			void DrawDebug(Vector3 a_owner_pos);

			/**
			* @return Returns bool if the debugger is enabled. (by default it is)
			*/
			bool GetIsEnabled();
			/**
			* Set the enabled state of the debugger.
			* @param is_enabled Boolean.
			*/
			void SetIsEnabled(bool a_enabled);

		private:
			TerrainOutliner * terrain_outliner_ = nullptr;	/** < Reference to outliner. */
			std::shared_ptr<std::vector<Vector3>> outline_nodes_;
#ifdef EDITOR
			iga::DebugRenderer* debug_renderer_;			/** < Reference to debug renderer. */
			std::vector<iga::DebugLine> debug_lines_;		/** < vector of all the debuglines. */			
#endif // EDITOR
			bool is_enabled_ = true;						/** < Bool if the debugger is enabled (default is true). */
		};
	} // End of namespace ~ tt
} // End of namespace ~ iga