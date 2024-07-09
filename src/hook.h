#pragma once

#include "SimpleIni.h"
#include <d3d11.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "imgui_internal.h"

#include "gui.h"

namespace ImGui::Renderer
{
	inline std::atomic initialized{ false };

	namespace DisplayTweaks
	{
		inline float resolutionScale{ 1.0f };
		inline bool  borderlessUpscale{ false };
	}

	float GetResolutionScale();

	void LoadSettings(const CSimpleIniA& a_ini);
	void Install();
}

namespace AssetDoctor
{
	struct Load3DHook //used
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> refVtbl { RE::VTABLE_TESObjectREFR[0] }; 
			_Load3D = refVtbl.write_vfunc(0x6A, Load3D); 
			SKSE::log::info("Hook installed: Load 3D");
		}

		private:
		static NiAVObject* Load3D(RE::TESObjectREFR* a_refr, bool a_backgroundLoading); 
		static inline REL::Relocation<decltype(Load3D)> _Load3D; 
	};
	struct PostAttachHook
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> niObjVtbl { RE::VTABLE_NiAVObject[0] }; 
			_PostAttachUpdate = niObjVtbl.write_vfunc(0x2F, PostAttachUpdate); 
			SKSE::log::info("Hook installed: Post Attach Update"); 
		}
		private:
		static void PostAttachUpdate(NiAVObject* niObj); 
		static inline REL::Relocation<decltype(PostAttachUpdate)> _PostAttachUpdate; 
	};
	struct TextureSetHook
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> texture_set_vtbl { RE::VTABLE_BSTextureSet[0] }; 
			_SetTexture = texture_set_vtbl.write_vfunc(0x26, SetTexture); 
			SKSE::log::info("Hook installed: Set Texture"); 
		}
		private:
		static void SetTexture(BSTextureSet* a_texture_set, BSTextureSet::Texture a_texture, NiSourceTexture* a_src_texture); 
		static inline REL::Relocation<decltype(SetTexture)> _SetTexture; 
	};
	struct FinishSetupGeometryHook //for textures
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> shader_vtbl { RE::VTABLE_BSLightingShaderProperty[0] }; 
			_FinishSetupGeometry = shader_vtbl.write_vfunc(0x28, FinishSetupGeometry); 
			SKSE::log::info("Hook installed: Finish Setup Geometry"); 
		}
		private:
		static bool FinishSetupGeometry(BSLightingShaderProperty* a_property, BSGeometry* a_geometry);
		static inline REL::Relocation<decltype(FinishSetupGeometry)> _FinishSetupGeometry; 
	};
	struct SetModelHook //used only for effect and skeleton nifs
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> model_vtbl { RE::VTABLE_TESModel[0] }; 
			_SetModel = model_vtbl.write_vfunc(0x5, SetModel); 
			SKSE::log::info("Hook installed: Set Model Hook");
		}
		static void SetModel(TESModel* a_model, const char* a_path); 
		static inline REL::Relocation<decltype(SetModel)> _SetModel; 
	};
}