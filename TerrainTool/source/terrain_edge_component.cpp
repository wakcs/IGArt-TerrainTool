#include <TerrainTool\terrain_edge_component.h>
#include <ig_art_engine.h>
#ifdef EDITOR
#include <imgui/include/imgui.h>
#endif

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(iga::tt::TerrainEdgeComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(iga::BaseRenderingComponent, iga::tt::TerrainEdgeComponent)

namespace iga
{
	namespace tt
	{
		TerrainEdgeComponent::TerrainEdgeComponent()
			:BaseRenderingComponent(std::weak_ptr<Entity>())
		{
			component_type_ = "TerrainEdgeComponent";
			edge_mesh_ = nullptr;
		}


		TerrainEdgeComponent::TerrainEdgeComponent(std::weak_ptr<Entity> a_entity)
			: BaseRenderingComponent(a_entity)
		{
			component_type_ = "TerrainEdgeComponent";
			edge_mesh_ = nullptr;
		}

		TerrainEdgeComponent::~TerrainEdgeComponent()
		{
		}
		void TerrainEdgeComponent::OnCreate(bool a_on_load)
		{
			if (a_on_load) {
				SetTexture(edge_texture_resource_->GetPath());
				if(edge_mesh_)
				{
					edge_mesh_resource_ = GetResourceManager()->LoadGeneratedMesh(edge_mesh_);
				}
				else {
					edge_mesh_resource_ = GetResourceManager()->Load<Mesh>(edge_mesh_resource_->GetPath());
				}
			}
			else {
				SetTexture("Default");
				edge_mesh_resource_ = GetResourceManager()->Load<Mesh>("Sprite");
			}

			if(std::shared_ptr<Entity> owner = owner_.lock()) {
				owner->SetVisibleInHierarchy(false);
			}
		}
		void TerrainEdgeComponent::Update()
		{
		}
		void TerrainEdgeComponent::PostUpdate()
		{
		}
		void TerrainEdgeComponent::SetTexture(const std::string & a_path)
		{
			edge_texture_resource_ = GetResourceManager()->Load<Texture>(a_path);
		}
		void TerrainEdgeComponent::SetMesh(Mesh * a_mesh)
		{
			edge_mesh_ = a_mesh;
			edge_mesh_resource_ = GetResourceManager()->LoadGeneratedMesh(new Mesh(*a_mesh));
		}
		void TerrainEdgeComponent::SetType(EdgeType a_type)
		{
			edge_type_ = a_type;
		}
		EdgeType TerrainEdgeComponent::GetType()
		{
			return edge_type_;
		}
		RenderDataStruct * TerrainEdgeComponent::OnBuildRenderData()
		{
			if (visible_) {
#ifdef EDITOR
				if (!visible_editor_only_ || (visible_editor_only_ && !GetGame()->IsPlaying())) {
#endif
					RenderDataStruct *rds = new RenderDataStruct();
					rds->mesh_ = GetResourceManager()->Get<Mesh>(edge_mesh_resource_);
					rds->model_matrix_ = owner_.lock()->GetTransform().lock()->GetTransform();
					rds->modifier_ = modifiers_;
					rds->texture_ = GetResourceManager()->Get<Texture>(edge_texture_resource_);
					return rds;
#ifdef EDITOR
				}
#endif
			}
			return nullptr;
		}
		void TerrainEdgeComponent::Inspect()
		{
		}
	}
}
