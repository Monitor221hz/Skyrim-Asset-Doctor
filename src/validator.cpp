#include "validator.h"

void AssetDoctor::Validator::AddTexturePath(const char *path)
{
    WriteLocker writeLocker(lock);
    texture_paths.emplace(std::string(path));
}

void AssetDoctor::Validator::AddMeshPath(const char *path)
{
    WriteLocker writeLocker(lock);
    mesh_paths.emplace(std::string(path));
}

void AssetDoctor::Validator::ValidateTexturePaths()
{
    WriteLocker writeLocker(lock);
    for(auto texture_path : texture_paths)
    {
        std::transform(texture_path.begin(), texture_path.end(), texture_path.begin(), ::tolower);

        if (!texture_path.empty() && texture_path[0] == '\b') { continue; }

        if (texture_path.length() > 5)
        {
            if (texture_path.compare(0, 4, "data") == 0)
            {
                texture_path.erase(0, 5);
            }
            if (texture_path.compare(0, 6, "actors") == 0)
            {
                texture_path.insert(0, "textures\\");
            }
        }

        std::filesystem::path absolute_path = std::filesystem::current_path() / "data" / texture_path;

        if (!std::filesystem::exists(absolute_path) && !(RE::BSResourceNiBinaryStream(texture_path).good())) 
        {
            SKSE::log::info("Missing Texture: {}", absolute_path.string());

            Interface::AddMissingTexture(texture_path);
        }
    }
    texture_paths.clear();


}

void AssetDoctor::Validator::ValidateMeshPaths()
{
    WriteLocker writeLocker(lock);
    for(auto mesh_path : mesh_paths)
    {
        std::transform(mesh_path.begin(), mesh_path.end(), mesh_path.begin(), ::tolower);

        if (!mesh_path.empty() && mesh_path[0] == '\b') { continue;}

        mesh_path.insert(0, "meshes\\");

        std::filesystem::path absolute_path = std::filesystem::current_path() / "data" / mesh_path;

        if (!std::filesystem::exists(absolute_path) && !(RE::BSResourceNiBinaryStream(mesh_path).good()))
        {
            SKSE::log::info("Missing Mesh: {}", absolute_path.string());

            
        }
    }
}
