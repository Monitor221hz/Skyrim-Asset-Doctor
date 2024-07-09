#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <unordered_set>
#include <d3d11.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "imgui_internal.h"

#include "raycast.h"
#include "validator.h"
#include "util.h"
#include "settings.h"
namespace AssetDoctor
{
    class Interface
    {
        public:
        
        static inline std::atomic missing_texture_log_write { false };

        static inline std::atomic missing_mesh_log_write { false };

        static inline std::atomic missing_mesh_write { false }; 

        static inline std::atomic reset_queued { false };
        static void Load()
        {
            world_to_cam_matrix = RELOCATION_ID(519579, 406126).address(); // 2F4C910, 2FE75F0
            view_port = (RE::NiRect<float> *)RELOCATION_ID(519618, 406160).address();
            raycast_distance = Settings::GetTargetRaycastDistance();
        }

        static void Reload()
        {
            auto& io = ImGui::GetIO(); 
            io.Fonts->Clear(); 
			font = io.Fonts->AddFontFromFileTTF(R"(Data\SKSE\Plugins\Asset_Doctor\Fonts\)" "Default_Font.ttf", 20.f);
            io.Fonts->Build(); 
            ImGui_ImplDX11_InvalidateDeviceObjects();
		    ImGui_ImplDX11_CreateDeviceObjects();
        }
        static void Draw();

        static inline uintptr_t world_to_cam_matrix;
        static inline RE::NiRect<float> *view_port;
        static void AddMissingMesh(FormID formID, std::string& path)
        {
            missing_mesh_write.store(true); 
            missing_mesh_data.emplace_back(std::make_pair(formID, path)); 
            missing_mesh_write.store(false); 
        }
        static void IncrementMissingMeshes()
        {
            missing_mesh_count.store(missing_mesh_count+1); 
        }
        static void IncrementMissingTextures()
        {
            missing_texture_count.store(missing_texture_count+1); 
        }
        static void QueueReset()
        {
            reset_queued.store(true);
        }

        static bool IsEnabled() { return enabled; }

        static void SetEnabled(bool a_enable) { enabled.store(a_enable); }

        static void SetLabelEnabled(bool a_enable) { label_enabled.store(a_enable); }

        static void Toggle() { enabled.store(!enabled); }

        static void ToggleLabels() { label_enabled.store(!label_enabled); }

        static void CycleLabelTextureSlot() 
        { 
            texture_slot_index += 1;
            texture_slot_index = texture_slot_index > 7 ? 0 : texture_slot_index;
        }

        static void CycleLabelMeshCountMode()
        {
            int mesh_count_mode_index = static_cast<int>(mesh_count_mode);
            mesh_count_mode_index +=1; 
            mesh_count_mode_index = mesh_count_mode_index >= static_cast<int>(MeshCountMode::Total) ? 0 : mesh_count_mode_index;
            mesh_count_mode = static_cast<MeshCountMode>(mesh_count_mode_index);
            
        }   

        static void CycleLabelMode()
        {
            int label_mode_index = static_cast<int>(label_mode);
            label_mode_index += 1;
            label_mode_index = label_mode_index >= static_cast<int>(LabelMode::Total) ? 0 : label_mode_index;
            label_mode = static_cast<LabelMode>(label_mode_index);
        }

        static void CycleLabelType()
        {
            switch (label_mode)
            {
            case LabelMode::TexturePaths:
                CycleLabelTextureSlot();
                break;
            case LabelMode::MeshCount:
                CycleLabelMeshCountMode();
                break;
            default:
                break;
            }
        }

        private:
            using Lock = std::shared_mutex;
            using ReadLocker = std::shared_lock<Lock>;
            using WriteLocker = std::unique_lock<Lock>;

            static inline Lock missing_labels_lock; 

            enum class MeshCountMode
            {
                Triangles,
                Vertices,
                Total,
            };
            enum class LabelMode
            {
                TexturePaths,
                MeshPaths,
                MeshCount,
                Total
            };
            static inline const char *empty_texture_slot_names[] = {
                "empty diffuse",
                "empty normal(gloss)",
                "empty subsurface/tint",
                "empty glow/detail",
                "empty env(cube)",
                "empty env mask",
                "empty inner-layer/face-tint",    
                "empty ss/specular/backlight",
                "empty"};

            static inline const char* mesh_count_units[] = {
                "tris",
                "verts"
            };
        static bool StartsWith(std::string &str, std::string prefix)
        {
            return (str.compare(0, prefix.length(), prefix) == 0);
        }
        static void DrawLabel(NiAVObject* mesh);
        static void DrawLabel(TESObjectREFR* refr);
        static void DrawLabelMeshPath(TESObjectREFR* refr);
        static void DrawLabelMeshCount(NiAVObject* mesh);
        static void DrawLabelTexturePath(TESObjectREFR* refr);
        static void DrawLabelTexturePath(NiAVObject* mesh);
        static void DrawTextureCount(); 
        static void DrawMeshCount(); 
        static void DrawCounters(); 
        static void DrawTextureLog();
        static void DrawMeshLog();
        static void DrawLabels();
        static void DrawMissingMeshLabel(FormID formID, std::string path); 
        static void DrawMissingMeshLabels(); 

        static inline std::unordered_set<std::string> missing_texture_paths; 
        static inline std::unordered_set<std::string> missing_mesh_paths;

        static inline std::vector<std::pair<FormID, std::string>> missing_mesh_data;

        static inline float missing_label_max_width; 

        static inline std::atomic<int> max_line_buffer = 45;
        static inline std::atomic<uint64_t> missing_texture_count = 0; 
        static inline std::atomic<uint64_t> missing_mesh_count = 0; 


        static inline int texture_slot_index = 0; 

        static inline LabelMode label_mode = LabelMode::TexturePaths;
        static inline MeshCountMode mesh_count_mode = MeshCountMode::Triangles;

        static inline std::atomic label_enabled = true;
        static inline std::atomic enabled = true;

        static inline int label_count = 1; 

        static inline float font_scale = 1.5f;
        static inline float raycast_distance = 2048.0f;
        static inline const char* empty = "";
        static inline ImFont* font; 
    };
}