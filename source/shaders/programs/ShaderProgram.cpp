#include "ShaderProgram.h"
#include <iostream>
#include "shaders/buffers/UniformBufferObject.h"
#include "shaders/loaders/interfaces/IShaderLoader.h"
#include "textures/interfaces/ITexture.h"

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

ShaderProgram::ShaderProgram(shared_ptr<IShaderLoader> newVertexShaderLoader,
							 shared_ptr<IShaderLoader> newFragmentShaderLoader,
							 std::shared_ptr<ITexture> newAlbedoMap,
							 std::shared_ptr<ITexture> newNormalsMap,
							 std::shared_ptr<ITexture> newRoughnessMap,
							 const MVPN & newMvpn,
							 const Lights & newLights,
							 const Lambertian & newLambertian)
	noexcept :
	IShaderProgram(newVertexShaderLoader,
				   newFragmentShaderLoader,
				   newAlbedoMap,
				   newNormalsMap,
				   newRoughnessMap,
				   newMvpn,
				   newLights,
				   newLambertian)
{
	vertexShaderLoader = newVertexShaderLoader;
	fragmentShaderLoader = newFragmentShaderLoader;

	albedoMap = newAlbedoMap;
	normalsMap = newNormalsMap;
	roughnessMap = newRoughnessMap;

	mvpn = & newMvpn;
	lights = & newLights;
	lambertian = & newLambertian;

	GLint vertexShaderId = 0;
	GLint fragmentShaderId = 0;

	if (createShaders(vertexShaderId, fragmentShaderId))
	{
		if (createProgram(vertexShaderId, fragmentShaderId))
		{
			createUBOs();
			createTextures();
		}
	}

	deleteShaders(vertexShaderId, fragmentShaderId);
}

ShaderProgram::~ShaderProgram() noexcept
{
	vertexShaderLoader.reset();
	fragmentShaderLoader.reset();

	deleteTextures();
	deleteUBOs();
	deleteProgram();
}

void ShaderProgram::activate() noexcept
{
	// Set the active shader program
	glUseProgram(id);

	// Activate the textures
	if (albedoMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + ALBEDO_TEXTURE_INDEX);
		albedoMap->activate();
	}

	if (normalsMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + NORMALS_TEXTURE_INDEX);
		normalsMap->activate();
	}

	if (roughnessMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + ROUGHNESS_TEXTURE_INDEX);
		roughnessMap->activate();
	}

	// Update the UBOs
	if (mvpnUBO.get()) { mvpnUBO->update(mvpn); }
	if (lightsUBO.get()) { lightsUBO->update(lights); }
	if (lambertianUBO.get()) { lambertianUBO->update(lambertian); }
}

void ShaderProgram::deactivate() const noexcept
{
	// Deactivate the textures
	if (roughnessMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + ROUGHNESS_TEXTURE_INDEX);
		roughnessMap->deactivate();
	}

	if (normalsMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + NORMALS_TEXTURE_INDEX);
		normalsMap->deactivate();
	}

	if (albedoMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + ALBEDO_TEXTURE_INDEX);
		albedoMap->deactivate();
	}

	// Unset the active shader program
	glUseProgram(0);
}

void ShaderProgram::setViewVector(const GLfloat * newViewVector) noexcept
{
	glUseProgram(id);

	glUniform3fv(glGetUniformLocation(id, "view"), 1, newViewVector);

	glUseProgram(0);
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED
///////////////////////////////////////////////////////////////////////////////

bool ShaderProgram::createShaders(GLint & vertexShaderId,
								  GLint & fragmentShaderId) noexcept
{
	// The shaders' sources
	string vertexShaderSource = "";
	string fragmentShaderSource = "";

	// Used to check if shader compilation was successful
	GLint compiled;

	// Create the vertex shader
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

	// If unsuccessful
	if (!vertexShaderId)
	{
		// Log the error
		cout << "Shader Program: creation of Vertex Shader failed." << endl;

		return false;
	}

	// Create the fragment shader
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// If unsuccessful
	if (!fragmentShaderId)
	{
		// Log the error
		cout << "Shader Program: creation of Fragment Shader failed." << endl;

		// No need to clean up the vertex shader as both shaders will be
		// deleted regardless

		return false;
	}

	// Try to load the vertex shader source
	if (!vertexShaderLoader.get() ||
	   (!vertexShaderLoader->load(vertexShaderSource)))
	{
		// If unsuccessful, use a default vertex shader
		vertexShaderSource = "#version 330 core\n"
			"layout(location = 0) in vec3 position;\n"
			"uniform mat4 modelMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 projectionMatrix;\n"
			"void main()\n"
			"{\n"
			"gl_Position = vec4(position, 1.0f);\n"
			"}\n\0";

		// Log the error
		cout << "Shader Program: unable to load Vertex Shader. Using default."
			<< endl;
	}

	// Set the fragment shader's source and try to compile it
	const char * vSource = vertexShaderSource.c_str();
	glShaderSource(vertexShaderId, 1, & vSource, NULL);
	glCompileShader(vertexShaderId);

	// Check the result of compilation
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, & compiled);

	// If compilation failed
	if (!compiled)
	{
		// Determine the info log necessary length
		GLint logLength = 0;
		glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, & logLength);

		// Create the info log
		GLchar * infoLog = new GLchar[logLength];

		// Log the error
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);

		cout << "Shader Program: Vertex Shader compilation failed: "
			<< infoLog << endl;

		// Delete the info log
		delete[] infoLog;

		return false;
	}

	// Try to load the fragment shader source
	if (!fragmentShaderLoader.get() ||
	   (!fragmentShaderLoader->load(fragmentShaderSource)))
	{
		// If unsuccessful, use a default fragment shader
		fragmentShaderSource = "#version 330 core\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"color = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
			"}\n\0";

		// Log the error
		cout << "Shader Program: unable to load Fragment Shader. Using default."
			<< endl;
	}

	// Set the fragment shader's source and try to compile it
	const char * fSource = fragmentShaderSource.c_str();
	glShaderSource(fragmentShaderId, 1, & fSource, NULL);
	glCompileShader(fragmentShaderId);

	// Check the result of compilation
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, & compiled);

	// If compilation failed
	if (!compiled)
	{
		// Determine the info log necessary length
		GLint logLength = 0;
		glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &logLength);

		// Create the info log
		GLchar* infoLog = new GLchar[logLength];

		// Log the error
		glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);

		cout << "Shader Program: Fragment Shader compilation failed: "
			<< infoLog << endl;

		// Delete the info log
		delete[] infoLog;

		return false;
	}

	return true;
}

void ShaderProgram::deleteShaders(GLint vertexShaderId,
								  GLint fragmentShaderId) noexcept
{
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
}

bool ShaderProgram::createProgram(GLint vertexShaderId,
								  GLint fragmentShaderId) noexcept
{
	// Create the GL program
	id = glCreateProgram();

	// Attach the shaders and link them
	glAttachShader(id, vertexShaderId);
	glAttachShader(id, fragmentShaderId);
	glLinkProgram(id);

	// Check for linking errors
	GLint linked;

	glGetProgramiv(id, GL_LINK_STATUS, & linked);

	if (!linked)
	{
		// Determine the info log necessary length
		GLint logLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, & logLength);

		// Create the info log
		GLchar * infoLog = new GLchar[logLength];

		// Log the error
		glGetProgramInfoLog(id, 512, NULL, infoLog);

		cout << "Shader Program: Linking failed: " << infoLog << endl;

		// Delete the info log
		delete[] infoLog;

		return false;
	}

	return true;
}

void ShaderProgram::deleteProgram() noexcept
{
	glDeleteProgram(id);
}

void ShaderProgram::createUBOs() noexcept
{
	// MVP matrices UBO
	mvpnUBO = make_unique<UniformBufferObject>();
	mvpnUBO->create(sizeof(MVPN));
	mvpnUBO->bind(id, MVPN::getBlockName(), MVPN::getBindingIndex());

	// Lights UBO
	lightsUBO = make_unique<UniformBufferObject>();
	lightsUBO->create(sizeof(Lights));
	lightsUBO->bind(id, Lights::getBlockName(), Lights::getBindingIndex());

	// Lambertian UBO
	lambertianUBO = make_unique<UniformBufferObject>();
	lambertianUBO->create(sizeof(Lambertian));
	lambertianUBO->bind(id, Lambertian::getBlockName(), Lambertian::getBindingIndex());
}

void ShaderProgram::deleteUBOs() noexcept
{
	// Destroy the UBOs
	if (mvpnUBO.get()) { mvpnUBO->destroy(); }
	if (lightsUBO.get()) { lightsUBO->destroy(); }
	if (lambertianUBO.get()) { lambertianUBO->destroy(); }

	// Release the smart pointers
	mvpnUBO.release();
	lightsUBO.release();
	lambertianUBO.release();
}

void ShaderProgram::createTextures() noexcept
{
	// Load the textures and bind them
	if (albedoMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + ALBEDO_TEXTURE_INDEX);
		albedoMap->create();
	}

	if (normalsMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + NORMALS_TEXTURE_INDEX);
		normalsMap->create();
	}

	if (roughnessMap.get())
	{
		glActiveTexture(GL_TEXTURE0 + ROUGHNESS_TEXTURE_INDEX);
		roughnessMap->create();
	}

}

void ShaderProgram::deleteTextures() noexcept
{
	// Destroy the textures
	if (albedoMap.get())
	{
		albedoMap->destroy();
	}

	if (normalsMap.get())
	{
		normalsMap->destroy();
	}

	if (roughnessMap.get())
	{
		roughnessMap->destroy();
	}

	// Release the smart pointers
	albedoMap.reset();
	normalsMap.reset();
	roughnessMap.reset();
}