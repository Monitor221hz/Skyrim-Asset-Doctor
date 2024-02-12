#pragma once

#include <unordered_set>
#include <d3d11.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "imgui_internal.h"

namespace AssetDoctor
{
    class Interface
    {
        public:

        static inline std::atomic missing_texture_log_write { false };

        static inline std::atomic missing_mesh_log_write { false };

        static inline std::atomic reset_queued { false };

        static void Draw();

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

        static void DrawTextureLog();
        static void DrawMeshLog();

        static inline std::unordered_set<std::string> missing_texture_paths; 
        static inline std::unordered_set<std::string> missing_mesh_paths;

        static inline std::atomic<int> maxLineBuffer = 250;

    };
}