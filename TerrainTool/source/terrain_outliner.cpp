#include <TerrainTool\terrain_outliner.h>
#include <TerrainTool\terrain_collider.h>
#include <TerrainTool\terrain_mesh.h>
#include <iostream>

namespace iga
{
	namespace tt
	{
		TerrainOutliner::TerrainOutliner(Vector3 a_world_pos, TerrainCollider * a_collider, TerrainMesh * a_mesh, std::vector<Vector3>* a_mesh_points, bool a_closed)
		{
			assert(a_collider && a_mesh);
			terrain_collider_ = a_collider;
			terrain_mesh_ = a_mesh;

			SetOwnerPosition(a_world_pos);

			if (a_mesh_points) {
				mesh_points_ = std::make_shared<std::vector<Vector3>>(*a_mesh_points);
			}
			else {
				mesh_points_ = std::make_shared<std::vector<Vector3>>();
				mesh_points_->push_back(Vector3(0, 0, 0));
				mesh_points_->push_back(Vector3(0, 10, 0));
				mesh_points_->push_back(Vector3(20, 10, 0));
				mesh_points_->push_back(Vector3(20, 0, 0));
			}

#ifdef EDITOR
			Vector3 pos;
			add_points_ = std::make_shared<std::vector<Vector3>>();
			for (int i = 0; i < mesh_points_->size() - 1; ++i)
			{
				pos = CalcPointBetween(mesh_points_->at(i + 1), mesh_points_->at(i));
				add_points_->push_back(pos);
			}
			if (a_closed) {
				pos = CalcPointBetween(mesh_points_->back(), mesh_points_->front());
				add_points_->push_back(pos);
			}
#endif // EDITOR

			is_closed_ = a_closed;
			GenerateTerrain();
		}

		TerrainOutliner::~TerrainOutliner()
		{
			mesh_points_.reset();
			add_points_.reset();
		}

		void TerrainOutliner::InsertMeshPoint(int index, Vector3 position)
		{
#ifdef EDITOR
			Vector3 local_pos = position - world_pos_;
			if (index <= mesh_points_->size())
			{
				for (size_t i = index - 1; i < index + 1; ++i)
				{
					if (i < mesh_points_->size() &&
						dist(Point3(local_pos), Point3(mesh_points_->at(i))) < b2_linearSlop)
					{
						std::cout << "Position too close to neighbours, won't be added!\n";
						return;
					}
				}
				mesh_points_->insert(mesh_points_->begin() + index, local_pos);
				InsertAddPoints(index);
				GenerateTerrain();
			}
#endif // EDITOR
		}

		void TerrainOutliner::RemoveMeshPoint()
		{
#ifdef EDITOR
			if (active_point_ >= 0 && active_point_ < mesh_points_->size())
			{
				if (mesh_points_->size() <= 2)
				{
					std::cout << "Terrain needs at least 2 points, cannot remove more!\n";
					active_point_ = -1;
					return;
				}
				else if (mesh_points_->size() <= 3)
				{
					is_closed_ = false;
				}
				mesh_points_->erase(mesh_points_->begin() + active_point_);
				RemoveAddPoints(active_point_);
			}
			active_point_ = -1;
			GenerateTerrain();
#endif // EDITOR
		}

		bool TerrainOutliner::UpdateActiveMeshPoint(Vector3 position)
		{
#ifdef EDITOR
			Vector3 local_pos = position - world_pos_;
			if (active_point_ >= 0)
			{
				for (size_t i = active_point_ - 1; i < active_point_ + 2; ++i)
				{
					if (i != active_point_ && i < mesh_points_->size() &&
						dist(Point3(local_pos), Point3(mesh_points_->at(i))) < b2_linearSlop)
					{
						std::cout << "Position too close to neighbours, won't be modified!\n";
						return false;
					}
				}
				mesh_points_->at(active_point_) = local_pos;
				
				for (int i = active_point_ - 1; i < active_point_ + 1; ++i)
				{
					if (i >= 0 && i < mesh_points_->size() - 1)
					{
						add_points_->at(i) = CalcPointBetween(mesh_points_->at(i + 1), mesh_points_->at(i));
					}
					else if (is_closed_)
					{
						add_points_->back() = CalcPointBetween(mesh_points_->front(), mesh_points_->back());
					}
				}
				GenerateTerrain();
			}
#endif // EDITOR
			return true;
		}

		bool TerrainOutliner::GetIsClosed()
		{
			return is_closed_;
		}

		void TerrainOutliner::SetIsClosed(bool is_closed)
		{
			is_closed_ = is_closed;
#ifdef EDITOR
			if (!is_closed_)
			{
				add_points_->pop_back();
			}
			else if (mesh_points_->size() > 2)
			{
				Vector3 pos = CalcPointBetween(mesh_points_->front(), mesh_points_->back());
				add_points_->push_back(pos);
			}
			else
			{
				std::cout << "Cannot close a single line terrain!\n";
				is_closed_ = false;
			}
#endif // EDITOR
			GenerateTerrain();
		}

		std::shared_ptr<std::vector<Vector3>> TerrainOutliner::GetMeshPoints()
		{
			return mesh_points_;
		}

		std::shared_ptr<std::vector<Vector3>> TerrainOutliner::GetAddPoints()
		{
			return add_points_;
		}

		int TerrainOutliner::GetActivePointIndex()
		{
			return active_point_;
		}

		Vector3 TerrainOutliner::GetActivePoint()
		{
			if (active_point_ >= 0)
			{
				return mesh_points_->at(active_point_);
			}
			return Vector3();
		}

		void TerrainOutliner::SetActivePoint(int active_point)
		{
			active_point_ = active_point;
		}

		TerrainCollider * TerrainOutliner::GetCollider()
		{
			return terrain_collider_;
		}

		TerrainMesh * TerrainOutliner::GetMesh()
		{
			return terrain_mesh_;
		}

		void TerrainOutliner::SetOwnerPosition(Vector3 a_world_pos)
		{
			world_pos_ = a_world_pos;
		}

		void TerrainOutliner::InsertAddPoints(int index)
		{
#ifdef EDITOR
			Vector3 pos;
			for (int i = index - 1; i < index + 1; ++i)
			{
				if (i >= 0 && i < mesh_points_->size() - 1)
				{
					pos = CalcPointBetween(mesh_points_->at(i + 1), mesh_points_->at(i));
					add_points_->insert(add_points_->begin() + i, pos);
				}
				else if (is_closed_)
				{
					pos = CalcPointBetween(mesh_points_->front(), mesh_points_->back());
					add_points_->push_back(pos);
				}
			}
			if (index > 0 && (is_closed_ || index < mesh_points_->size() - 1))
			{
				add_points_->erase(add_points_->begin() + index + 1);
			}
#endif // EDITOR
		}

		void TerrainOutliner::RemoveAddPoints(int index)
		{
#ifdef EDITOR
			Vector3 pos;
			if (index == 0)
			{
				add_points_->erase(add_points_->begin());
			}
			else if (index == mesh_points_->size())
			{
				add_points_->pop_back();
				if (is_closed_)
				{
					pos = CalcPointBetween(mesh_points_->front(), mesh_points_->back());
					add_points_->back() = pos;
				}
			}
			else
			{
				for (int i = 0; i < 2; ++i)
				{
					add_points_->erase(add_points_->begin() + (index - 1));
				}
				pos = CalcPointBetween(mesh_points_->at(index - 1), mesh_points_->at(index));
				add_points_->insert(add_points_->begin() + (index - 1), pos);
			}
#endif // EDITOR
		}

		Vector3 TerrainOutliner::CalcPointBetween(Vector3 point_a, Vector3 point_b)
		{
#ifdef EDITOR
			Vector3 dir = Vector3(point_b - point_a);
			float dist = length(dir) / 2;
			return point_a + (normalize(dir)*dist);
#endif // EDITOR
		}

		void TerrainOutliner::GenerateTerrain()
		{
			if (terrain_collider_ && terrain_mesh_)
			{
				terrain_collider_->GenerateCollider(is_closed_, mesh_points_);
				terrain_mesh_->GenerateMesh(is_closed_, mesh_points_);
			}
			else
			{
				std::cout << "Terrain collider and/or mesh components are missing!";
			}
			got_modified_ = true;
		}
	} // End of namespace ~ tt
} // End of namespace ~ iga