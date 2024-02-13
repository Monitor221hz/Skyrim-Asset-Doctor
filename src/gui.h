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


        private:


        static bool StartsWith(std::string& str,std::string prefix)
        {
            return (str.compare(0, prefix.length(), prefix) == 0);
        }

        static void DrawLabel(TESObjectREFR* refr);
        static void DrawLabel(NiAVObject* mesh);
        static void DrawTextureLog();
        static void DrawMeshLog();
        static void DrawLabels();

        static inline std::unordered_set<std::string> missing_texture_paths; 
        static inline std::unordered_set<std::string> missing_mesh_paths;

        static inline std::atomic<int> maxLineBuffer = 250;

        static inline int label_count = 1; 

        static inline float font_scale = 1.5f;

    };
}