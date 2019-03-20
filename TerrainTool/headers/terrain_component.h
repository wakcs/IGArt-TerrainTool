#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include <Graphics/base_rendering_component.h>
#include <Resource/resource.h>

namespace iga
{
	class Scene; 

	namespace tt
	{
		class TerrainBuilder;
		struct EdgeData;
		enum class EdgeType;
		class TerrainComponent : public BaseRenderingComponent
		{
			friend class MenuBar;
		public:
			/**
			*   Cereal versions of iga::tt::TerrainComponent
			*/
			enum class Version {
				DEFAULT = 0, /**< Default normal version. */
				LATEST /**< Latest version. */
			};
			/**
			* Constructor.
			*/
			TerrainComponent();

			/**
			* Constructor.
			*/
			TerrainComponent(std::weak_ptr<Entity> a_entity);
			/**
			* Deconstructor.
			*/
			~TerrainComponent();

			/**
			* Inherited via Component.
		    *  @param a_on_load Wether the component is loaded by a file or created in run-time. True is loaded by a file. False (default value) is created in run-time.
			*/
			void OnCreate(bool a_on_load = false) override;
			/**
			* Inherited via Component.
			*/
			void Update() override;
			/**
			* Inherited via Component.
			*/
			void PostUpdate() override;

			/**
			*  Function to load a texture for the fill mesh from a path.
			*  @param a_type A const EdgeType defining the type.
			*  @param a_path A const std::string reference that contains the path to the texture.
			*/
			void SetTexture(const EdgeType a_type, const std::string& a_path);

			/**
			* Virtual function that gets render data when it wants to build render data.
			*/
			virtual RenderDataStruct* OnBuildRenderData() override;

			/**
			* Inherited via Component.
			*/
			void Inspect() override;

			template<class Archive>
			void serialize(Archive & a_archive, uint32 const a_version);

			ModifierStruct modifiers_; /**< Modifier struct to modify the sprite with. @see iga::ModifierStruct */
		private:
			tt::TerrainBuilder * terrain_ = nullptr;			/** < The terrain it creates. */
			std::weak_ptr<Scene> scene_;						/** < Pointer to scene, to create node entities. */
			std::vector<std::weak_ptr<Entity>> e_mesh_nodes_;	/** < List of all the movable node entities. */
			std::vector<std::weak_ptr<Entity>> e_add_nodes_;	/** < List of all the addable node entities. */
			std::vector<std::weak_ptr<Entity>> e_edges_;		/** < List of all the addable node entities. */

			std::shared_ptr<Resource> fill_texture_resource_;	/**< The texture resource of the fill mesh. @see iga::Resource @see iga::Texture */
			std::shared_ptr<Resource> fill_mesh_resource_;		/**< The mesh resource of the fill mesh. @see iga::Resource @see iga::Texture */

			std::string fill_texture_path_ = "Default";				/** < Path of the fill texture. */
			std::string top_texture_path_ = "res/top_edge.png";		/** < Path of the top edge texture. */
			std::string side_texture_path_ = "res/side_edge.png";	/** < Path of the side edge texture. */
			std::string bot_texture_path_ = "res/bot_edge.png";		/** < Path of the bottom edge texture. */
			std::shared_ptr<std::vector<Vector3>> mesh_nodes_;		/** < Pointer to the vector of mesh nodes. */
			std::shared_ptr<std::vector<Vector3>> add_nodes_;		/** < Pointer to the vector of add nodes. */
			std::shared_ptr<std::vector<EdgeData*>> edges_;			/** < Pointer to the vector of edges. */
			bool is_closed_;
#ifdef EDITOR
			char texture_path_buffer_[256] = " ";		/**< Editor only variable. A char buffer for the fill texture path text box. */
			char texture_top_path_buffer_[256] = " ";	/**< Editor only variable. A char buffer for the top texture path text box. */
			char texture_side_path_buffer_[256] = " ";	/**< Editor only variable. A char buffer for the side texture path text box. */
			char texture_bot_path_buffer_[256] = " ";	/**< Editor only variable. A char buffer for the bot texture path text box. */
			float node_offset_ = 0.5f;					/**< Offset so that the nodes are always on top. */
			static bool draw_edges;						/**< Used to specify if the edges should be drawn. */
#endif // EDITOR

		};
		SUBSCRIBECOMPONENT(TerrainComponent)

			template<class Archive>
		inline void tt::TerrainComponent::serialize(Archive & a_archive, uint32 const a_version) {
			switch(static_cast<Version>(a_version)) {
				case Version::LATEST:
				case Version::DEFAULT:
					a_archive(cereal::base_class<iga::BaseRenderingComponent>(this));
					//TODO: serialize terrain
					a_archive(cereal::make_nvp("terrain_nodes", mesh_nodes_),
							  cereal::make_nvp("terrain_closed", is_closed_),
							  cereal::make_nvp("terrain_fill_mesh", fill_mesh_resource_),
							  cereal::make_nvp("terrain_fill_path", fill_texture_path_),
							  cereal::make_nvp("terrain_top_path", top_texture_path_),
							  cereal::make_nvp("terrain_side_path", side_texture_path_),
							  cereal::make_nvp("terrain_bot_path", bot_texture_path_),
							  cereal::make_nvp("terrain_nodes_entities", e_mesh_nodes_),
							  cereal::make_nvp("terrain_add_entities", e_add_nodes_),
							  cereal::make_nvp("terrain_edge_entities", e_edges_)
					);
					break;
			}
			
		}
	} // End of namespace ~ tt
} // End of namespace ~ iga

CEREAL_CLASS_VERSION(iga::tt::TerrainComponent, (static_cast<iga::uint32>(iga::tt::TerrainComponent::Version::LATEST) - 1));
