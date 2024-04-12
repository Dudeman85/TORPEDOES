#pragma once
#include <string>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <engine/Constants.h>

namespace engine
{
	//Forward declare Image class for second constructor
	class Image;

	///Abstraction class for OpenGL textures
	class Texture
	{
	private:
		Texture(const Texture&);
		Texture();

	public:
		///Constructor
		Texture(int sx, int sy, const std::vector<std::uint16_t>& data)
		{
			//Generate and bind texture
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, sx, sy, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, &data[0]);
			float aniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		///Load a texture from path
		Texture(std::string path, unsigned int filteringType = GL_NEAREST, bool flip = true)
		{
			//Flip the image when loading into an OpenGL texture
			stbi_set_flip_vertically_on_load(flip);

			int width, height, nrChannels;
			unsigned char* imageData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

			if (imageData)
			{
				//Set the OpenGL texture format to include alpha if appropriate
				GLint colorFormat;
				if (nrChannels == 4)
				{
					colorFormat = GL_RGBA;
				}
				else if (nrChannels == 3)
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					colorFormat = GL_RGB;
				}
				else
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					colorFormat = GL_RED;
				}

				//Generate and bind texture
				glGenTextures(1, &id);
				glBindTexture(GL_TEXTURE_2D, id);

				//Set texture filtering parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringType);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringType);

				//Generate the texture using the image data
				glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, imageData);
				glGenerateMipmap(GL_TEXTURE_2D);

				//Unbind texture
				glBindTexture(GL_TEXTURE_2D, 0);

				//Image data is no longer needed
				stbi_image_free(imageData);
			}
			else
			{
				std::cout << "Error loading texture from " << path << std::endl;
			}
		}
		///Declare the constuctor through image. It is defined in Image.h
		inline Texture(Image image, unsigned int filteringType = GL_NEAREST);

		~Texture()
		{
			glDeleteTextures(1, &id);
		}

		///Sets the OpenGL sampling type when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
		void SetScalingFilter(unsigned int type)
		{
			glBindTexture(GL_TEXTURE_2D, id);

			//Set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);

			//Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		///Get this textures OpenGL ID
		unsigned int ID()
		{
			return id;
		}

		///Use this texture to draw the next sprite
		void Use()
		{
			glBindTexture(GL_TEXTURE_2D, id);
		}

		std::string path = "";
		std::string type = "";
	private:
		unsigned int id = 0;
	};
}