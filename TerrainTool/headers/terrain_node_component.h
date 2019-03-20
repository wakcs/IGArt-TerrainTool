#pragma once

#include <Core\Component.h>
#include <vectormath\vectormath.hpp>

#include <cereal/cereal.hpp>

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

namespace iga
{
	namespace tt
	{
		class TerrainOutliner;
		class TerrainNodeComponent : public Component
		{
		public:
			/**
			*   Cereal versions of iga::TerrainNodeComponent
			*/
			enum class Version {
				DEFAULT = 0, /**< Default normal version. */
				LATEST /**< Latest version. */
			};

			/**
			* Constructor.
			*/
			TerrainNodeComponent();
			/**
			* Constructor.
			* @param a_entity The entity that owns this component.
			*/
			TerrainNodeComponent(std::weak_ptr<Entity> a_entity);
			/**
			* Deconstructor.
			*/
			~TerrainNodeComponent();

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
			* Inherited via Component.
			*/
			void Inspect() override;

			/**
			* Inherited via Component.
			*/
			template<class Archive>
			void serialize(Archive & a_archive, uint32 const a_version);

			/**
			* Set the outliner this component points to
			* @param a_outliner The terrain outliner it will point to.
			*/
			void SetTerrainOutliner(TerrainOutliner * a_outliner);
			/**
			* Set the node index this entity will reperesent.
			* @param a_index The index of the node it will represent.
			*/
			void SetIndex(int a_index);
			/**
			* If set to true this node is a movable node,
			* else it's an addable node.
			* @param a_movable Boolean to toggle between movable or addable.
			*/
			void SetMovable(bool a_movable);
			/**
			* Sets the offset so that the nodes are always on top.
			* @param a_offset Float to set the offset
			*/
			void SetNodeOffset(float a_offset);

		private:
			TerrainOutliner * terrain_outliner_;	/** < Reference to the outliner. */
			Vector3 old_pos_;						/** < Position of this entity from previous update. */
			int node_index_;						/** < Index of the node this represents. */
			bool is_movable_ = false;				/** < bool if is a movable node. */
			float node_offset_ = 0.f;				/** < Offset that ensures the node is infront of the terrain. */
		};
	} // End of namespace ~ tt
	template<class Archive>
	inline void tt::TerrainNodeComponent::serialize(Archive & a_archive, uint32 const a_version) {
		switch(static_cast<Version>(a_version)) {
			case Version::LATEST:
			case Version::DEFAULT:
				a_archive(cereal::base_class<Component>(this));
				a_archive(cereal::make_nvp("node_index", node_index_),
						  cereal::make_nvp("node_movable", is_movable_)
				);
				break;
		}
	}
} // End of namespace ~ iga

CEREAL_CLASS_VERSION(iga::tt::TerrainNodeComponent, (static_cast<iga::uint32>(iga::tt::TerrainNodeComponent::Version::LATEST) - 1));
