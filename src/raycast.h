#pragma once 
#include "util.h"
using namespace RE;
namespace AssetDoctor
{
    static RE::TESObjectREFR *CastRayAtPoint(RE::NiPoint3& a_pos, float a_range)
    {
    RE::NiPoint3 rayStart = a_pos;
	RE::NiPoint3 rayEnd = a_pos;
	rayStart.z += a_range;
	rayEnd.z -= a_range;
	auto havokWorldScale = RE::bhkWorld::GetWorldScale();
	RE::bhkPickData pick_data;
	pick_data.rayInput.from = rayStart * havokWorldScale;
	pick_data.rayInput.to = rayEnd * havokWorldScale;
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (!pc) {
		return nullptr; 
	}
	if (!pc->GetParentCell() || !pc->GetParentCell()->GetbhkWorld()) {
		return nullptr; 
	}
	pc->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
	if (pick_data.rayOutput.HasHit()) {
		RE::NiPoint3 hitpos = rayStart + (rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
        a_pos = hitpos; // update the position to the hit position
        auto collidable = pick_data.rayOutput.rootCollidable;
        if (collidable)
        {
            RE::TESObjectREFR *ref = RE::TESHavokUtilities::FindCollidableRef(*collidable);
            if (ref)
            {
                return ref;
            }
        }
	}
	return nullptr; 
    }
    static RE::TESObjectREFR *CastRefRayFromActor(RE::Actor *a_actor, RE::NiPoint3 a_rayEnd, float a_castPos, float *ret_rayDist)
    {
        auto havokWorldScale = RE::bhkWorld::GetWorldScale();
        RE::bhkPickData pick_data;
        RE::NiPoint3 rayStart = a_actor->GetPosition();
        float castHeight = a_actor->GetHeight() * a_castPos;
        rayStart.z += castHeight; // cast from center of actor
        /*Setup ray*/
        pick_data.rayInput.from = rayStart * havokWorldScale;
        pick_data.rayInput.to = a_rayEnd * havokWorldScale;

        /*Setup collision filter, ignoring the actor.*/
        uint32_t collisionFilterInfo = 0;
        a_actor->GetCollisionFilterInfo(collisionFilterInfo);
        uint16_t collisionGroup = collisionFilterInfo >> 16;
        pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionGroup) << 16) | static_cast<uint32_t>(RE::COL_LAYER::kCharController);
        /*Do*/
        a_actor->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
        if (pick_data.rayOutput.HasHit())
        {
            RE::NiPoint3 hitpos = rayStart + (a_rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
            if (ret_rayDist)
            {
                *ret_rayDist = hitpos.GetDistance(rayStart);
            }

            auto collidable = pick_data.rayOutput.rootCollidable;
            if (collidable)
            {
                RE::TESObjectREFR *ref = RE::TESHavokUtilities::FindCollidableRef(*collidable);
                if (ref)
                {
                    return ref;
                }
            }
        }
        return nullptr;
    }
    static NiAVObject* CastMeshRayFromActor(RE::Actor *a_actor, RE::NiPoint3 a_rayEnd, float a_castPos, float *ret_rayDist)
    {
        auto havokWorldScale = RE::bhkWorld::GetWorldScale();
        RE::bhkPickData pick_data;
        RE::NiPoint3 rayStart = a_actor->GetPosition();
        float castHeight = a_actor->GetHeight() * a_castPos;
        rayStart.z += castHeight; // cast from center of actor
        /*Setup ray*/
        pick_data.rayInput.from = rayStart * havokWorldScale;
        pick_data.rayInput.to = a_rayEnd * havokWorldScale;

        /*Setup collision filter, ignoring the actor.*/
        uint32_t collisionFilterInfo = 0;
        a_actor->GetCollisionFilterInfo(collisionFilterInfo);
        uint16_t collisionGroup = collisionFilterInfo >> 16;
        pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionGroup) << 16) | static_cast<uint32_t>(RE::COL_LAYER::kCharController);
        /*Do*/
        a_actor->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
        if (pick_data.rayOutput.HasHit())
        {
            RE::NiPoint3 hitpos = rayStart + (a_rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
            if (ret_rayDist)
            {
                *ret_rayDist = hitpos.GetDistance(rayStart);
            }

            auto collidable = pick_data.rayOutput.rootCollidable;
            if (collidable)
            {
                auto *obj = RE::TESHavokUtilities::FindCollidableObject(*collidable);
                if (obj)
                {
                    return obj;
                }
            }
        }
        return nullptr;        
    }
    static NiAVObject* CastMeshRayFromCamera(float rayDist, float *ret_rayDist, NiPoint3* hitPos)
    {
        auto havokWorldScale = RE::bhkWorld::GetWorldScale();
        RE::bhkPickData pick_data;


        auto* camera = Main::WorldRootCamera(); 
        if (!camera) { return nullptr; }
        RE::NiPoint3 rayOrigin = camera->world.translate;
        RE::NiPoint3 rayDir = NiPoint3(camera->world.rotate.entry[0][0],
                                       camera->world.rotate.entry[1][0],
                                       camera->world.rotate.entry[2][0]);
        MathUtil::Normalize(rayDir);

        /*Setup ray*/
        auto rayStart = rayOrigin * havokWorldScale; 
        auto rayEnd = (rayOrigin + rayDir * rayDist) * havokWorldScale;

        pick_data.rayInput.from = rayStart;
        pick_data.rayInput.to = rayEnd;

        /*Setup collision filter, ignoring the player and any actors*/
        uint32_t collisionFilterInfo = 0;
        auto* player = PlayerCharacter::GetSingleton();
        if (!player || !player->Is3DLoaded()) { return nullptr; }

        player->GetCollisionFilterInfo(collisionFilterInfo);
        uint16_t collisionGroup = collisionFilterInfo >> 16;
        pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionGroup) << 16) | static_cast<uint32_t>(RE::COL_LAYER::kCharController);

        /*Do*/
        auto* parentCell = player->GetParentCell();
        if (!parentCell) { return nullptr; }
        auto* bhkWorld =  parentCell->GetbhkWorld();
        if (!bhkWorld) { return nullptr; }
        
        bhkWorld->PickObject(pick_data);
        // SKSE::log::info("Ray cast from x {} y {} z {} to x {} y {} z {}", rayStart.x, rayStart.y, rayStart.z, rayEnd.x, rayEnd.y, rayEnd.z);
        if (pick_data.rayOutput.HasHit())
        {

            RE::NiPoint3 hitPos = rayStart + (rayDir - rayStart) * pick_data.rayOutput.hitFraction;
            if (ret_rayDist)
            {
                *ret_rayDist = hitPos.GetDistance(rayStart);
            }

            auto collidable = pick_data.rayOutput.rootCollidable;
            if (collidable)
            {
                
                auto *obj = RE::TESHavokUtilities::FindCollidableObject(*collidable);
                if (obj)
                {
                    

                    return obj;
                }
            }
        }
        return nullptr;        
    }    
}