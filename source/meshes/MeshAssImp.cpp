#include "MeshAssImp.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <assimp/PostProcess.h>

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

MeshAssImp::MeshAssImp(std::string & newPath) noexcept:
	IMesh(newPath)
{
	if (!load())
	{
		cout << "Mesh: unable to load mesh from file: \"" << path << "\"."
			<< endl;

		createDefault();
	}

	glInitialize();
}

MeshAssImp::MeshAssImp(std::string && newPath) noexcept:
	IMesh(std::move(newPath))
{

	if (!load())
	{
		cout << "Mesh: unable to load mesh from file: \"" << path << "\"."
			<< endl;

		createDefault();
	}

	glInitialize();
}

MeshAssImp::MeshAssImp(const IMesh & copy) noexcept:
	IMesh(copy)
{
	// Copy the data
	path = copy.path;
	vertices = copy.vertices;
	indices = copy.indices;

	// Initialize the GL buffers
	glInitialize();
}

IMesh & MeshAssImp::operator= (const IMesh & copy) noexcept
{
	// Clean-up the existing data
	glFinalize();
	unload();

	// Copy the new data
	path = copy.path;
	vertices = copy.vertices;
	indices = copy.indices;

	// Reinitialize the GL buffers
	glInitialize();

	return * this;
}

MeshAssImp::MeshAssImp(IMesh && move) noexcept:
	IMesh(std::move(move))
{
	// Move only if the new data is valid
	if (move.VAO != 0)
	{
		path = std::move(move.path);
		vertices = std::move(move.vertices);
		indices = std::move(move.indices);

		VAO = move.VAO;
		VBO = move.VBO;
		EBO = move.EBO;

		// Invalidate the source buffer IDs
		move.VAO = 0;
		move.VBO = 0;
		move.EBO = 0;
	}
}

IMesh & MeshAssImp::operator= (IMesh && move) noexcept
{
	// Clean-up the existing data
	glFinalize();
	unload();

	// Move only if the new data is valid
	if (move.VAO != 0)
	{
		path = std::move(move.path);
		vertices = std::move(move.vertices);
		indices = std::move(move.indices);

		VAO = move.VAO;
		VBO = move.VBO;
		EBO = move.EBO;

		// Invalidate the source buffer IDs
		move.VAO = 0;
		move.VBO = 0;
		move.EBO = 0;
	}

	return * this;
}

MeshAssImp::~MeshAssImp() noexcept
{
	// Release the resources
	glFinalize();
	unload();
}

void MeshAssImp::draw() const noexcept
{
	// Bind the vertex array object
	glBindVertexArray(VAO);

	// Ready to draw
	glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the vertex array
	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED
///////////////////////////////////////////////////////////////////////////////

bool MeshAssImp::load()
{
	// Use Assimp's importer to load the data
	Assimp::Importer importer;

	// The scene contains all the data read from the provided file
	const aiScene * scene = importer.ReadFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate);

	// Ensure the scene is valid
	if (scene && scene->mRootNode &&
		!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
	{
		// Recursively iterate over the scene tree to load the data
		return loadNode(* (scene->mRootNode),
			scene->mRootNode->mTransformation,
			* scene);
	}

	// If the scene is somehow not usable, report an error
	cout << "Assimp: " << string(importer.GetErrorString()) << endl;

	return false;
}

void MeshAssImp::unload()
{
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////

bool MeshAssImp::loadNode(aiNode & node, aiMatrix4x4 & parentTransform,
	const aiScene & scene)
{
	// The result of the loading operation
	bool loadResult = true;

	// The node's transform matrix (relative to the world)
	aiMatrix4x4 nodeTransform = parentTransform * node.mTransformation;

	int numMeshes = node.mNumMeshes;

	// Iterate over the node's meshes
	for (int numMesh = 0; numMesh < numMeshes; numMesh++)
	{
		aiMesh * mesh = scene.mMeshes[node.mMeshes[numMesh]];

		// Only if the mesh is not null
		if (mesh)
		{
			// Load the mesh and accumulate the loading result
			loadResult = loadResult && loadMesh(* mesh);
		}
		else
		{
			// Mesh is null, log and report the problem
			cout << "Mesh: unable to access node's mesh " << numMesh
				<< " as it is null." << endl;

			loadResult = false;
		}
	}

	int numChildren = node.mNumChildren;

	// Iterate over the node's children
	for (int numChild = 0; numChild < numChildren; numChild++)
	{
		aiNode * child = node.mChildren[numChild];

		// Only if the child is not null
		if (child)
		{
			// Recursively load the node's children and accumulate their
			// loading results
			loadResult = loadResult && loadNode(* child, nodeTransform, scene);
		}
		else
		{
			// Child is null, log and report the problem
			 cout << "Mesh: unable to access node's child" << numChild
				<< " as it is null." << endl;

			loadResult = false;
		}
	}

	return loadResult;
}

bool MeshAssImp::loadMesh(aiMesh & mesh)
{
	bool loadResult = true;

	// Keep this as GetNumUVChannels has O(n) complexity
	int numUVChannels = mesh.GetNumUVChannels();

	// Iterate over the mesh vertices
	for (unsigned int nVertex = 0; nVertex < mesh.mNumVertices; nVertex++)
	{
		// Create a new vertex to copy the mesh's data
		Vertex vertex;

		// Store the position
		vertex.Position = vec3(mesh.mVertices[nVertex].x,
									mesh.mVertices[nVertex].y,
									mesh.mVertices[nVertex].z);

		// Iterate over the UV channels
		for (int uvChannel = 0; uvChannel < numUVChannels; uvChannel++)
		{
			vec2 UV;

			// If the mesh has UVs for that channel, copy the values
			if (mesh.HasTextureCoords(uvChannel))
			{
				UV.x = mesh.mTextureCoords[uvChannel][nVertex].x;
				UV.y = mesh.mTextureCoords[uvChannel][nVertex].y;
			}

			// Store the UVs
			vertex.TexCoords = UV;
		}

		// Sanity check: vertex's TexCoords vector length must be
		// equal to the declared number of UV channels
		//if (vertex.TexCoords.size() != numUVChannels)
		//{
			// The mesh should have UVs,
			// log and report the problem
			//cout << "Mesh: mesh" << mesh.mName.C_Str() << " has "
			//	<< numUVChannels << ", but only " << vertex.TexCoords.size()
			//	<< " channels were found." << endl;

			//loadResult = false;
		//}

		// If the mesh has normals, copy the values
		if (mesh.HasNormals())
		{
			// Store the normal
			vertex.Normal = vec3(mesh.mNormals[nVertex].x,
				mesh.mNormals[nVertex].y,
				mesh.mNormals[nVertex].z);
		}
		else
		{
			// The mesh should have normals (due to the importer's flags),
			// log and report the problem
			cout << "Mesh: mesh \"" << mesh.mName.C_Str()
				<< "\" has no normals." << endl;

			loadResult = false;
		}

		// If the mesh has tangents and bitangents, copy the values
		if (mesh.HasTangentsAndBitangents())
		{
			// Store the tanget
			vertex.Tangent = vec3(mesh.mTangents[nVertex].x,
				mesh.mTangents[nVertex].y,
				mesh.mTangents[nVertex].z);

			// Store the bitanget
			vertex.Bitangent = vec3(mesh.mBitangents[nVertex].x,
				mesh.mBitangents[nVertex].y,
				mesh.mBitangents[nVertex].z);
		}
		else
		{
			// The mesh should have both tangents and bitangents
			// (due to the importer's flags),
			// log and report the problem
			cout << "Mesh: mesh \"" << mesh.mName.C_Str()
				<< "\" has no tangents / bitangents." << endl;

			loadResult = false;
		}

		// Add the new vertex to the instance's vertices vector
		vertices.emplace_back(vertex);
	}

	// Only if the mesh has faces
	if (mesh.HasFaces())
	{
		// Iterate over the mesh faces
		for (unsigned int nFace = 0; nFace < mesh.mNumFaces; nFace++)
		{
			aiFace & face = mesh.mFaces[nFace];

			// Sanity check: the face should have 3 indices
			// (due to the importer's triangulation)

			if (face.mNumIndices != 3)
			{
				// The face should describe a triangle,
				// log and report the problem
				cout << "Mesh: face has " << face.mNumIndices << " vertices."
					<< endl;

				loadResult = false;
			}

			// Iterate over the face indices
			for (unsigned int nIndex = 0; nIndex < face.mNumIndices; nIndex++)
			{
				// Add the index to the instance's indices vector
				indices.emplace_back(GLuint(face.mIndices[nIndex]));
			}
		}
	}
	else
	{
		// The mesh should have faces (due to the importer),
		// log and report the problem
		cout << "Mesh: mesh \"" << mesh.mName.C_Str() << "\" has no faces" << endl;

		loadResult = false;
	}

	return loadResult;
}

void MeshAssImp::createDefault()
{
	// This method creates a default mesh (a cube) as a fallback in case the
	// mesh could not be loaded from the provided path
	// This ensures that there will always be valid data to pass to GL

	cout << "Mesh: using default." << endl;

	// Create the vertices
	vertices.resize(24);
	fill(vertices.begin(), vertices.end(), Vertex());
	
	// Face 0
	vertices[0].Position = vec3(-1.0f, -1.0f,  1.0f);
	vertices[1].Position = vec3(-1.0f,  1.0f,  1.0f);
	vertices[2].Position = vec3( 1.0f,  1.0f,  1.0f);
	vertices[3].Position = vec3( 1.0f, -1.0f,  1.0f);

	vertices[0].Normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[1].Normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[2].Normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[3].Normal = vec3(0.0f, 0.0f, 1.0f);

	vertices[0].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[1].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[2].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[3].Tangent = vec3(1.0f, 0.0f, 0.0f);

	vertices[0].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[1].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[2].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[3].Bitangent = vec3(0.0f, 1.0f, 0.0f);

	vertices[0].TexCoords = vec2(0.0f, 0.0f);
	vertices[1].TexCoords = vec2(0.0f, 1.0f);
	vertices[2].TexCoords = vec2(1.0f, 1.0f);
	vertices[3].TexCoords = vec2(1.0f, 0.0f);

	// Face 1
	vertices[4].Position = vec3(-1.0f, -1.0f, -1.0f);
	vertices[5].Position = vec3(-1.0f,  1.0f, -1.0f);
	vertices[6].Position = vec3(-1.0f,  1.0f,  1.0f);
	vertices[7].Position = vec3(-1.0f, -1.0f,  1.0f);

	vertices[4].Normal = vec3(-1.0f, 0.0f, 0.0f);
	vertices[5].Normal = vec3(-1.0f, 0.0f, 0.0f);
	vertices[6].Normal = vec3(-1.0f, 0.0f, 0.0f);
	vertices[7].Normal = vec3(-1.0f, 0.0f, 0.0f);

	vertices[4].Tangent = vec3(0.0f, 0.0f, 1.0f);
	vertices[5].Tangent = vec3(0.0f, 0.0f, 1.0f);
	vertices[6].Tangent = vec3(0.0f, 0.0f, 1.0f);
	vertices[7].Tangent = vec3(0.0f, 0.0f, 1.0f);

	vertices[4].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[5].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[6].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[7].Bitangent = vec3(0.0f, 1.0f, 0.0f);

	vertices[4].TexCoords = vec2(0.0f, 0.0f);
	vertices[5].TexCoords = vec2(0.0f, 1.0f);
	vertices[6].TexCoords = vec2(1.0f, 1.0f);
	vertices[7].TexCoords = vec2(1.0f, 0.0f);

	// Face 2
	vertices[8].Position  = vec3( 1.0f, -1.0f, -1.0f);
	vertices[9].Position  = vec3( 1.0f,  1.0f, -1.0f);
	vertices[10].Position = vec3(-1.0f,  1.0f, -1.0f);
	vertices[11].Position = vec3(-1.0f, -1.0f, -1.0f);

	vertices[8].Normal  = vec3(0.0f, 0.0f, -1.0f);
	vertices[9].Normal  = vec3(0.0f, 0.0f, -1.0f);
	vertices[10].Normal = vec3(0.0f, 0.0f, -1.0f);
	vertices[11].Normal = vec3(0.0f, 0.0f, -1.0f);

	vertices[8].Tangent  = vec3(-1.0f, 0.0f, 0.0f);
	vertices[9].Tangent  = vec3(-1.0f, 0.0f, 0.0f);
	vertices[10].Tangent = vec3(-1.0f, 0.0f, 0.0f);
	vertices[11].Tangent = vec3(-1.0f, 0.0f, 0.0f);

	vertices[8].Bitangent  = vec3(0.0f, 1.0f, 0.0f);
	vertices[9].Bitangent  = vec3(0.0f, 1.0f, 0.0f);
	vertices[10].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[11].Bitangent = vec3(0.0f, 1.0f, 0.0f);

	vertices[8].TexCoords = vec2(0.0f, 0.0f);
	vertices[9].TexCoords = vec2(0.0f, 1.0f);
	vertices[10].TexCoords = vec2(1.0f, 1.0f);
	vertices[11].TexCoords = vec2(1.0f, 0.0f);

	// Face 3
	vertices[12].Position = vec3( 1.0f, -1.0f,  1.0f);
	vertices[13].Position = vec3( 1.0f,  1.0f,  1.0f);
	vertices[14].Position = vec3( 1.0f,  1.0f, -1.0f);
	vertices[15].Position = vec3( 1.0f, -1.0f, -1.0f);

	vertices[12].Normal = vec3(1.0f, 0.0f, 0.0f);
	vertices[13].Normal = vec3(1.0f, 0.0f, 0.0f);
	vertices[14].Normal = vec3(1.0f, 0.0f, 0.0f);
	vertices[15].Normal = vec3(1.0f, 0.0f, 0.0f);

	vertices[12].Tangent = vec3(0.0f, 0.0f, -1.0f);
	vertices[13].Tangent = vec3(0.0f, 0.0f, -1.0f);
	vertices[14].Tangent = vec3(0.0f, 0.0f, -1.0f);
	vertices[15].Tangent = vec3(0.0f, 0.0f, -1.0f);

	vertices[12].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[13].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[14].Bitangent = vec3(0.0f, 1.0f, 0.0f);
	vertices[15].Bitangent = vec3(0.0f, 1.0f, 0.0f);

	vertices[12].TexCoords = vec2(0.0f, 0.0f);
	vertices[13].TexCoords = vec2(0.0f, 1.0f);
	vertices[14].TexCoords = vec2(1.0f, 1.0f);
	vertices[15].TexCoords = vec2(1.0f, 0.0f);

	// Face 4
	vertices[16].Position = vec3(-1.0f,  1.0f,  1.0f);
	vertices[17].Position = vec3(-1.0f,  1.0f, -1.0f);
	vertices[18].Position = vec3( 1.0f,  1.0f, -1.0f);
	vertices[19].Position = vec3( 1.0f,  1.0f,  1.0f);

	vertices[16].Normal = vec3(0.0f, 1.0f, 0.0f);
	vertices[17].Normal = vec3(0.0f, 1.0f, 0.0f);
	vertices[18].Normal = vec3(0.0f, 1.0f, 0.0f);
	vertices[19].Normal = vec3(0.0f, 1.0f, 0.0f);

	vertices[16].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[17].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[18].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[19].Tangent = vec3(1.0f, 0.0f, 0.0f);

	vertices[16].Bitangent = vec3(0.0f, 0.0f, -1.0f);
	vertices[17].Bitangent = vec3(0.0f, 0.0f, -1.0f);
	vertices[18].Bitangent = vec3(0.0f, 0.0f, -1.0f);
	vertices[19].Bitangent = vec3(0.0f, 0.0f, -1.0f);

	vertices[16].TexCoords = vec2(0.0f, 0.0f);
	vertices[17].TexCoords = vec2(0.0f, 1.0f);
	vertices[18].TexCoords = vec2(1.0f, 1.0f);
	vertices[19].TexCoords = vec2(1.0f, 0.0f);

	// Face 5
	vertices[20].Position = vec3(-1.0f, -1.0f, -1.0f);
	vertices[21].Position = vec3(-1.0f, -1.0f,  1.0f);
	vertices[22].Position = vec3( 1.0f, -1.0f,  1.0f);
	vertices[23].Position = vec3( 1.0f, -1.0f, -1.0f);

	vertices[20].Normal = vec3(0.0f, -1.0f, 0.0f);
	vertices[21].Normal = vec3(0.0f, -1.0f, 0.0f);
	vertices[22].Normal = vec3(0.0f, -1.0f, 0.0f);
	vertices[23].Normal = vec3(0.0f, -1.0f, 0.0f);

	vertices[20].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[21].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[22].Tangent = vec3(1.0f, 0.0f, 0.0f);
	vertices[23].Tangent = vec3(1.0f, 0.0f, 0.0f);

	vertices[20].Bitangent = vec3(0.0f, 0.0f, 1.0f);
	vertices[21].Bitangent = vec3(0.0f, 0.0f, 1.0f);
	vertices[22].Bitangent = vec3(0.0f, 0.0f, 1.0f);
	vertices[23].Bitangent = vec3(0.0f, 0.0f, 1.0f);

	vertices[20].TexCoords = vec2(0.0f, 0.0f);
	vertices[21].TexCoords = vec2(0.0f, 1.0f);
	vertices[22].TexCoords = vec2(1.0f, 1.0f);
	vertices[23].TexCoords = vec2(1.0f, 0.0f);

	// Create the indices
	
	// Face 0
	indices.emplace_back(GLuint(0));
	indices.emplace_back(GLuint(1));
	indices.emplace_back(GLuint(2));

	indices.emplace_back(GLuint(0));
	indices.emplace_back(GLuint(2));
	indices.emplace_back(GLuint(3));

	// Face 1
	indices.emplace_back(GLuint(4));
	indices.emplace_back(GLuint(5));
	indices.emplace_back(GLuint(6));

	indices.emplace_back(GLuint(4));
	indices.emplace_back(GLuint(6));
	indices.emplace_back(GLuint(7));

	// Face 2
	indices.emplace_back(GLuint(8));
	indices.emplace_back(GLuint(9));
	indices.emplace_back(GLuint(10));

	indices.emplace_back(GLuint(8));
	indices.emplace_back(GLuint(10));
	indices.emplace_back(GLuint(11));

	// Face 3
	indices.emplace_back(GLuint(12));
	indices.emplace_back(GLuint(13));
	indices.emplace_back(GLuint(14));

	indices.emplace_back(GLuint(12));
	indices.emplace_back(GLuint(14));
	indices.emplace_back(GLuint(15));

	// Face 4
	indices.emplace_back(GLuint(16));
	indices.emplace_back(GLuint(17));
	indices.emplace_back(GLuint(18));

	indices.emplace_back(GLuint(16));
	indices.emplace_back(GLuint(18));
	indices.emplace_back(GLuint(19));

	// Face 5
	indices.emplace_back(GLuint(20));
	indices.emplace_back(GLuint(21));
	indices.emplace_back(GLuint(22));

	indices.emplace_back(GLuint(20));
	indices.emplace_back(GLuint(22));
	indices.emplace_back(GLuint(23));
}

void MeshAssImp::glInitialize()
{
	glGenVertexArrays(1, & VAO);
	glGenBuffers(1, & VBO);
	glGenBuffers(1, & EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), & vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), & indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, Position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, Bitangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void MeshAssImp::glFinalize()
{
	glDeleteVertexArrays(1, & VAO);
	glDeleteBuffers(1, & VBO);
	glDeleteBuffers(1, & EBO);

	VAO = 0;
	VBO = 0;
	EBO = 0;
}