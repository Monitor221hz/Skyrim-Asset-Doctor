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

    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Missing Textures");
    ImGui::BeginChild("Scrolling");

    if (missing_texture_log_write)
    {
        for (auto &path : missing_texture_paths)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), path.c_str());
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

    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Missing Meshes");
    ImGui::BeginChild("Scrolling");

    if (missing_mesh_log_write)
    {
        for (auto &path : missing_mesh_paths)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), path.c_str());
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();
}
void AssetDoctor::Interface::DrawLabel(TESObjectREFR *refr)
{
    DrawLabel(refr->Get3D2());
}
void AssetDoctor::Interface::DrawLabel(NiAVObject *mesh, NiPoint3& point)
{
    if (!mesh) { return; }
    auto geoms = NifUtil::Node::GetAllGeometries(mesh);
    auto size = ImGui::GetWindowSize();
    point *= bhkWorld::GetWorldScale();
    for (auto* geom : geoms)
    {
        if (!geom) { continue; }

        auto* geom_name = geom->name.c_str();

        if (geom_name && geom_name[0] != '\0') { SKSE::log::debug("Geom {}", geom_name); }
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
        const char *raw_texture_path = texture_set->GetTexturePath(BSTextureSet::Texture::kDiffuse);
        if (!raw_texture_path || raw_texture_path[0] == '\0' || paths.contains(raw_texture_path))
        {
            continue;
        }
        RE::NiPoint3 target_pos = mesh->world.translate;
        float x, y, z;
        float line_height = ImGui::GetTextLineHeightWithSpacing();

        NiCamera::WorldPtToScreenPt3((float(*)[4])world_to_cam_matrix, *view_port, target_pos, x, y, z, 1e-5f);
        y =  960.0f * y + 120.0f- (line_height * label_count);
        x = 540.0f * x + 650.0f;
        y = MathUtil::Clamp(y, 0.0, size.y);
        x = MathUtil::Clamp(x, 0.0, size.x);


        Validator::AddTexturePath(raw_texture_path);
        paths.emplace(raw_texture_path);

        ImGui::SetCursorScreenPos(ImVec2(x, y));
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(ImGui::CalcTextSize(raw_texture_path).x,
                                                 line_height),
            ImColor(0.0f, 0.0f, 0.0f, 0.68f));

        ImGui::Text(raw_texture_path);
        label_count++;
    }

}
void AssetDoctor::Interface::DrawLabels()
{
    auto* pick_data = CrosshairPickData::GetSingleton();
    auto* ui = UI::GetSingleton();
    if (!pick_data || !world_to_cam_matrix || !view_port || !ui) { return; }
    label_count = 1;
    auto *pick_ref = pick_data->target.get().get();

    if (!pick_ref)
    {
        //raycast as backup
        float ray_dist;
        NiPoint3 hit_pos; 
        auto *mesh = CastMeshRayFromCamera(1024.0f, &ray_dist, &hit_pos);
        if (!mesh) { return; }
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
