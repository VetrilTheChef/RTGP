#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

// Forward declarations

class IMesh;
class IModel;
class IShaderProgram;

struct MVPN;
struct Lights;
struct Lambertian;

// The interface that Model Factory classes must implement

class IModelFactory
{
	public:
		virtual ~IModelFactory() noexcept {};

		// Create a static model
		virtual bool createStaticModel(std::shared_ptr<IModel> & model,
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
									   glm::vec3 scale) const noexcept = 0;

	protected:
		IModelFactory() {};

		// Disallowed - no need for 2 instances of the same model factory
		IModelFactory(const IModelFactory & copy) = delete;
		IModelFactory & operator= (const IModelFactory & copy) = delete;

		// Disallowed - no need to move a model factory
		IModelFactory(IModelFactory && move) = delete;
		IModelFactory & operator= (IModelFactory && move) = delete;
};