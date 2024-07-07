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
        ValidateTexturePath(texture_path);
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

AssetDoctor::Validator::AssetStatus AssetDoctor::Validator::ValidateTexturePath(std::string &texture_path)
{

    std::transform(texture_path.begin(), texture_path.end(), texture_path.begin(), ::tolower);

    if (!texture_path.empty() && texture_path[0] == '\b')
    {
        return AssetStatus::Invalid;
    }

    if (texture_path.length() > 5 && texture_path.compare(0, 4, "data") == 0)
    {
        texture_path.erase(0, 5);
    }

    if (texture_path.length() > 7 && texture_path.compare(0, 8, "textures") != 0)
    {
        texture_path.insert(0, "textures\\");
    }

    std::filesystem::path absolute_path = std::filesystem::current_path() / "data" / texture_path;
    auto bs_stream = RE::BSResourceNiBinaryStream(texture_path);

    if (std::filesystem::exists(absolute_path))
    {
        return AssetStatus::Loose;
    }
    if (bs_stream.good())
    {
        return AssetStatus::Archive;
    }

    SKSE::log::info("Missing Texture: {}", absolute_path.string());

    Interface::AddMissingTexture(texture_path);
    
    return AssetStatus::Missing;
}

AssetDoctor::Validator::AssetStatus AssetDoctor::Validator::ValidateMeshPath(std::string &mesh_path)
{
std::transform(mesh_path.begin(), mesh_path.end(), mesh_path.begin(), ::tolower);

        if (!mesh_path.empty() && mesh_path[0] == '\b') { return AssetStatus::Invalid; }

        mesh_path.insert(0, "meshes\\");

        std::filesystem::path absolute_path = std::filesystem::current_path() / "data" / mesh_path;
        auto bs_stream = RE::BSResourceNiBinaryStream(mesh_path);

        if (std::filesystem::exists(absolute_path))
        {
            return AssetStatus::Loose;
        }
        if (bs_stream.good())
        {
            return AssetStatus::Archive;
        }
        SKSE::log::info("Missing Mesh: {}", absolute_path.string());
        Interface::AddMissingMesh(mesh_path); 
        return AssetStatus::Missing; 
}
