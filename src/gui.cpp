#include "gui.h"
using namespace RE;
void AssetDoctor::Interface::Draw()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

    ImGui::Begin("##Main", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    auto* current_font = ImGui::GetFont();
    current_font->Scale = font_scale;

    ImGui::PushFont(current_font);
    DrawMeshLog();
    DrawTextureLog();
    DrawLabels();
    ImGui::PopFont();

    ImGui::SetWindowFontScale(1.0f);

    if (reset_queued)
    {
        missing_texture_paths.clear();
        missing_mesh_paths.clear();
        reset_queued.store(false);
    }

    
    ImGui::End();
}



void AssetDoctor::Interface::DrawTextureLog()
{
    if (missing_texture_paths.empty())
    {
        return;
    }

    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Missing Textures");
    ImGui::BeginChild("Scrolling Textures");

    if (missing_texture_log_write)
    {
        for (auto &path : missing_texture_paths)
        {
            ImGui::TextColored(Settings::GetMissingTextureTextColor(), path.c_str());
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();
}

void AssetDoctor::Interface::DrawMeshLog()
{
    if (missing_mesh_paths.empty())
    {
        return;
    }

    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Missing Meshes");
    ImGui::BeginChild("Scrolling Meshes");

    if (missing_mesh_log_write)
    {
        for (auto &path : missing_mesh_paths)
        {
            ImGui::TextColored(Settings::GetMissingMeshTextColor(), path.c_str());
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();
}

void AssetDoctor::Interface::DrawLabelTexturePath(TESObjectREFR *refr)
{
    DrawLabelTexturePath(refr->Get3D2());
}
void AssetDoctor::Interface::DrawLabel(NiAVObject *mesh)
{
    switch(label_mode)
    {
        case LabelMode::MeshCount:
            DrawLabelMeshCount(mesh);
            break;
        case LabelMode::MeshPaths:
            DrawLabelMeshPath(mesh->GetUserData());
            break;
        case LabelMode::TexturePaths:
            DrawLabelTexturePath(mesh);
            break;
    }
}

void AssetDoctor::Interface::DrawLabel(TESObjectREFR *refr)
{
    DrawLabel(refr->Get3D2());
}
void AssetDoctor::Interface::DrawLabelMeshPath(TESObjectREFR *refr)
{
    if (!refr) { return; }
    auto* bound_obj = refr->GetObjectReference();
    if (!bound_obj) { return; }

    auto* model = bound_obj->As<TESModel>();
    if (!model) { return; }

    const char *raw_model_path = model->GetModel();
    if (!raw_model_path || raw_model_path[0] == '\0')
    {
        return;
    }

    auto size = ImGui::GetWindowSize();
    float scale_x = size.x * .75;
    float scale_y = size.y * .75;

    RE::NiPoint3 target_pos = refr->GetPosition();
    float x, y, z;
    std::string model_path(raw_model_path);
    float line_height = ImGui::GetTextLineHeightWithSpacing();
    float line_width = ImGui::CalcTextSize(model_path.c_str()).x;
    NiCamera::WorldPtToScreenPt3((float(*)[4])world_to_cam_matrix, *view_port, target_pos, x, y, z, 1e-5f);
    y *= scale_y;
    x *= scale_x;
    if ((y <= 0.0f && x <= 0.0f) || (y >= size.y || x >= size.x))
    {
        x = size.x / 2 - line_width;
        y = size.y / 2;
    }
    else
    {
        x = MathUtil::Clamp(x, 0.0f, size.x - line_width);
        y = MathUtil::Clamp(y, 0.0f, size.y);
    }

    y += (y < size.y / 2) ? (line_height * label_count) : -(line_height * label_count);
    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(line_width,
                                             line_height),
        ImColor(0.0f, 0.0f, 0.0f, 0.68f));
    auto status = Validator::ValidateTexturePath(model_path);
            switch(status)
        {
            case Validator::AssetStatus::Missing:
                ImGui::TextColored(Settings::GetMissingAssetTextColor(), raw_model_path);
                break;
            case Validator::AssetStatus::Archive:
                ImGui::TextColored(Settings::GetArchiveAssetTextColor(), raw_model_path);
                break;
            case Validator::AssetStatus::Loose:
                ImGui::TextColored(Settings::GetLooseAssetTextColor(), raw_model_path);
                break;
            default:
                ImGui::Text(raw_model_path);
                break;
        }
    // ImGui::TextColored(ImVec4(0, 0.9f, 0.9f, 1), message.c_str());

    label_count++;
}
void AssetDoctor::Interface::DrawLabelMeshCount(NiAVObject *mesh)
{
    if (!mesh) { return; }
    auto geoms = NifUtil::Node::GetAllGeometries(mesh);
    auto size = ImGui::GetWindowSize();
    int last_label_count = label_count;
    float scale_x = size.x * .75;
    float scale_y = size.y * .75;
    for (auto* geom : geoms)
    {
        if (!geom) { continue; }

        auto* trishape = netimmerse_cast<BSTriShape*>(geom);
        if (!trishape)
        {
            continue; 
        }
        const char* name = trishape->name.c_str();

        uint16_t mesh_count= mesh_count_mode == MeshCountMode::Triangles ? trishape->GetTrishapeRuntimeData().triangleCount : trishape->GetTrishapeRuntimeData().vertexCount;
        auto mesh_count_mode_index = static_cast<int>(mesh_count_mode);
        std::string message = name && name[0] != '\0' ? std::format("{} : {} {}", name, mesh_count, mesh_count_units[mesh_count_mode_index]) : std::format("Trishape : {} {}", mesh_count, mesh_count_units[mesh_count_mode_index]);
        
        auto& geom_data = geom->GetGeometryRuntimeData();

        auto* model_data = geom_data.rendererData;
        RE::NiPoint3 target_pos = geom->world.translate;
        float x, y, z;
        float line_height = ImGui::GetTextLineHeightWithSpacing();
        float line_width = ImGui::CalcTextSize(message.c_str()).x;
        NiCamera::WorldPtToScreenPt3((float(*)[4])world_to_cam_matrix, *view_port, target_pos, x, y, z, 1e-5f);
        y *=  scale_y;
        x *= scale_x;
        if ((y <= 0.0f && x <= 0.0f) || (y >= size.y || x >= size.x))
        {
            x = size.x/2 - line_width; 
            y = size.y/2;
        }
        else 
        {
            x = MathUtil::Clamp(x, 0.0f, size.x - line_width);
            y = MathUtil::Clamp(y, 0.0f, size.y);
        }

        y += (y < size.y/2) ? (line_height * label_count) : -(line_height * label_count) ;
        ImGui::SetCursorScreenPos(ImVec2(x, y));
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(line_width,
                                                 line_height),
            ImColor(0.0f, 0.0f, 0.0f, 0.68f));
        
        ImGui::TextColored(ImVec4(0.9f,0.9f,0.9f,1), message.c_str());       
        label_count++;
    }
}
void AssetDoctor::Interface::DrawLabelTexturePath(NiAVObject *mesh)
{
    if (!mesh) { return; }
    auto geoms = NifUtil::Node::GetAllGeometries(mesh);
    auto size = ImGui::GetWindowSize();
    int last_label_count = label_count;
    float scale_x = size.x*.75;
    float scale_y = size.y*.75;
    
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


        std::unordered_set<std::string_view> paths;
        const char *raw_texture_path = texture_set->GetTexturePath(static_cast<BSTextureSet::Texture>(texture_slot_index));
        if (!raw_texture_path || raw_texture_path[0] == '\0' || paths.contains(raw_texture_path))
        {
            continue;
        }

        RE::NiPoint3 target_pos = geom->world.translate;
        float x, y, z;
        float line_height = ImGui::GetTextLineHeightWithSpacing();
        float line_width = ImGui::CalcTextSize(raw_texture_path).x;
        NiCamera::WorldPtToScreenPt3((float(*)[4])world_to_cam_matrix, *view_port, target_pos, x, y, z, 1e-5f);
        y *=  scale_y;
        x *= scale_x;
        if ((y <= 0.0f && x <= 0.0f) || (y >= size.y || x >= size.x))
        {
            x = size.x/2 - line_width; 
            y = size.y/2;
        }
        else 
        {
            x = MathUtil::Clamp(x, 0.0f, size.x - line_width);
            y = MathUtil::Clamp(y, 0.0f, size.y);
        }

        y += (y < size.y/2) ? (line_height * label_count) : -(line_height * label_count) ;
        std::string texture_path = raw_texture_path;
        auto status = Validator::ValidateTexturePath(texture_path);
        paths.emplace(raw_texture_path);

        ImGui::SetCursorScreenPos(ImVec2(x, y));
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(line_width,
                                                 line_height),
            ImColor(0.0f, 0.0f, 0.0f, 0.68f));
        switch(status)
        {
            case Validator::AssetStatus::Missing:
                ImGui::TextColored(ImVec4(0.9f,0,0,1), raw_texture_path);
                break;
            case Validator::AssetStatus::Archive:
                ImGui::TextColored(ImVec4(0,0.9f,0.9f,1), raw_texture_path);
                break;
            case Validator::AssetStatus::Loose:
                ImGui::TextColored(ImVec4(0.7f,0,0.9f,1), raw_texture_path);
                break;
            default:
                ImGui::Text(raw_texture_path);
                break;
        }
        label_count++;

        for (int i = 0; i < 8; i++)
        {
            if (i == texture_slot_index) { continue; }
            const char *raw_texture_path = texture_set->GetTexturePath(static_cast<BSTextureSet::Texture>(i));
            if (!raw_texture_path || raw_texture_path[0] == '\0')
            {
                continue;
            }

            Validator::AddTexturePath(raw_texture_path);
        }
        // const auto new_material = static_cast<BSLightingShaderMaterialBase*>(material->Create());

        // if (!new_material) { return; }

        // new_material->CopyMembers(material);
        // new_material->ClearTextures();
        // // new_material->OnLoadTextureSet(0, texture_set);
        // lighting_shader->SetMaterial(new_material, true);

        // lighting_shader->SetupGeometry(geom);
        // lighting_shader->FinishSetupGeometry(geom);

        // new_material->~BSLightingShaderMaterialBase();
        // RE::free(new_material);
        // geom->SetMaterialNeedsUpdate(true);
        
    }
    

    if (label_count == last_label_count)
    {
        const char*  empty_message = empty_texture_slot_names[texture_slot_index];

        ImGui::SetCursorScreenPos(size / 2.0f);
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(ImGui::CalcTextSize(empty_message).x,
                                                 ImGui::GetTextLineHeightWithSpacing()),
            ImColor(0.0f, 0.0f, 0.0f, 0.68f));

        ImGui::Text(empty_message);
    }
}

void AssetDoctor::Interface::DrawLabels()
{   
    if (!label_enabled) { return; }
    auto* pick_data = CrosshairPickData::GetSingleton();
    auto* ui = UI::GetSingleton();
    if (!pick_data || !world_to_cam_matrix || !view_port || !ui) { return; }
    label_count = 0;
    auto *pick_ref = pick_data->target.get().get();

    if (!pick_ref)
    {
        //raycast as backup
        float ray_dist;
        NiPoint3 hit_pos; 
        auto *mesh = CastMeshRayFromCamera(raycast_distance, &ray_dist, &hit_pos);
        if (!mesh) { return; }
        // DrawLabelTexture(mesh);
        DrawLabel(mesh);
        return;
        
    }
    
    DrawLabel(pick_ref);






    //group 
    // auto *tes = TES::GetSingleton();

    // tes->ForEachReferenceInRange(pick_ref, 50.0f, [&](RE::TESObjectREFR &b_ref)
    //                              {
	// 			if (const auto base = b_ref.GetBaseObject(); base && b_ref.Is3DLoaded()) {
	// 				DrawLabel(&b_ref);
	// 			}
	// 			return RE::BSContainer::ForEachResult::kContinue; });
}
