#pragma once

#include "interfaces/IModelFactory.h"

// This class represents a model factory.
// It is responsible for creating and initializing all types of model.

class ModelFactory : public IModelFactory
{
	public:
		ModelFactory() noexcept;

		~ModelFactory() noexcept;

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
									   glm::vec3 scale) const noexcept override;
};