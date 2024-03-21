///*********************************************************************
//Matt Marchant 2016
//http://trederia.blogspot.com
//
//tmxlite - Zlib license.
//
//This software is provided 'as-is', without any express or
//implied warranty. In no event will the authors be held
//liable for any damages arising from the use of this software.
//
//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute
//it freely, subject to the following restrictions:
//
//1. The origin of this software must not be misrepresented;
//you must not claim that you wrote the original software.
//If you use this software in a product, an acknowledgment
//in the product documentation would be appreciated but
//is not required.
//
//2. Altered source versions must be plainly marked as such,
//and must not be misrepresented as being the original software.
//
//3. This notice may not be removed or altered from any
//source distribution.
//*********************************************************************/
//#pragma once
//
//#include <vector>
//#include <set>
//#include <map>
//#include <string>
//#include <engine/OldMapLayer.h>
//#include <engine/GL/Texture.h>
//#include <engine/GL/Shader.h>
//#include <engine/GL/Camera.h>
//#include <engine/Vector.h>
//
/////A class to load a tilemap using the tmxlite library
//class Tilemap final
//{
//public:
//	///Constructor
//	Tilemap(engine::Camera* cam);
//	~Tilemap();
//	
//	///Load tilemap
//	void loadMap(const std::string ownMap);
//	///Draw tilemap
//	void draw(float layer);
//
//	///Get the position of a tile in world coordinates
//	engine::Vector2 GetTilePosition(unsigned int x, unsigned int y);
//	///Check larger area collisionbox
//	std::vector<engine::Vector2> CheckCollisionBox(engine::Vector2 topLeft, engine::Vector2 bottomRight);
//	///Check the smaller Collisions that are turned on upon the larger collision box colliding
//	unsigned int checkCollision(float x, float y);
//	///The size of a tile
//	tmx::Vector2u tileSize;
//	///The position of a tile
//	glm::vec3 position;
//	///The bounds of the map
//	tmx::FloatRect bounds;
//
//	///How many layers are there in the map
//	std::set<float> zLayers;
//
//	///A 2D vector of tile IDs used for simple tile collision checking
//	std::vector<std::vector<unsigned int>> collisionLayer;
//private:
//	void initGLStuff(const tmx::Map&);
//	std::shared_ptr<engine::Texture> loadTexture(const std::string&);
//
//
//	std::map<float, std::vector<std::shared_ptr<MapLayer>>> mapLayers;
//	std::vector<std::shared_ptr<engine::Texture>> allTextures;
//	
//	engine::Shader* m_shader;
//	engine::Camera* camera;
//};
