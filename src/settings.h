#pragma once 
#include "SimpleIni.h"

namespace AssetDoctor
{
    class Settings
    {
        public: 

        struct HotKeyConfig
        {
            uint32_t toggle_gui; 
            uint32_t toggle_labels;
            uint32_t cycle_texture_slot; 
        };

        struct TargetConfig
        {
            float raycast_distance = 2048.0f;

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
            hot_key_options.cycle_texture_slot = (uint32_t)ini.GetLongValue("Hotkeys", "Cycle Texture Slot", 64);  
            hot_key_options.toggle_labels = (uint32_t)ini.GetLongValue("Hotkeys", "Toggle Labels", 65);
            hot_key_options.toggle_gui = (uint32_t)ini.GetLongValue("Hotkeys", "Toggle GUI", 66);     
            target_options.raycast_distance = static_cast<float>(ini.GetDoubleValue("Targeting", "Raycast Distance", 2048.0f));
        }

        [[nodiscard]] static uint32_t GetToggleLabelKeyCode() { return hot_key_options.toggle_labels; }
        [[nodiscard]] static uint32_t GetToggleGUICode() { return hot_key_options.toggle_gui; }
        [[nodiscard]] static float GetTargetRaycastDistance() { return target_options.raycast_distance; }

        private:
        static inline HotKeyConfig hot_key_options; 
        static inline TargetConfig target_options;

    };
}