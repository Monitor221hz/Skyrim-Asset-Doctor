#include "validator.h"

void AssetDoctor::Validator::AddTexturePath(const char *path)
{
    WriteLocker writeLocker(q_lock);
    texture_paths.emplace(std::string(path));
}

void AssetDoctor::Validator::AddMeshPath(const char *path)
{
    WriteLocker writeLocker(q_lock);
    mesh_paths.emplace(std::string(path));
}

bool AssetDoctor::Validator::ValidateTexturePaths()
{
    WriteLocker writeLocker(q_lock);
    bool valid = true; 
    for(auto texture_path : texture_paths)
    {
        auto result = ValidateTexturePath(texture_path);
        valid = result != AssetStatus::Missing; 
    }
    texture_paths.clear();
    asset_logger->flush(); 
    return valid; 
}

bool AssetDoctor::Validator::ValidateMeshPaths()
{
    WriteLocker writeLocker(q_lock);
    bool valid = true; 
    for(auto mesh_path : mesh_paths)
    {
        ValidateMeshPath(mesh_path); 
    }
    asset_logger->flush(); 
    return valid; 
}

AssetDoctor::Validator::AssetStatus AssetDoctor::Validator::ValidateTexturePath(std::string &texture_path)
{

    std::transform(texture_path.begin(), texture_path.end(), texture_path.begin(), ::tolower);

    if (!texture_path.empty() && texture_path[0] == '\b')
    {
        return AssetStatus::Invalid;
    }
    auto data_substr_index = texture_path.find("data\\");
    if (data_substr_index != std::string::npos)
    {
        texture_path.erase(0, data_substr_index+5); 
    }
    if (texture_path.length() > 7 && texture_path.compare(0, 8, "textures") != 0)
    {
        texture_path.insert(0, "textures\\");
    }
    ReadLocker locker(path_lock); 
    if (missing_asset_paths.contains(texture_path))
    {
        return AssetStatus::Missing;
    }
    locker.unlock(); 
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

    asset_logger->warn("missing texture {}", absolute_path.relative_path().string()); 
    Interface::IncrementMissingTextures(); 
    missing_asset_paths.emplace(texture_path); 
    
    return AssetStatus::Missing;
}

AssetDoctor::Validator::AssetStatus AssetDoctor::Validator::ValidateMeshPath(std::string &mesh_path)
{
        std::transform(mesh_path.begin(), mesh_path.end(), mesh_path.begin(), ::tolower);

        if (!mesh_path.empty() && mesh_path[0] == '\b') { return AssetStatus::Invalid; }
        mesh_path.insert(0, "meshes\\");

        ReadLocker locker(path_lock); 
        if (missing_asset_paths.contains(mesh_path))
        {
            return AssetStatus::Missing; 
        }
        locker.unlock(); 

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
        asset_logger->warn("missing mesh {}", absolute_path.relative_path().string()); 
        Interface::IncrementMissingMeshes(); 
        AddMissingAsset(mesh_path); 
        return AssetStatus::Missing; 
}

AssetDoctor::Validator::AssetStatus AssetDoctor::Validator::ValidateMeshPath(TESObjectREFR* refr, std::string &mesh_path)
{
        std::transform(mesh_path.begin(), mesh_path.end(), mesh_path.begin(), ::tolower);

        if (!mesh_path.empty() && mesh_path[0] == '\b') { return AssetStatus::Invalid; }
        mesh_path.insert(0, "meshes\\");

        ReadLocker locker(path_lock); 
        if (missing_asset_paths.contains(mesh_path))
        {
            return AssetStatus::Missing; 
        }
        locker.unlock(); 

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
        asset_logger->warn("missing mesh {}", absolute_path.relative_path().string()); 
        Interface::IncrementMissingMeshes(); 
        AddMissingAsset(mesh_path); 
        // Interface::AddMissingMesh(refr->GetFormID(), mesh_path);
        return AssetStatus::Missing; 
}

void AssetDoctor::Validator::ValidateReference(TESObjectREFR* ref)
{
    if (!ref) { return; }

    auto* ref3d = ref->Get3D2();

    if (!ref3d) { return; }

    auto geoms = NifUtil::Node::GetAllGeometries(ref3d);

    for (auto* geom : geoms)
    {
        if (!geom) { continue; }
        auto effect_ptr = geom->GetGeometryRuntimeData().properties[BSGeometry::States::kEffect];

        auto* effect = effect_ptr.get();

        if (!effect) { continue; }

        auto* lighting_shader = netimmerse_cast<BSLightingShaderProperty *>(effect);
        if (!lighting_shader) { continue; }

        auto* base_material = lighting_shader->material;
        if (!base_material) { continue; }

        auto* material = static_cast<BSLightingShaderMaterialBase*>(static_cast<BSShaderMaterial*>(base_material));
        if (!material) { continue; }

        auto texture_set_ptr = material->GetTextureSet();

        auto* texture_set = texture_set_ptr.get();
        if (!texture_set) { continue; }

        for(int i = 0; i < 8; i++)
        {
            const char* raw_texture_path = texture_set->GetTexturePath(static_cast<BSTextureSet::Texture>(i));
            if (!raw_texture_path || raw_texture_path[0] == '\0') { continue; }

            Validator::AddTexturePath(raw_texture_path);
        }
        
    }
    
    bool texturesValid = Validator::ValidateTexturePaths();
    Validator::ValidateReferenceMesh(ref); 
    return;
}

void AssetDoctor::Validator::ValidateReferenceMesh(TESObjectREFR *ref)
{
    auto* bound_object = ref->GetObjectReference();
    if (!bound_object) { return; }
    auto *model = bound_object->As<TESModel>();
    if (!model)
    {
        return;
    }
    const char *raw_model_path = model->GetModel();
    if (!raw_model_path || raw_model_path[0] == '\0')
    {
        return;
    }
    std::string model_path(raw_model_path); 
    ValidateMeshPath(ref, model_path); 
}

void AssetDoctor::Validator::ValidateNiObject(NiAVObject *niObject)
{
    if (!niObject) { return; }
    auto geoms = NifUtil::Node::GetAllGeometries(niObject);

    for (auto* geom : geoms)
    {
        if (!geom) { continue; }
        auto effect_ptr = geom->GetGeometryRuntimeData().properties[BSGeometry::States::kEffect];

        auto* effect = effect_ptr.get();

        if (!effect) { continue; }

        auto* lighting_shader = netimmerse_cast<BSLightingShaderProperty *>(effect);
        if (!lighting_shader) { continue; }

        auto* base_material = lighting_shader->material;
        if (!base_material) { continue; }

        auto* material = static_cast<BSLightingShaderMaterialBase*>(static_cast<BSShaderMaterial*>(base_material));
        if (!material) { continue; }

        auto texture_set_ptr = material->GetTextureSet();

        auto* texture_set = texture_set_ptr.get();
        if (!texture_set) { continue; }

        for(int i = 0; i < 8; i++)
        {
            const char* raw_texture_path = texture_set->GetTexturePath(static_cast<BSTextureSet::Texture>(i));
            if (!raw_texture_path || raw_texture_path[0] == '\0') { continue; }

            Validator::AddTexturePath(raw_texture_path);
        }
        
    }
    
    if (!Validator::ValidateTexturePaths())
    {
        auto *ref = niObject->GetUserData();
        if (!ref) { return; }
        auto *bound_obj = ref->GetObjectReference();
        if (!bound_obj)
        {
            return;
        }
        ValidateBoundObject(bound_obj); 
    }
}

void AssetDoctor::Validator::ValidateLightingShaderProperty(BSLightingShaderProperty *lighting_shader)
{

    auto *base_material = lighting_shader->material;
    if (!base_material)
    {
        return;
    }

    auto *material = static_cast<BSLightingShaderMaterialBase *>(static_cast<BSShaderMaterial *>(base_material));
    if (!material)
    {
        return;
    }

    auto texture_set_ptr = material->GetTextureSet();

    auto *texture_set = texture_set_ptr.get();
    if (!texture_set)
    {
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        const char *raw_texture_path = texture_set->GetTexturePath(static_cast<BSTextureSet::Texture>(i));
        if (!raw_texture_path || raw_texture_path[0] == '\0')
        {
            continue;
        }
        Validator::AddTexturePath(raw_texture_path);
    }
    Validator::ValidateTexturePaths(); 
}

void AssetDoctor::Validator::ValidateBoundObject(TESBoundObject *bound_object)
{
    auto *model = bound_object->As<TESModel>();
    if (!model)
    {
        return;
    }
    const char *raw_model_path = model->GetModel();
    if (!raw_model_path || raw_model_path[0] == '\0')
    {
        return;
    }
    std::string model_path(raw_model_path); 
    ValidateMeshPath(model_path);
}

void AssetDoctor::Validator::AddMissingAsset(std::string a_path)
{
    WriteLocker locker(path_lock);
    if (missing_asset_paths.size() == missing_asset_paths.max_size())
    {
        missing_asset_paths.clear(); 
        Interface::QueueReset(); 
    }
    missing_asset_paths.emplace(a_path);  
}
