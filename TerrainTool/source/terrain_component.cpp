#include <TerrainTool\terrain_component.h>

#include <ig_art_engine.h>

#include <fstream>

#include <TerrainTool\terrain_builder.h>
#include <TerrainTool\terrain_collider.h>
#include <TerrainTool\terrain_mesh.h>
#include <TerrainTool\terrain_outliner.h>
#include <TerrainTool\terrain_debugger.h>
#include <TerrainTool\terrain_node_component.h>
#include <TerrainTool\terrain_edge_component.h>

#ifdef EDITOR
#include <imgui/include/imgui.h>
#endif

namespace iga
{
	namespace tt
	{
#ifdef EDITOR
		bool TerrainComponent::draw_edges = false;
#endif

		TerrainComponent::TerrainComponent()
			:BaseRenderingComponent(std::weak_ptr<Entity>())
		{
			component_type_ = "TerrainComponent";
		}


		TerrainComponent::TerrainComponent(std::weak_ptr<Entity> a_entity)
			: BaseRenderingComponent(a_entity)
		{
			component_type_ = "TerrainComponent";
		}

		TerrainComponent::~TerrainComponent()
		{
			mesh_nodes_.reset();
			delete terrain_;
			
			if (std::shared_ptr<Scene> scene = scene_.lock()) {
				for (int i = 0; i < e_mesh_nodes_.size(); ++i)
				{
					scene->DestroyEntity(e_mesh_nodes_[i]);
				}
				for (int i = 0; i < e_add_nodes_.size(); ++i)
				{
					scene->DestroyEntity(e_add_nodes_[i]);
				}
				for (int i = 0; i < e_edges_.size(); ++i)
				{
					scene->DestroyEntity(e_edges_[i]);
				}
			}
		}

		void TerrainComponent::OnCreate(bool a_on_load)
		{
			Vector3 trans_pos;
			b2World* world;
			if (std::shared_ptr<Transform> transform = GetOwner().lock()->GetTransform().lock()) {
				trans_pos = transform->GetPosition();
				world = &GetGame()->GetPhysicsWorld();
				if (a_on_load) {
					terrain_ = new TerrainBuilder(trans_pos, world, mesh_nodes_.get(), is_closed_);
					fill_mesh_resource_ = GetResourceManager()->LoadGeneratedMesh(new Mesh(*terrain_->terrain_mesh_->GetMesh()));
					for (int i = 0; i < e_mesh_nodes_.size(); ++i)
					{
						e_mesh_nodes_[i].lock()->GetComponent<TerrainNodeComponent>().lock()->SetTerrainOutliner(terrain_->terrain_outiner_);
					}
					for (int i = 0; i < e_add_nodes_.size(); ++i)
					{
						e_add_nodes_[i].lock()->GetComponent<TerrainNodeComponent>().lock()->SetTerrainOutliner(terrain_->terrain_outiner_);
					}
				}
				else {
					terrain_ = new TerrainBuilder(trans_pos, world);
				}
				SetTexture(EdgeType::FILL, fill_texture_path_);
				SetTexture(EdgeType::TOP, top_texture_path_);
				SetTexture(EdgeType::SIDE, side_texture_path_);
				SetTexture(EdgeType::BOTTOM, bot_texture_path_);
				scene_ = GetOwner().lock()->GetScene();
				mesh_nodes_ = terrain_->terrain_outiner_->GetMeshPoints();
				add_nodes_ = terrain_->terrain_outiner_->GetAddPoints();
				edges_ = terrain_->terrain_mesh_->GetEdges();
				is_closed_ = terrain_->terrain_outiner_->GetIsClosed();
				if (e_edges_.size() == edges_->size()) {
					std::shared_ptr<Entity> entity;
					for (int i = 0; i < edges_->size(); ++i)
					{
						if (entity = e_edges_[i].lock()) {
							if (std::shared_ptr<iga::tt::TerrainEdgeComponent> edge = entity->GetComponent<iga::tt::TerrainEdgeComponent>().lock()) {
								edge->SetMesh(edges_->at(i)->mesh);
							}
						}
					}
				}
			}
		}

		void TerrainComponent::Update()
		{
			if (std::shared_ptr<Transform> transform = GetOwner().lock()->GetTransform().lock()) {
				if (transform->dirty_mask_ & static_cast<uint8>(Transform::DirtyFlags::TERRAIN)) {
					terrain_->SetOwnerPosition(transform->GetPosition());
				}
#ifdef EDITOR
				//adds/moves entities to represent the nodes
				if (terrain_->terrain_outiner_->GotModified() || transform->dirty_mask_ & static_cast<uint8>(Transform::DirtyFlags::TERRAIN)) {
					//Moveable nodes
					if (mesh_nodes_->size() != e_mesh_nodes_.size()) {
						for (int i = 0; i < e_mesh_nodes_.size(); ++i)
						{
							scene_.lock()->DestroyEntity(e_mesh_nodes_[i]);
						}
						e_mesh_nodes_.clear();
						std::weak_ptr<Entity> entity_comp;
						std::shared_ptr<Entity> entity;
						std::weak_ptr<iga::SpriteComponent> sprite_comp;
						std::weak_ptr<iga::tt::TerrainNodeComponent> node_comp;
						if (e_mesh_nodes_.empty()) {
							for (int i = 0; i < mesh_nodes_->size(); ++i)
							{
								entity_comp = scene_.lock()->CreateEntity();
								if (entity = entity_comp.lock()) {
									entity->SetName("move_node_" + std::to_string(i));

									if (std::shared_ptr<iga::Transform> n_transform = entity->GetTransform().lock()) {
										transform->AddChild(n_transform);
										Vector3 n_pos = Vector3(mesh_nodes_->at(i));
										n_pos.setZ(node_offset_);
										n_transform->SetPosition(n_pos);
									}

									sprite_comp = entity->NewComponent<iga::SpriteComponent>();
									if (std::shared_ptr<iga::SpriteComponent> sprite = sprite_comp.lock()) {
										sprite->SetTexture("res/node_move.png");
									}

									node_comp = entity->NewComponent<iga::tt::TerrainNodeComponent>();
									if (std::shared_ptr<iga::tt::TerrainNodeComponent> node = node_comp.lock()) {
										node->SetTerrainOutliner(terrain_->terrain_outiner_);
										node->SetIndex(i);
										node->SetMovable(true);
										node->SetNodeOffset(node_offset_);
									}
								}
								e_mesh_nodes_.push_back(entity_comp);
							}
						}
					}
					else if (transform->dirty_mask_ & static_cast<uint8>(Transform::DirtyFlags::TERRAIN)) {
						std::shared_ptr<Entity> entity;
						for (int i = 0; i < mesh_nodes_->size(); ++i)
						{
							if (entity = e_mesh_nodes_[i].lock()) {
								if (std::shared_ptr<iga::Transform> n_transform = entity->GetTransform().lock()) {
									Vector3 n_pos = Vector3(mesh_nodes_->at(i));
									n_pos.setZ(node_offset_);
									n_transform->SetPosition(n_pos);
									n_transform->dirty_mask_ &= ~(static_cast<uint8>(Transform::DirtyFlags::TERRAIN));
								}
							}
						}
					}

					//Addable nodes
					if (add_nodes_->size() != e_add_nodes_.size()) {
						for (int i = 0; i < e_add_nodes_.size(); ++i)
						{
							scene_.lock()->DestroyEntity(e_add_nodes_[i]);
						}
						e_add_nodes_.clear();						
						std::weak_ptr<Entity> entity_comp;
						std::shared_ptr<Entity> entity;
						std::weak_ptr<iga::SpriteComponent> sprite_comp;
						std::weak_ptr<iga::tt::TerrainNodeComponent> node_comp;
						if (e_add_nodes_.empty()) {
							for (int i = 0; i < add_nodes_->size(); ++i)
							{
								entity_comp = scene_.lock()->CreateEntity();
								if (entity = entity_comp.lock()) {
									entity->SetName("add_node_" + std::to_string(i));

									if (std::shared_ptr<iga::Transform> n_transform = entity->GetTransform().lock()) {
										transform->AddChild(n_transform);
										Vector3 n_pos = Vector3(add_nodes_->at(i));
										n_pos.setZ(node_offset_);
										n_transform->SetPosition(n_pos);
									}

									sprite_comp = entity->NewComponent<iga::SpriteComponent>();
									if (std::shared_ptr<iga::SpriteComponent> sprite = sprite_comp.lock()) {
										sprite->SetTexture("res/node_add.png");
									}

									node_comp = entity->NewComponent<iga::tt::TerrainNodeComponent>();
									if (std::shared_ptr<iga::tt::TerrainNodeComponent> node = node_comp.lock()) {
										node->SetTerrainOutliner(terrain_->terrain_outiner_);
										node->SetIndex(1 + i);
										node->SetMovable(false);
									}
								}
								e_add_nodes_.push_back(entity_comp);
							}
						}
					}
					else {
						std::shared_ptr<Entity> entity;
						for (int i = 0; i < add_nodes_->size(); ++i)
						{
							if (entity = e_add_nodes_[i].lock()) {
								if (std::shared_ptr<iga::Transform> n_transform = entity->GetTransform().lock()) {
									Vector3 n_pos = Vector3(add_nodes_->at(i));
									n_pos.setZ(node_offset_);
									n_transform->SetPosition(n_pos);
								}
							}
						}
					}

					if (transform->dirty_mask_ & static_cast<uint8>(Transform::DirtyFlags::TERRAIN)) {
						terrain_->terrain_mesh_->GenerateMesh(is_closed_, mesh_nodes_);
					}
					//Edges
					if (edges_->size() != e_edges_.size()) {
						for (int i = 0; i < e_edges_.size(); ++i)
						{
							scene_.lock()->DestroyEntity(e_edges_[i]);
						}
						e_edges_.clear();
						std::weak_ptr<Entity> entity_comp;
						std::shared_ptr<Entity> entity;
						std::weak_ptr<iga::tt::TerrainEdgeComponent> edge_comp;
						if (e_edges_.empty()) {
							for (int i = 0; i < edges_->size(); ++i)
							{
								entity_comp = scene_.lock()->CreateEntity();
								if (entity = entity_comp.lock()) {
									entity->SetName("edge_" + std::to_string(i));

									if (std::shared_ptr<iga::Transform> n_transform = entity->GetTransform().lock()) {
										transform->AddChild(n_transform);
										n_transform->SetPosition(edges_->at(i)->pos);
										n_transform->SetRotation(edges_->at(i)->angle* (180 / float(M_PI)));
									}

									edge_comp = entity->NewComponent<iga::tt::TerrainEdgeComponent>();
									if (std::shared_ptr<iga::tt::TerrainEdgeComponent> edge = edge_comp.lock()) {
										switch (edges_->at(i)->type)
										{
										case EdgeType::TOP:
											edge->SetTexture(top_texture_path_);
											break;
										case EdgeType::SIDE:
											edge->SetTexture(side_texture_path_);
											break;
										case EdgeType::BOTTOM:
											edge->SetTexture(bot_texture_path_);
											break;
										}
										edge->SetMesh(edges_->at(i)->mesh);
									}
								}
								e_edges_.push_back(entity_comp);
							}
						}
					}
					else {
						std::shared_ptr<Entity> entity;
						for (int i = 0; i < edges_->size(); ++i)
						{
							if (entity = e_edges_[i].lock()) {
								if (std::shared_ptr<iga::Transform> n_transform = entity->GetTransform().lock()) {
									n_transform->SetPosition(edges_->at(i)->pos);
									n_transform->SetRotation(edges_->at(i)->angle* (180.0f / float(M_PI)));
								}
								if (std::shared_ptr<iga::tt::TerrainEdgeComponent> edge = entity->GetComponent<iga::tt::TerrainEdgeComponent>().lock()) {
									switch (edges_->at(i)->type)
									{
									case EdgeType::TOP:
										edge->SetTexture(top_texture_path_);
										break;
									case EdgeType::SIDE:
										edge->SetTexture(side_texture_path_);
										break;
									case EdgeType::BOTTOM:
										edge->SetTexture(bot_texture_path_);
										break;
									}
									edge->SetType(edges_->at(i)->type);
									edge->SetMesh(edges_->at(i)->mesh);
								}
							}
						}
					}
					//Set mesh
					fill_mesh_resource_ = GetResourceManager()->LoadGeneratedMesh(new Mesh(*terrain_->terrain_mesh_->GetMesh()));

#ifdef EDITOR
					//Draw debug
					if (draw_edges && !GetGame()->IsPlaying()) {
						terrain_->terrain_debugger_->DrawDebug(transform->GetPosition());
					}
#endif //EDITOR

					//Reset modified
					terrain_->terrain_outiner_->SetActivePoint(-1);
					terrain_->terrain_outiner_->ResetModified();
				}
#endif // EDITOR

				transform->dirty_mask_ &= ~(static_cast<uint8>(Transform::DirtyFlags::TERRAIN));
			}
		}

		void TerrainComponent::PostUpdate()
		{
		}

		void TerrainComponent::SetTexture(const EdgeType a_type, const std::string & a_path)
		{
			switch (a_type)
			{
			case EdgeType::FILL:
				fill_texture_resource_ = GetResourceManager()->Load<Texture>(a_path);
				fill_texture_path_ = a_path;
				break;
			case EdgeType::TOP:
				top_texture_path_ = a_path;
				break;
			case EdgeType::SIDE:
				side_texture_path_ = a_path;
				break;
			case EdgeType::BOTTOM:
				bot_texture_path_ = a_path;
				break;
			}
			if (a_type != EdgeType::FILL) {
				std::shared_ptr<Entity> entity;
				for (int i = 0; i < e_edges_.size(); ++i)
				{
					if (entity = e_edges_[i].lock()) {
						if (std::shared_ptr<iga::tt::TerrainEdgeComponent> edge = entity->GetComponent<iga::tt::TerrainEdgeComponent>().lock()) {
							if (edge->GetType() == a_type) {
								switch (a_type)
								{
								case EdgeType::TOP:
									edge->SetTexture(top_texture_path_);
									break;
								case EdgeType::SIDE:
									edge->SetTexture(side_texture_path_);
									break;
								case EdgeType::BOTTOM:
									edge->SetTexture(bot_texture_path_);
									break;
								}
							}
						}
					}
				}
			}
#ifdef EDITOR
			char buffer[256] = "";
			const std::string& name = a_path;
			size_t name_length = name.length();
			switch (a_type)
			{
			case EdgeType::FILL:
				for (size_t i = 0; i < 256; ++i) {
					if (i < name_length) {
						texture_path_buffer_[i] = name.c_str()[i];
					}
					else {
						texture_path_buffer_[i] = buffer[i];
					}
				}
				break;
			case EdgeType::TOP:
				for (size_t i = 0; i < 256; ++i) {
					if (i < name_length) {
						texture_top_path_buffer_[i] = name.c_str()[i];
					}
					else {
						texture_top_path_buffer_[i] = buffer[i];
					}
				}
				break;
			case EdgeType::SIDE:
				for (size_t i = 0; i < 256; ++i) {
					if (i < name_length) {
						texture_side_path_buffer_[i] = name.c_str()[i];
					}
					else {
						texture_side_path_buffer_[i] = buffer[i];
					}
				}
				break;
			case EdgeType::BOTTOM:
				for (size_t i = 0; i < 256; ++i) {
					if (i < name_length) {
						texture_bot_path_buffer_[i] = name.c_str()[i];
					}
					else {
						texture_bot_path_buffer_[i] = buffer[i];
					}
				}
				break;
			}
#endif
		}

		RenderDataStruct* TerrainComponent::OnBuildRenderData()
		{
			if(visible_) {
#ifdef EDITOR
				if(!visible_editor_only_ || (visible_editor_only_ && !GetGame()->IsPlaying())) {
#endif
					RenderDataStruct *rds = new RenderDataStruct();
					rds->mesh_ = GetResourceManager()->Get<Mesh>(fill_mesh_resource_);
					rds->model_matrix_ = owner_.lock()->GetTransform().lock()->GetTransform();
					rds->modifier_ = modifiers_;
					rds->texture_ = GetResourceManager()->Get<Texture>(fill_texture_resource_);
#ifdef EDITOR
					CheckSetSelectedModifiers(GetOwner(), rds);
#endif

					// before return rds // sometimes rds is also called render_data
					// obviously dont include this comment.
					return rds;
#ifdef EDITOR
				}
#endif
			}
			return nullptr;
		}

		void TerrainComponent::Inspect()
		{
#ifdef EDITOR
			bool collider_enabled = terrain_->terrain_collider_->GetEnabled();
			if (ImGui::Checkbox("Collision enabled", &collider_enabled)) {
				terrain_->terrain_collider_->SetEnabled(collider_enabled);
			}

			is_closed_ = terrain_->terrain_outiner_->GetIsClosed();
			if (ImGui::Checkbox("Is terrain closed", &is_closed_)) {
				terrain_->terrain_outiner_->SetIsClosed(is_closed_);
			}

			float texture_scale = terrain_->terrain_mesh_->GetTextureScale();
			ImGui::DragFloat("Texture scale", &texture_scale, 0.1f, 0.5f, 100.f);
			if (texture_scale != terrain_->terrain_mesh_->GetTextureScale()) {
				terrain_->terrain_mesh_->SetTextureScale(texture_scale);
			}

			ImGui::Text("fill tex");
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;

			ImGui::SameLine(0.f, 0.f);
			ImGui::Text(":");
			ImGui::SameLine(0.f, 10.f);

			if (ImGui::InputText("TexturePath", texture_path_buffer_, IM_ARRAYSIZE(texture_path_buffer_), input_text_flags)) {
				SetTexture(EdgeType::FILL, texture_path_buffer_);
			}

			ImGui::Text("top tex");
			ImGui::SameLine(0.f, 0.f);
			ImGui::Text(":");
			ImGui::SameLine(0.f, 10.f);

			if (ImGui::InputText("TopTexturePath", texture_top_path_buffer_, IM_ARRAYSIZE(texture_top_path_buffer_), input_text_flags)) {
				SetTexture(EdgeType::TOP, texture_top_path_buffer_);
			}

			ImGui::Text("side tex");

			ImGui::SameLine(0.f, 0.f);
			ImGui::Text(":");
			ImGui::SameLine(0.f, 10.f);

			if (ImGui::InputText("SideTexturePath", texture_side_path_buffer_, IM_ARRAYSIZE(texture_side_path_buffer_), input_text_flags)) {
				SetTexture(EdgeType::SIDE, texture_side_path_buffer_);
			}

			ImGui::Text("bot tex");

			ImGui::SameLine(0.f, 0.f);
			ImGui::Text(":");
			ImGui::SameLine(0.f, 10.f);

			if (ImGui::InputText("BotTexturePath", texture_bot_path_buffer_, IM_ARRAYSIZE(texture_bot_path_buffer_), input_text_flags)) {
				SetTexture(EdgeType::BOTTOM, texture_bot_path_buffer_);
			}
#endif // EDITOR
		}
	}
}
