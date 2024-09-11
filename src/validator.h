#pragma once 

#include <algorithm>
#include <string>
#include <shared_mutex>
#include <unordered_set>
#include <filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/async.h" 
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/dup_filter_sink.h"
#include "gui.h"
namespace AssetDoctor
{
    class Validator
    {


        public:

        enum class AssetStatus
        {
            Invalid,
            Missing, 
            Archive,
            Loose
        };
        static void Setup()
        {
            spdlog::init_thread_pool(8192, 1);
	        std::filesystem::path absolute_path = std::filesystem::current_path() / "data" / "Asset Doctor Report.txt"; 
            std::vector<spdlog::sink_ptr> sinks; 
            sinks.push_back(std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::seconds(5)));
            sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(absolute_path.string(), true)); 
	        asset_logger = std::make_shared<spdlog::async_logger>("async asset logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            asset_logger->set_pattern("%l: %v");
        }
        static void AddTexturePath(const char* path);
        static void AddMeshPath(const char* path);


        static bool ValidateTexturePaths();
        static bool ValidateMeshPaths();

        static AssetStatus ValidateTexturePath(std::string& texture_path);
        static AssetStatus ValidateTexturePath(TESObjectREFR* refr, std::string& texture_path); 
        static AssetStatus ValidateMeshPath(std::string& mesh_path);
        static AssetStatus ValidateMeshPath(TESObjectREFR* refr, std::string& mesh_path); 
        static AssetStatus ValidateMeshPath(TESModel* model, std::string& mesh_path); 

        static void ValidateReference(TESObjectREFR* ref); 
        static void ValidateReferenceMesh(TESObjectREFR* ref); 
        static void ValidateNiObject(TESObjectREFR* ref, NiAVObject* niObject); 
        static void ValidateLightingShaderProperty(TESObjectREFR* refr, BSLightingShaderProperty* property); 
        static void ValidateEffectShaderProperty(TESObjectREFR* refr, BSEffectShaderProperty* property); 
        static void ValidateBoundObject(TESBoundObject* bound_object); 

        static void EnablePBRSupport() { pbr_mode.store(true); }
        static void EnableAutoParallaxMode() { auto_parallax_mode.store(true); }

        static bool IsPBRMode() { return pbr_mode; }
        static bool IsAutoParallaxMode() { return false; }

        private:
            using Lock = std::shared_mutex;
            using ReadLocker = std::shared_lock<Lock>;
            using WriteLocker = std::unique_lock<Lock>;
            static inline Lock q_lock;
            static inline Lock path_lock; 

            static inline std::unordered_set<std::string> texture_paths;
            static inline std::unordered_set<std::string> mesh_paths;
            static inline std::shared_ptr<spdlog::logger> asset_logger;

            static inline std::unordered_set<std::string> missing_asset_paths; 
            static inline std::unordered_set<std::string> unk_missing_asset_paths; 

            static inline std::atomic pbr_mode = false; 
            static inline std::atomic auto_parallax_mode = false; 

            static bool AddMissingAsset(std::string a_path); 
            static bool AddUnkMissingAsset(std::string a_path); 
            
            
    };
}