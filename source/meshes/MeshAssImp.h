#pragma once

#include <assimp/matrix4x4.h>
#include "includes/Vertex.h"
#include "interfaces/IMesh.h"

// Forward declarations

struct aiMesh;
struct aiNode;
struct aiScene;

// This class represents a mesh loaded using the AssImp importer.
// It is responsible for loading the mesh data, and organizing
// it in a way that is accessible to OpenGL.

class MeshAssImp : public IMesh
{
	public:
		MeshAssImp(std::string & newMeshPath) noexcept;

		MeshAssImp(std::string && newMeshPath) noexcept;

		MeshAssImp(const IMesh & copy) noexcept;

		IMesh & operator= (const IMesh & copy) noexcept;

		MeshAssImp(IMesh && move) noexcept;

		IMesh & operator= (IMesh && move) noexcept;

		~MeshAssImp() noexcept;

		void draw() const noexcept override;

	private:
		// Create the resources and load the mesh data
		bool load();

		// Load a scene tree node and its childrens
		bool loadNode(aiNode & node, aiMatrix4x4 & parentTransform, const aiScene & scene);

		// Load data from a given mesh
		bool loadMesh(aiMesh & mesh);

		// Free the resources
		void unload();

		// Create default data as fallback
		void createDefault();

		// Initialize the GL buffers
		void glInitialize();

		// Finalize the GL buffers
		void glFinalize();
};