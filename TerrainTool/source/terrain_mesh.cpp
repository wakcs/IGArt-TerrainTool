#include <TerrainTool\terrain_mesh.h>
#include <TerrainTool\Triangulation.h>
#include <Resource\resource.h>
#include <ig_art_engine.h>
#ifdef _WIN32
#undef max
#undef min
#endif
namespace iga
{
	namespace tt
	{
		TerrainMesh::TerrainMesh(Vector3 a_world_pos)
		{
			triangulator_ = new Triangulation();
			terrain_mesh_ = new Mesh();
			SetOwnerPosition(a_world_pos);
			max_angle_top_ = std::cos((max_angle_top_ * b2_pi / 180.0f));
			max_angle_bot_ = std::cos((max_angle_bot_ * b2_pi / 180.0f));
			terrain_edges_ = std::make_shared<std::vector<EdgeData*>>();
		}

		TerrainMesh::~TerrainMesh()
		{
			terrain_edges_.reset();
		}

		bool TerrainMesh::GenerateMesh(const bool is_closed, const std::shared_ptr<std::vector<Vector3>> points)
		{
			terrain_mesh_->vertices_.clear();
			Vertex ver;
			ver.normal_ = Vector3(0, 0, -1);

			Polygon polygon;
			Vector2 vec2d;
			for (int i = 0; i < points->size(); ++i)
			{
				
				vec2d = Vector2(points->at(i).getX(), points->at(i).getY());
				ver.position_ = points->at(i);
				ver.uv_ = vec2d / texture_scale_;
				terrain_mesh_->vertices_.push_back(ver);
				polygon.push_back(vec2d);
			}

			/**
			* Personal earclipping
			*/
			terrain_mesh_->indices_ = triangulator_->Triangulate(polygon);
			if (terrain_mesh_->indices_.size() > 2) {
				GenerateEdges(is_closed, &polygon);
				return true;
			}
			terrain_mesh_->vertices_.clear();
			return false;
		}
		Mesh * TerrainMesh::GetMesh()
		{
			return terrain_mesh_;
		}
		std::shared_ptr<std::vector<EdgeData*>> TerrainMesh::GetEdges()
		{
			return terrain_edges_;
		}
		bool TerrainMesh::IsEmpty()
		{
			return (terrain_mesh_->vertices_.size() == 0);
		}
		void TerrainMesh::SetOwnerPosition(Vector3 a_world_pos)
		{
			world_pos_ = a_world_pos;
		}
		void TerrainMesh::SetTextureScale(float a_scale)
		{
			texture_scale_ = a_scale;
			Vector2 v2;
			for (int i = 0; i < terrain_mesh_->vertices_.size(); ++i)
			{
				v2 = Vector2(terrain_mesh_->vertices_[i].position_.getX(), terrain_mesh_->vertices_[i].position_.getY());
				terrain_mesh_->vertices_[i].uv_ = v2 / texture_scale_;
			}
		}
		void TerrainMesh::SetMaxAngleTop(float a_angle)
		{
			max_angle_top_ = std::cos((a_angle * float(M_PI) / 180.0f));
		}
		void TerrainMesh::SetMaxAngleBot(float a_angle)
		{
			max_angle_top_ = std::cos((a_angle * float(M_PI) / 180.0f));
		}
		void TerrainMesh::SetEdgeHeight(float a_height)
		{
			edge_height_ = a_height;
		}
		float TerrainMesh::GetEdgeHeight()
		{
			return edge_height_;
		}
		void TerrainMesh::GenerateEdges(bool is_closed, std::vector<Vector2>* polygon)
		{
			terrain_edges_->clear();
			Vector2 upvec(1, 0);
			Vector2 univec;
			Vector2 pos_a, pos_b;
			float edge_width;
			EdgeData* edge_data;
			Vertex ver;
			ver.normal_ = Vector3(0, 0, -1);
			ver.uv_ = Vector2(0, 0);
			for (int i = 0; i < polygon->size(); ++i)
			{
				if (i < polygon->size() - 1) {
					pos_a = polygon->at(i);
					pos_b = polygon->at(i + 1);
				}
				else if (is_closed) {
					pos_a = polygon->back();
					pos_b = polygon->front();
				}
				else {
					break;
				}
				edge_width = dist(Point2(pos_a), Point2(pos_b)) / 2;

				edge_data = new EdgeData();

				//Angle
				univec = normalize(pos_b - pos_a);
				float dotp = dot(univec, upvec);
				if (dotp > max_angle_top_) {
					edge_data->type = EdgeType::TOP;
				}
				else if (dotp < max_angle_top_ && dotp > max_angle_bot_) {
					edge_data->type = EdgeType::SIDE;
				}
				else if (dotp < max_angle_bot_) {
					edge_data->type = EdgeType::BOTTOM;
				}
				float det = univec.getX()*upvec.getY() - univec.getY()*upvec.getX();
				edge_data->angle = -atan2(det, dotp);

				//Mesh data
				edge_data->mesh = new Mesh();
				edge_data->mesh->indices_ = edge_indices_;
				switch (edge_data->type)
				{
				case EdgeType::TOP:
				case EdgeType::BOTTOM:
					edge_width += edge_height_ / 2;
					break;
				}
				//Bot-left
				ver.position_ = Vector3(-edge_width, -edge_height_, 0);
				ver.uv_ = Vector2(-edge_width / texture_scale_, edge_height_ / texture_scale_);
				edge_data->mesh->vertices_.push_back(ver);
				//Top-left
				ver.position_ = Vector3(-edge_width, 0, 0);
				ver.uv_ = Vector2(-edge_width / texture_scale_, 0);
				edge_data->mesh->vertices_.push_back(ver);
				//Top-right
				ver.position_ = Vector3(edge_width, 0, 0);
				ver.uv_ = Vector2(edge_width / texture_scale_, 0);
				edge_data->mesh->vertices_.push_back(ver);
				//Bot-right
				ver.position_ = Vector3(edge_width, -edge_height_, 0);
				ver.uv_ = Vector2(edge_width / texture_scale_, edge_height_ / texture_scale_);
				edge_data->mesh->vertices_.push_back(ver);


				//Position
				edge_data->pos = CalcPointBetween(Vector3(pos_a.getX(),pos_a.getY(), 0), Vector3(pos_b.getX(),pos_b.getY(), 0));
				switch (edge_data->type)
				{
				case EdgeType::TOP:
				case EdgeType::BOTTOM:
					edge_data->pos.setZ(0.2f);
					break;
				case EdgeType::SIDE:
					edge_data->pos.setZ(0.15f);
					break;
				}
				terrain_edges_->push_back(edge_data);
			}
		}
		Vector3 TerrainMesh::CalcPointBetween(const Vector3 point_a, const Vector3 point_b) const
		{
			Vector3 dir = Vector3(point_b - point_a);
			float dist = length(dir) / 2;
			return point_a + (normalize(dir)*dist);
		}
	}
}