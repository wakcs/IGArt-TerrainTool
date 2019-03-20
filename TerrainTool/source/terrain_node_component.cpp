#include <TerrainTool\terrain_node_component.h>
#include <ig_art_engine.h>
#include <TerrainTool\terrain_outliner.h>

#ifdef EDITOR
#include <imgui/include/imgui.h>
#endif

namespace iga
{
	namespace tt
	{
		TerrainNodeComponent::TerrainNodeComponent()
			:Component(std::weak_ptr<Entity>())
		{
		}

		TerrainNodeComponent::TerrainNodeComponent(std::weak_ptr<Entity> a_entity)
			:Component(a_entity)
		{
			component_type_ = "TerrainNodeComponent";
		}


		TerrainNodeComponent::~TerrainNodeComponent()
		{
		}

		void TerrainNodeComponent::OnCreate(bool a_on_load)
		{
			UNUSED( a_on_load );
			if (std::shared_ptr<Entity> entity = GetOwner().lock()) {
				if (std::shared_ptr<SpriteComponent> sprite = entity->GetComponent<SpriteComponent>().lock()) {
					sprite->SetVisibilityEditorOnly(true);
				}
				if (std::shared_ptr<Transform> transform = entity->GetTransform().lock()) {
					old_pos_ = transform->GetPosition();
				}

				entity->SetVisibleInHierarchy(false);
			}

		}

		void TerrainNodeComponent::Update()
		{
			if (std::shared_ptr<Transform> transform = GetOwner().lock()->GetTransform().lock()) {
				if (transform->dirty_mask_ & static_cast<uint8>(Transform::DirtyFlags::TERRAIN)) {
					if (std::shared_ptr<Transform> p_transform = transform->GetParent().lock()) {
						if (terrain_outliner_ && node_index_ >= 0 && is_movable_) {
							terrain_outliner_->SetActivePoint(node_index_);
							Vector3 n_pos = transform->GetPosition();
							n_pos.setZ(n_pos.getZ() - node_offset_);
							if (!terrain_outliner_->UpdateActiveMeshPoint(p_transform->GetPosition() + n_pos)) {
								transform->SetPosition(old_pos_);
							}
						}
						old_pos_ = transform->GetPosition();
					}
				}

				transform->dirty_mask_ &= ~(static_cast<uint8>(Transform::DirtyFlags::TERRAIN));
			}
		}

		void TerrainNodeComponent::PostUpdate()
		{
		}

		void TerrainNodeComponent::Inspect()
		{
#ifdef EDITOR
			if (!is_movable_) {
				if (ImGui::Button("Add this node to terrain")) {
					if (std::shared_ptr<Transform> transform = GetOwner().lock()->GetTransform().lock()) {
						if (std::shared_ptr<Transform> p_transform = transform->GetParent().lock()) {
							if (terrain_outliner_ && node_index_ >= 0) {
								terrain_outliner_->InsertMeshPoint(node_index_, p_transform->GetPosition() + transform->GetPosition());
								terrain_outliner_->SetActivePoint(node_index_);
							}
						}
					}
				}
			}
			else {
				if (ImGui::Button("Remove this node from terrain")) {
					if (std::shared_ptr<Transform> transform = GetOwner().lock()->GetTransform().lock()) {
						if (terrain_outliner_ && node_index_ >= 0) {
							terrain_outliner_->SetActivePoint(node_index_);
							terrain_outliner_->RemoveMeshPoint();
							GetOwner().lock()->GetScene().lock()->DestroyEntity(GetOwner().lock());
						}
					}
				}
			}
#endif // EDITOR
		}

		void TerrainNodeComponent::SetTerrainOutliner(TerrainOutliner * a_outliner)
		{
			terrain_outliner_ = a_outliner;
		}

		void TerrainNodeComponent::SetIndex(int a_index)
		{
			node_index_ = a_index;
		}

		void TerrainNodeComponent::SetMovable(bool a_movable)
		{
			is_movable_ = a_movable;
		}
		void TerrainNodeComponent::SetNodeOffset(float a_offset)
		{
			node_offset_ = a_offset;
		}
	} // End of namespace ~ tt
} // End of namespace ~ iga
