#pragma once
#include <filesystem>
#include <unordered_map>

#include <engine/GL/Texture.h>
#include <engine/GL/Model.h>
#include <engine/Constants.h>

namespace engine
{
    //Recursively loads all textures 
    std::unordered_map<std::string, Texture*> ProcessDirectoryTextures(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR)
    {
        std::unordered_map<std::string, Texture*> textures;
    
        //For each item in the directory
        for (const auto& directoryEntry : std::filesystem::directory_iterator(path))
        {
            std::string pathName = directoryEntry.path().string();

            //If the item is a directory and recursive is enabled
            if (is_directory(directoryEntry.path()) && includeSubdirectories)
            {
                // Call the function recursively
                std::unordered_map<std::string, Texture*> subdirTextures = ProcessDirectoryTextures(pathName, includeSubdirectories, filteringType);
                textures.insert(subdirTextures.begin(), subdirTextures.end());
                continue;
            }

            // Name creation
            std::string name = pathName;
            size_t position = path.find(path);
            if (position != std::string::npos)
            {
                // Erase the original path from name
                name.erase(position, path.length());
            }

            if (!name.empty())
            {
                // Erase the first character, which is "/"
                name = name.substr(1);
            }

            std::string extension = "";
            position = name.find_last_of('.');
            if (position != std::string::npos)
            {
                extension = name.substr(position + 1); // Save the extension
            }

            // Load .png as texture
            if (extension == "png")
            {
                textures.emplace(name, new Texture(pathName, filteringType, true, false));
            }
        }

        return textures;
    }

    //Recursively loads all models
    std::unordered_map<std::string, Model*> ProcessDirectoryModels(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR)
    {
        std::unordered_map<std::string, Model*> models;
    
        //For each item in the directory
        for (const auto& directoryEntry : std::filesystem::directory_iterator(path))
        {
            std::string pathName = directoryEntry.path().string();

            //If the item is a directory and recursive is enabled
            if (is_directory(directoryEntry.path()) && includeSubdirectories)
            {
                // Call the function recursively
                std::unordered_map<std::string, Model*> subdirTextures = ProcessDirectoryModels(pathName, includeSubdirectories, filteringType);
                models.insert(subdirTextures.begin(), subdirTextures.end());
                continue;
            }

            // Name creation
            std::string name = pathName;
            size_t position = path.find(path);
            if (position != std::string::npos)
            {
                // Erase the original path from name
                name.erase(position, path.length());
            }

            if (!name.empty())
            {
                // Erase the first character, which is "/"
                name = name.substr(1);
            }

            std::string extension = "";
            position = name.find_last_of('.');
            if (position != std::string::npos)
            {
                extension = name.substr(position + 1); // Save the extension
            }

            // Load .obj as model
            if (extension == "obj")
            {
                models.emplace(name, new Model(pathName, false));
            }
        }

        return models;
    }

    //Load every texture from path, if includeSubdirectories is specified also recursively loads subdirectories
    //Returns a map with the name of the texture's path relative to root directory, ex. "level1/Player.png"
    std::unordered_map<std::string, Texture*> PreloadTextures(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR)
    {
        return ProcessDirectoryTextures(assetPath + path, includeSubdirectories, filteringType);
    }

    //Load every model from path, if includeSubdirectories is specified also recursively loads subdirectories
    //Returns a map with the name of the models's path relative to root directory, ex. "level1/Player.obj"
    std::unordered_map<std::string, Model*> PreloadModels(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR)
    {
        return ProcessDirectoryModels(assetPath + path, includeSubdirectories, filteringType);
    }
}