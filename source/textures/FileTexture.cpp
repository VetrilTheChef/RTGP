#include "FileTexture.h"
#include "stb_image/stb_image.h"
#include <iostream>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

FileTexture::FileTexture(std::string& newTexturePath) noexcept :
	ITexture()
{
	path = newTexturePath;
}

FileTexture::FileTexture(std::string&& newTexturePath) noexcept :
	FileTexture(newTexturePath)
{
}

FileTexture::~FileTexture() noexcept
{
	destroy();
}

bool FileTexture::create() noexcept
{
	// This function assumes glActiveTexture has been already called.
	// Thus it is responsibility of the texture user to set the active
	// texture correctly before creating the texture itself.

	int channels = 0;
	unsigned char * image;

	// Load the image file
	image = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

	// If the image was loaded
	if (image)
	{
		// Generate a texture id
		glGenTextures(1, & texture);

		// Bind the texture to initialize it
		glBindTexture(GL_TEXTURE_2D, texture);

		GLint format = channels == 4 ? GL_RGBA : GL_RGB;

		glTexImage2D(GL_TEXTURE_2D, 0, format, (GLsizei)width, (GLsizei)height,
					 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Generate the mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Setup the UV values to repeat outside of the 0-1 range
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Specify the minification / magnification filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		// The image can be deleted as OpenGL has now loaded a copy
		stbi_image_free(image);

		return true;
	}
	else
	{
		// Log a warning
		cout << "Texture: could not load image " << path << "." << endl;

		return false;
	}
}

void FileTexture::bind(GLuint newProgram, const string & newBlockName,
					   GLuint newIndex) noexcept
{
	/*
	//index = newIndex;

	// Get the Uniform location
	GLint UL = glGetUniformLocation(newProgram, newBlockName.c_str());

	// If the block is found
	if (UL != GL_INVALID_INDEX)
	{
		// Done by the layout qualifier in the GLSL shader
		//glUniformBlockBinding(newProgram, UBI, index);

		// Initialize with NULL data
		glBindBufferBase(GL_UNIFORM_BUFFER, index, UBO);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	else
	{
		// Log a warning
		cout << "Uniform Buffer Object: could not find block named "
			<< newBlockName << "." << endl;
	}*/
}



void FileTexture::activate() noexcept
{
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, texture);
}

void FileTexture::deactivate() noexcept
{
	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FileTexture::destroy() noexcept
{
	width = -1;
	height = -1;

	glDeleteTextures(1, &texture);

	texture = -1;
}