#pragma once 
#include "imgui_internal.h"
#include "SimpleIni.h"

namespace AssetDoctor
{
    class Settings
    {
        private:
            static ImVec4 ColorFromHex(int hexValue)
            {
                ImVec4 vec;
                vec.x = ((hexValue >> 16) & 0xFF) / 255.0;
                vec.y = ((hexValue >> 8) & 0xFF) / 255.0;
                vec.z = ((hexValue) & 0xFF) / 255.0;
                vec.w = 1.0f;
                return vec;
            }

        public: 

        struct HotKeyConfig
        {
            uint32_t toggle_gui; 
            uint32_t toggle_labels; 
            uint32_t cycle_labels;
            uint32_t cycle_label_type; 
        };

        struct TargetConfig
        {
            float raycast_distance = 2048.0f;

        };
        struct LogDisplayConfig
        {
            ImVec4 missing_mesh_text_color; 
            ImVec4 missing_texture_text_color; 
        };
        struct LabelDisplayConfig
        {
            ImVec4 archive_text_color; 
            ImVec4 loose_color; 
            ImVec4 missing_color; 
            ImVec4 too_many_vertices_color; 

        };

        static void Load()
        {
            constexpr auto path = L"Data/SKSE/Plugins/Asset_Doctor/Settings.ini";

            CSimpleIniA ini; 
            ini.SetUnicode();
            const auto rc = ini.LoadFile(path);
            if (rc < 0)
            {
                SKSE::log::error("Couldn't read Settings.ini");
                return;
            }
            hot_key_options.cycle_label_type = (uint32_t)ini.GetLongValue("Hotkeys", "Cycle Label Sub-Type", 64);  
            hot_key_options.cycle_labels = (uint32_t)ini.GetLongValue("Hotkeys", "Cycle Label Type", 65);
            hot_key_options.toggle_gui = (uint32_t)ini.GetLongValue("Hotkeys", "Toggle GUI", 66);     
            hot_key_options.toggle_labels = (uint32_t)ini.GetLongValue("Hotkeys", "Toggle Labels", 62); 
            target_options.raycast_distance = static_cast<float>(ini.GetDoubleValue("Targeting", "Raycast Distance", 2048.0f));

            log_display_options.missing_mesh_text_color = ColorFromHex(ini.GetLongValue("Log Interface", "Missing Mesh Text Color")); 
            log_display_options.missing_texture_text_color = ColorFromHex(ini.GetLongValue("Log Interface", "Missing Texture Text Color")); 

            label_display_options.archive_text_color = ColorFromHex(ini.GetLongValue("Labels", "Archive Asset Text Color")); 
            label_display_options.loose_color = ColorFromHex(ini.GetLongValue("Labels", "Loose Asset Text Color")); 
            label_display_options.missing_color = ColorFromHex(ini.GetLongValue("Labels", "Missing Asset Text Color")); 
        }
        [[nodiscard]] static uint32_t GetCycleLabelTypeKeyCode() { return hot_key_options.cycle_label_type; }
        [[nodiscard]] static uint32_t GetCycleLabelKeyCode() { return hot_key_options.cycle_labels; }
        [[nodiscard]] static uint32_t GetToggleGUICode() { return hot_key_options.toggle_gui; }
        [[nodiscard]] static uint32_t GetToggleLabelCode() { return hot_key_options.toggle_labels; }
        [[nodiscard]] static float GetTargetRaycastDistance() { return target_options.raycast_distance; }

        [[nodiscard]] static ImVec4 GetMissingMeshTextColor() { return log_display_options.missing_mesh_text_color; }
        [[nodiscard]] static ImVec4 GetMissingTextureTextColor() { return log_display_options.missing_texture_text_color; }
        [[nodiscard]] static ImVec4 GetArchiveAssetTextColor() { return label_display_options.archive_text_color; }
        [[nodiscard]] static ImVec4 GetLooseAssetTextColor() { return label_display_options.loose_color; }
        [[nodiscard]] static ImVec4 GetMissingAssetTextColor() { return label_display_options.missing_color; }

        private:
        static inline HotKeyConfig hot_key_options; 
        static inline TargetConfig target_options;
        static inline LogDisplayConfig log_display_options; 
        static inline LabelDisplayConfig label_display_options; 

    };
}