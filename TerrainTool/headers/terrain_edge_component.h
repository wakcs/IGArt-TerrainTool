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
	namespace tt
	{
		enum class EdgeType;
		class TerrainEdgeComponent : public BaseRenderingComponent
		{
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
			TerrainEdgeComponent();

			/**
			* Constructor.
			*/
			TerrainEdgeComponent(std::weak_ptr<Entity> a_entity);
			/**
			* Deconstructor.
			*/
			~TerrainEdgeComponent();

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
			*  Function to load a texture for the mesh from a path.
			*  @param a_path A const std::string reference that contains the path to the texture.
			*/
			void SetTexture(const std::string& a_path);
			/**
			*  Function to load the mesh for this edge
			*  @param a_path A mesh reference that contains the mesh data.
			*/
			void SetMesh(Mesh* a_mesh);
			/**
			*  Function to set the edge type.
			*  @param a_type The type of the edge.
			*/
			void SetType(EdgeType a_type);

			/**
			*  @return the edge type.
			*/
			EdgeType GetType();

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

		private:
			ModifierStruct modifiers_; /**< Modifier struct to modify the sprite with. @see iga::ModifierStruct */
			std::shared_ptr<Resource> edge_texture_resource_;	/**< The texture resource of the mesh. @see iga::Resource @see iga::Texture */
			std::shared_ptr<Resource> edge_mesh_resource_;		/**< The mesh resource of the mesh. @see iga::Resource @see iga::Texture */
			Mesh* edge_mesh_;									/**< The mesh data, that holds the mesh info for the resource to load. */
			EdgeType edge_type_;								/**< The type of this edge. */
		};
	} // End of namespace ~ tt
	template<class Archive>
	inline void tt::TerrainEdgeComponent::serialize(Archive & a_archive, uint32 const a_version) {
		switch (static_cast<Version>(a_version)) {
		case Version::LATEST:
		case Version::DEFAULT:
			a_archive(cereal::base_class<Component>(this));
			a_archive(	cereal::make_nvp("edge_mesh_modifiers", modifiers_),
						cereal::make_nvp("edge_texture_resource", edge_texture_resource_),
						cereal::make_nvp("edge_mesh_resource", edge_mesh_resource_),
						cereal::make_nvp("edge_type", edge_type_)
			);
			break;
		}
	}
} // End of namespace ~ iga

CEREAL_CLASS_VERSION(iga::tt::TerrainEdgeComponent, (static_cast<iga::uint32>(iga::tt::TerrainEdgeComponent::Version::LATEST) - 1));
