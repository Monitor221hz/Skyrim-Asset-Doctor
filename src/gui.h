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

        static inline std::atomic reset_queued { false };
        static void Load()
        {
            world_to_cam_matrix = RELOCATION_ID(519579, 406126).address(); // 2F4C910, 2FE75F0
            view_port = (RE::NiRect<float> *)RELOCATION_ID(519618, 406160).address();
            raycast_distance = Settings::GetTargetRaycastDistance();
        }
        static void Draw();

        static inline uintptr_t world_to_cam_matrix;
        static inline RE::NiRect<float> *view_port;

        static void AddMissingTexture(std::string a_string)
        {
            missing_texture_log_write.store(false);
            if (missing_texture_paths.size() > maxLineBuffer) { missing_texture_paths.clear(); }
            missing_texture_paths.emplace(a_string);
            missing_texture_log_write.store(true);
        }

        static void AddMissingMesh(std::string a_string)
        {
            missing_mesh_log_write.store(false);
            if (missing_mesh_paths.size() > maxLineBuffer) { missing_mesh_paths.clear(); }
            missing_mesh_paths.emplace(a_string);
            missing_mesh_log_write.store(true);
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

        static void CycleTextureSlot() 
        { 
            texture_index += 1;
            texture_index = texture_index > 7 ? 0 : texture_index;
        }

        private:
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

        static bool StartsWith(std::string &str, std::string prefix)
        {
            return (str.compare(0, prefix.length(), prefix) == 0);
        }

        static void DrawLabelVertices(NiAVObject* mesh);
        static void DrawLabelTexture(TESObjectREFR* refr);
        static void DrawLabelTexture(NiAVObject* mesh);
        static void DrawTextureLog();
        static void DrawMeshLog();
        static void DrawLabels();

        static inline std::unordered_set<std::string> missing_texture_paths; 
        static inline std::unordered_set<std::string> missing_mesh_paths;

        static inline std::atomic<int> maxLineBuffer = 45;
        static inline int texture_index = 0; 

        static inline std::atomic label_enabled = true;
        static inline std::atomic enabled = true;

        static inline int label_count = 1; 

        static inline float font_scale = 1.5f;
        static inline float raycast_distance = 2048.0f;

    };
}