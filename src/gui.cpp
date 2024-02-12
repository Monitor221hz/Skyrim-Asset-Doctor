#include "gui.h"

void AssetDoctor::Interface::Draw()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

    ImGui::Begin("##Main", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);

    DrawMeshLog();
    DrawTextureLog();

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
