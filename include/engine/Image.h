#pragma once
#include <vector>
#include <engine/GL/Texture.h>

using namespace std;

namespace engine
{
	//Struct representing a pixel's r, g, b, and a values
	struct Pixel
	{
		unsigned char r, g, b, a;
	};

	//Image class basically just a 2d vector of rgba values
	class Image
	{
	public:
		Image(const char* path)
		{
			//Dont flip the image when loading to an Image
			stbi_set_flip_vertically_on_load(false);
			//Load image
			unsigned char* imageData = stbi_load(path, &width, &height, &channels, 4);

			//If the image is loaded successfully
			if (imageData)
			{
				//Resize the pixmap vector to be able to insert just by index
				pixmap.resize(width);
				for (size_t i = 0; i < width; i++)
					pixmap[i].resize(height);

				int i = 0;
				//For each row and column
				for (size_t y = 0; y < height; y++)
				{
					for (size_t x = 0; x < width; x++)
					{
						//Get the rgba values and put them in a nice to use 2D vector of Pixels
						pixmap[x][y] = Pixel{ imageData[i], imageData[i + 1], imageData[i + 2], imageData[i + 3] };
						i += 4;
					}
				}

				//Image data is no longer needed
				stbi_image_free(imageData);
			}
			else
			{
				std::cout << "Error loading texture from " << path << std::endl;
			}
		}
		//Construct an image from a 2D vector of pixels
		Image(vector<vector<Pixel>> pixels)
		{
			width = pixels.size();
			height = pixels[0].size();
			channels = 4;
			pixmap = pixels;
		}

		vector<Pixel> operator[](int i)
		{
			return pixmap[i];
		}

		//Returns the data of this imge in stbimage friendly format
		unsigned char* data()
		{
			unsigned char* data = new unsigned char[width * height * 4];
			int i = 0;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					data[i++] = pixmap[x][y].r;
					data[i++] = pixmap[x][y].g;
					data[i++] = pixmap[x][y].b;
					data[i++] = pixmap[x][y].a;
				}
			}
			return data;
		}

		//Get a subsection of pixels from x1 y1 top-left, to x2, y2 bottom-right (inclusive).
		Image Slice(int x1, int y1, int x2, int y2)
		{
			assert(x1 < x2&& y1 < y2 && "x1 and y1 must be less than x2 and y2!");
			assert(x1 >= 0 && y1 >= 0 && x2 < width&& y2 < height && "Slice must be in bounds of original image!");

			vector<vector<Pixel>> slice;
			slice.resize(x2 - x1 + 1);
			for (size_t i = 0; i < x2 - x1 + 1; i++)
			{
				slice[i].resize(y2 - y1 + 1);
			}

			//For the region defined by parameters
			int sliceX = 0;
			for (size_t x = x1; x < x2; x++)
			{
				int sliceY = 0;
				for (size_t y = y1; y < y2; y++)
				{
					//Move the pixels from this pixmap to the sliced one
					slice[sliceX][sliceY] = pixmap[x][y];
					sliceY++;
				}
				sliceX++;
			}

			return Image(slice);
		}

		int width;
		int height;
	private:
		int channels;
		vector<vector<Pixel>> pixmap;
	};

	//Create a texture from an image
	//Declared in Texture.h
	Texture::Texture(Image image, unsigned int filteringType)
	{
		//Convert the image to a 1D char array for OpenGL
		unsigned char* imageData = new unsigned char[image.width * image.height * 4];
		int i = 0;
		//Make sure to flip the vertical for OpenGL
		for (int y = image.height - 1; y >= 0; y--)
		{
			for (int x = 0; x < image.width; x++)
			{
				imageData[i++] = image[x][y].r;
				imageData[i++] = image[x][y].g;
				imageData[i++] = image[x][y].b;
				imageData[i++] = image[x][y].a;
			}
		}

		//Generate and bind texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		//Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringType);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringType);

		//Generate the texture using the image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//Slice spritesheet image to multiple textures.
	//spritesWide is how many sprites wide the spritesheet is and spritesHigh is how many sprites tall the spritesheet is
	vector<Texture*> SliceSpritesheet(const char* path, int spritesWide, int spritesHigh)
	{
		vector<Texture*> slicedTextures;

		//Load the spritesheet form path
		Image spritesheet = Image(path);

		//Get the size of each sprite
		const int width = floor((spritesheet.width - 1) / spritesWide);
		const int height = floor((spritesheet.height - 1) / spritesHigh);

		//Warn if the spritesheet is weirdly sized
		if (spritesheet.width % spritesWide != 0 || spritesheet.height % spritesHigh != 0)
			cout << "Spritesheet is not divisible by sprite count. Clipping may occur!";

		//For each sprite to slice out
		int paddingY = 0;
		for (size_t row = 0; row < spritesHigh; row++)
		{
			int paddingX = 0;
			for (size_t col = 0; col < spritesWide; col++)
			{
				//Get the slice from the spritesheet
				Image slice = spritesheet.Slice(col * width + paddingX, row * height + paddingY, col * width + width + paddingX, row * height + height + paddingY);
				slicedTextures.push_back(new Texture(slice));
				paddingX++;
			}
			paddingY++;
		}

		return slicedTextures;
	}

	//Creates animations from a spritesheet.
	//Each row of sprites on the spritesheet becomes one animation. 
	//You must provide a vector of delays for each frame going from top-left to bottom-right
	vector<Animation> AnimationsFromSpritesheet(const char* path, int spritesWide, int spritesHigh, vector<int> delays)
	{
		vector<Texture*> allFrames = SliceSpritesheet(path, spritesWide, spritesHigh);

		vector<Animation> animations;

		assert(allFrames.size() == delays.size() && "You must provide a delay for after each animation frame!");

		//For each animation (row in the spritesheet)
		for (size_t i = 0; i < spritesHigh; i++)
		{
			//Slice the proper textures to a new vector
			vector<Texture*> frames(spritesWide);
			copy(allFrames.begin() + i * spritesWide, allFrames.begin() + (i + 1) * spritesWide, frames.begin());

			//Slice the proper timings to a new vector
			vector<int> slicedDelays(spritesWide);
			copy(delays.begin() + i * spritesWide, delays.begin() + (i + 1) * spritesWide, slicedDelays.begin());

			//Create a new animation out of the sliced data
			animations.push_back(Animation(frames, slicedDelays));
		}

		return animations;
	}
}