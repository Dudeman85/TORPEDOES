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
#define _USE_MATH_DEFINES
#include <engine/MapLayer.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <engine/GL/Texture.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine/GL/Utils.h>

//MapLayer::MapLayer(const tmx::Map& map, std::size_t layerIdx, const std::vector<std::shared_ptr<engine::Texture>>& textures, unsigned int tileSize) : m_allTextures(textures), tileSize(tileSize)
MapLayer::MapLayer(const tmx::Map& map, std::size_t layerIdx, const std::vector<std::shared_ptr<engine::Texture>>& textures) : m_allTextures(textures)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    createSubsets(map, layerIdx);
    /*
    const auto mapSize = map.getTileCount();
    unsigned int mapSizeUInt = static_cast<unsigned int>(mapSize.x * mapSize.y);

    createSubsets(map, layerIdx, mapSizeUInt);
    */
}

MapLayer::~MapLayer()
{
    for(auto& ss : m_subsets)
    {
        if(ss.vbo)
        {
            glDeleteBuffers(1, &ss.vbo);
        }
        //don't delete the tileset textures as these are
        //shared and deleted elsewhere
    }
}

void MapLayer::draw(glm::mat4 model, unsigned int modelLoc, unsigned int tilesetCountLoc, unsigned int tileSizeLoc)
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_subsets[0].vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zLayer));
    model = glm::rotate(model, (float)M_PI, glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniform2f(tileSizeLoc, tileSize.x, tileSize.y);

    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    for(const auto& ss : m_subsets)
    {
        glBindBuffer(GL_ARRAY_BUFFER, ss.vbo);

        glUniform2f(tilesetCountLoc, ss.sx, ss.sy);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ss.texture->ID());
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ss.lookup->ID());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindVertexArray(0);
}

//private
//void MapLayer::createSubsets(const tmx::Map& map, std::size_t layerIdx, unsigned int mapSizeUInt)
void MapLayer::createSubsets(const tmx::Map& map, std::size_t layerIdx)
{
    const auto& layers = map.getLayers();
    if(layerIdx >= layers.size() || (layers[layerIdx]->getType() != tmx::Layer::Type::Tile))
    {
        std::cout << "Invalid layer index or layer type, layer will be empty" << std::endl;
        return;
    }
    const auto layer = dynamic_cast<const tmx::TileLayer*>(layers[layerIdx].get());
    
    auto bounds = map.getBounds();
    float verts[] = 
    {
      bounds.left, bounds.top, 0.f, 0.f, 0.f,
      bounds.left + bounds.width, bounds.top, 0.f, 1.f, 0.f,
      bounds.left, bounds.top + bounds.height, 0.f, 0.f, 1.f,
      bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 1.f
    };
    
    const auto& mapSize = map.getTileCount();
    const auto& tilesets = map.getTilesets();
    for(auto i = 0u; i < tilesets.size(); ++i)
    {        
        //check each tile ID to see if it falls in the current tile set
        const auto& ts = tilesets[i];
        int numRows = ts.getTileCount() / ts.getColumnCount();
        int numCols = ts.getColumnCount();
        const auto& tileIDs = layer->getTiles();
        std::vector<std::uint16_t> pixelData;
        bool tsUsed = false;
        
        for(auto y = 0u; y < mapSize.y; ++y)
        {
            for(auto x = 0u; x < mapSize.x; ++x)
            {
               auto idx = y * mapSize.x + x;
               if (idx < tileIDs.size() && tileIDs[idx].ID >= ts.getFirstGID()
                    && tileIDs[idx].ID < (ts.getFirstGID() + ts.getTileCount()))
                {      
                   auto id = tileIDs[idx].ID - ts.getFirstGID() + 1;
                   pixelData.push_back(static_cast<std::uint16_t>(id)); //red channel - making sure to index relative to the tileset
                   pixelData.push_back(static_cast<std::uint16_t>(tileIDs[idx].flipFlags)); //green channel - tile flips are performed on the shader
                    tsUsed = true;

                }
                else
                {
                    //pad with empty space
                    pixelData.push_back(0);
                    pixelData.push_back(0);
                }
            }
        }
        
        //if we have some data for this tile set, create the resources
        if(tsUsed)
        {
            m_subsets.emplace_back();
            m_subsets.back().sx = numCols;
            m_subsets.back().sy = numRows;
            m_subsets.back().texture = m_allTextures[i];
            m_subsets.back().lookup = std::make_shared<engine::Texture>(mapSize.x, mapSize.y, pixelData);
     
            glGenBuffers(1, &m_subsets.back().vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_subsets.back().vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        }
    }    
}
