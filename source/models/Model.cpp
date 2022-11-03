#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

Model::Model(std::unique_ptr<IMesh> newMesh,
			 std::shared_ptr<IShaderProgram> newProgram) noexcept:
	IModel(std::move(newMesh),
		   std::move(newProgram))
{
	// Compute the model matrix
	updateModelMatrix();
}

Model::Model(std::unique_ptr<IMesh> newMesh,
			 std::shared_ptr<IShaderProgram> newProgram,
			 glm::vec3 newPosition,
			 glm::vec3 newRotation,
			 glm::vec3 newScale) noexcept :
	IModel(std::move(newMesh),
		   std::move(newProgram)),
		   position(newPosition),
		   rotation(newRotation),
		   scale(newScale)
{
	// Compute the model matrix
	updateModelMatrix();
}

Model::Model(IModel && move) noexcept:
	IModel(std::move(move))
{
	// Move the data
	mesh = std::move(move.mesh);
	program = std::move(move.program);

	position = move.getPosition();
	rotation = move.getRotation();
	scale = move.getScale();

	// Update the model matrix
	updateModelMatrix();

	move.setPosition(vec3(0.0f));
	move.setRotation(vec3(0.0f));
	move.setScale(vec3(1.0f));
}

IModel & Model::operator= (IModel && move) noexcept
{
	// Move the data
	mesh = std::move(move.mesh);
	program = std::move(move.program);

	position = move.getPosition();
	rotation = move.getRotation();
	scale = move.getScale();

	// Update the model matrix
	updateModelMatrix();

	move.setPosition(vec3(0.0f));
	move.setRotation(vec3(0.0f));
	move.setScale(vec3(1.0f));

	return * this;
}

vec3 Model::getPosition() const noexcept
{
	return position;
}

void Model::setPosition(const vec3 newPosition) noexcept
{
	position = newPosition;

	// Update the model matrix
	updateModelMatrix();
}

vec3 Model::getRotation() const noexcept
{
	return rotation;
}

void Model::setRotation(const vec3 newRotation) noexcept
{
	rotation = newRotation;

	// Update the model matrix
	updateModelMatrix();
}

vec3 Model::getScale() const noexcept
{
	return scale;
}

void Model::setScale(const vec3 newScale) noexcept
{
	scale = newScale;

	// Update the model matrix
	updateModelMatrix();
}

Model::~Model() noexcept
{
}

mat4 Model::getModelMatrix() const noexcept
{
	return modelMatrix;
}

std::shared_ptr<IShaderProgram> Model::getProgram() const noexcept
{
	return program;
}

void Model::update(double deltaSeconds) noexcept
{
}

void Model::render() const noexcept
{
	if (mesh.get() && program.get())
	{
		// Activate the shader program
		program->activate();

		// Draw the mesh
		mesh->draw();

		// Deactivate the shader program
		program->deactivate();
	}
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED
///////////////////////////////////////////////////////////////////////////////

void Model::updateModelMatrix() noexcept
{
	// Precompute a model matrix and save it
	modelMatrix = glm::scale(mat4(1.0f), scale);

	modelMatrix = rotate(modelMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

	modelMatrix = translate(modelMatrix, position);
}