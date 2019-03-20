#include <TerrainTool\terrain_collider.h>

namespace iga
{
	namespace tt
	{
		TerrainCollider::TerrainCollider(Vector3 a_world_pos, b2World * a_physics_world)
		{
			body_def_.position.Set(a_world_pos.getX(), a_world_pos.getY());
			body_ = a_physics_world->CreateBody(&body_def_);
			SetOwnerPosition(a_world_pos);
		}

		TerrainCollider::~TerrainCollider()
		{
			b2Fixture* fix_tmp = body_->GetFixtureList();
			if (fix_tmp)
			{
				body_->DestroyFixture(fix_tmp);
			}
		}

		bool TerrainCollider::GenerateCollider(const bool a_closed, const std::shared_ptr<std::vector<Vector3>> a_nodes)
		{
			// remove old shape
			b2Fixture* fix_tmp = body_->GetFixtureList();
			if (fix_tmp)
			{
				body_->DestroyFixture(fix_tmp);
			}
			shape_.Clear();

			// generate new shape
			size_t terrain_size = a_nodes->size();
			std::vector<b2Vec2> points_2d;
			b2Vec2 node_pos;
			// converts vector of 3D to vector of 2D
			for (size_t i = 0; i < terrain_size; ++i)
			{
				node_pos = b2Vec2(a_nodes->at(i).getX(), a_nodes->at(i).getY());
				points_2d.push_back(node_pos);
			}
			if (a_closed)
			{
				shape_.CreateLoop(&points_2d[0], uint32(terrain_size));
			}
			else
			{
				shape_.CreateChain(&points_2d[0], uint32(terrain_size));
			}

			// add new shape
			body_->CreateFixture(&shape_, 0.f);
			return true;
		}

		b2ChainShape * TerrainCollider::GetShape()
		{
			return &shape_;
		}

		void TerrainCollider::SetOwnerPosition(Vector3 a_world_pos)
		{
			world_pos_ = a_world_pos;
			body_->SetTransform(b2Vec2(world_pos_.getX(), world_pos_.getY()), 0);
		}

		bool TerrainCollider::IsEmpty()
		{
			return (!body_->GetFixtureList());
		}
		void TerrainCollider::SetEnabled(bool a_enabled)
		{
			is_enabled_ = a_enabled;
			b2Fixture* fix_tmp = body_->GetFixtureList();
			if (is_enabled_ && !fix_tmp) {
				body_->CreateFixture(&shape_, 0.f);
			}
			else if (fix_tmp)
			{
				body_->DestroyFixture(fix_tmp);
			}
		}
	}
}
