#pragma once 

#include <algorithm>
#include <string>
#include <shared_mutex>
#include <unordered_set>
#include <filesystem>

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

        static void AddTexturePath(const char* path);
        static void AddMeshPath(const char* path);


        static void ValidateTexturePaths();
        static void ValidateMeshPaths();


        static AssetStatus ValidateTexturePath(std::string& texture_path);
        static AssetStatus ValidateMeshPath(std::string& mesh_path); 

        private:
            using Lock = std::shared_mutex;
            using ReadLocker = std::shared_lock<Lock>;
            using WriteLocker = std::unique_lock<Lock>;
            static inline Lock lock;

            static inline std::unordered_set<std::string> texture_paths;
            static inline std::unordered_set<std::string> mesh_paths;

    };
}