#include "ShaderProgram.h"
#include <iostream>
#include "shaders/buffers/UniformBufferObject.h"
#include "shaders/loaders/interfaces/IShaderLoader.h"

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

ShaderProgram::ShaderProgram(shared_ptr<IShaderLoader> newVertexShaderLoader,
							 shared_ptr<IShaderLoader> newFragmentShaderLoader,
							 const MVPN & newMvpn,
							 const Lights & newLights)
	noexcept :
	IShaderProgram(newVertexShaderLoader,
				   newFragmentShaderLoader,
				   newMvpn,
				   newLights)
{
	vertexShaderLoader = newVertexShaderLoader;
	fragmentShaderLoader = newFragmentShaderLoader;

	mvpn = & newMvpn;
	lights = & newLights;

	GLint vertexShaderId = 0;
	GLint fragmentShaderId = 0;

	if (createShaders(vertexShaderId, fragmentShaderId))
	{
		if (createProgram(vertexShaderId, fragmentShaderId))
		{
			createUBOs();
		}
	}

	deleteShaders(vertexShaderId, fragmentShaderId);
}

ShaderProgram::~ShaderProgram() noexcept
{
	vertexShaderLoader.reset();
	fragmentShaderLoader.reset();

	deleteUBOs();
	deleteProgram();
}

void ShaderProgram::activate() noexcept
{
	glUseProgram(id);

	if (mvpnUBO.get() && lightsUBO.get())
	{
		mvpnUBO->update(mvpn);
		lightsUBO->update(lights);
	}
}

void ShaderProgram::deactivate() const noexcept
{
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
}

void ShaderProgram::deleteUBOs() noexcept
{
	// Destroy the UBOs
	if (mvpnUBO.get())
	{
		mvpnUBO->destroy();
	}

	if (lightsUBO.get())
	{
		lightsUBO->destroy();
	}

	// Release the smart pointers
	mvpnUBO.release();
	lightsUBO.release();
}