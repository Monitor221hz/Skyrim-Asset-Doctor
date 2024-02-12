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

        static void AddTexturePath(const char* path);
        static void AddMeshPath(const char* path);


        static void ValidateTexturePaths();
        static void ValidateMeshPaths();


        private:
            using Lock = std::shared_mutex;
            using ReadLocker = std::shared_lock<Lock>;
            using WriteLocker = std::unique_lock<Lock>;
            static inline Lock lock;

            static inline std::unordered_set<std::string> texture_paths;
            static inline std::unordered_set<std::string> mesh_paths;

    };
}