#include "ModelFactory.h"
#include "meshes/MeshAssImp.h"
#include "models/Model.h"
#include "shaders/loaders/FileShaderLoader.h"
#include "shaders/programs/ShaderProgram.h"
#include "textures/FileTexture.h"

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

ModelFactory::ModelFactory() noexcept
{
}

ModelFactory::~ModelFactory() noexcept
{
}

bool ModelFactory::createStaticModel(std::shared_ptr<IModel> & model,
									 std::string meshPath,
									 std::string vertexShaderPath,
									 std::string fragmentShaderPath,
									 std::string albedoPath,
									 std::string normalsPath,
									 std::string roughnessPath,
									 MVPN & mvpn,
									 Lights & lights,
									 Lambertian & lambertian,
								     glm::vec3 position,
								     glm::vec3 rotation,
								     glm::vec3 scale) const noexcept
{
	// Create a concrete implementation of a Mesh
	unique_ptr<IMesh> mesh = make_unique<MeshAssImp>(meshPath);

	// Create the shader program
	shared_ptr<IShaderProgram> program = make_shared<ShaderProgram>(
		make_shared<FileShaderLoader>(vertexShaderPath),
		make_shared<FileShaderLoader>(fragmentShaderPath),
		make_shared<FileTexture>(albedoPath),
		make_shared<FileTexture>(normalsPath),
		make_shared<FileTexture>(roughnessPath),
		mvpn, lights, lambertian);

	model = make_shared<Model>(move(mesh), program, position, rotation, scale);

	// Return true if not null
	return model.get();
}
