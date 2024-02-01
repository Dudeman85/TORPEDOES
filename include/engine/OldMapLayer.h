/*********************************************************************
Matt Marchant 2016
http://trederia.blogspot.com

tmxlite - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <engine/GL/Texture.h>
#include <tmxlite/Map.hpp>

class MapLayer final
{
public:
	MapLayer(const tmx::Map&, std::size_t, const std::vector <std::shared_ptr<engine::Texture>>& textures);
	~MapLayer();
	
	MapLayer(const MapLayer&) = delete;
	MapLayer& operator = (const MapLayer&) = delete;
	
	void draw(glm::mat4 model, unsigned int modelLoc, unsigned int, unsigned int);

	float zLayer = 0;
	glm::vec2 tileSize;
private:
	std::vector < std::shared_ptr<engine::Texture> > m_allTextures;

	unsigned int VAO;
	//unsigned int tileSize;

	struct Subset final
	{
		float sx;
		float sy;
		unsigned vbo = 0;
		std::shared_ptr<engine::Texture> texture = 0;
		std::shared_ptr<engine::Texture> lookup = 0;
	};
	std::vector<Subset> m_subsets;

	void createSubsets(const tmx::Map&, std::size_t);
};