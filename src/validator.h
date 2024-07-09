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
            asset_logger->set_pattern("[%x] %l: %v");
            // 	        std::filesystem::path absolute_path = std::filesystem::current_path() / "data\\SKSE\\Plugins\\Asset_Doctor" / "Asset Report.txt"; 
            // std::vector<spdlog::sink_ptr> sinks; 
            // sinks.push_back(std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::seconds(5)));
            // sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(absolute_path, true)); 
	        // asset_logger = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", absolute_path.string(), true);
            // asset_logger->set_pattern("[%x] %l: %v");
        }
        static void AddTexturePath(const char* path);
        static void AddMeshPath(const char* path);


        static bool ValidateTexturePaths();
        static bool ValidateMeshPaths();

        static AssetStatus ValidateTexturePath(std::string& texture_path);
        static AssetStatus ValidateMeshPath(std::string& mesh_path);
        static AssetStatus ValidateMeshPath(TESObjectREFR* refr, std::string& mesh_path); 

        static void ValidateReference(TESObjectREFR* ref); 
        static void ValidateReferenceMesh(TESObjectREFR* ref); 
        static void ValidateNiObject(NiAVObject* niObject); 
        static void ValidateLightingShaderProperty(BSLightingShaderProperty* property); 
        static void ValidateBoundObject(TESBoundObject* bound_object); 
        
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

            static void AddMissingAsset(std::string a_path); 
    };
}