#include "event.h"
#include "util.h"
using namespace RE;


RE::BSEventNotifyControl AssetDoctor::ObjectLoadEventHandler::ProcessEvent(const RE::TESObjectLoadedEvent *a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent> *a_eventSource)
{
    auto cont = RE::BSEventNotifyControl::kContinue;

    if (!a_event->loaded) { return cont; }

    auto* form = TESForm::LookupByID(a_event->formID);

    auto* ref = form->As<TESObjectREFR>();

    if (!ref) { return cont; }

    auto* ref3d = ref->Get3D2();

    if (!ref3d) { return cont; }

    auto geoms = NifUtil::Node::GetAllGeometries(ref3d);

    for (auto* geom : geoms)
    {
        if (!geom) { continue; }
        auto effect_ptr = geom->GetGeometryRuntimeData().properties[BSGeometry::States::kEffect];

        auto* effect = effect_ptr.get();

        if (!effect) { continue; }

        auto* lighting_shader = netimmerse_cast<BSLightingShaderProperty *>(effect);
        if (!lighting_shader) { continue; }

        auto* base_material = lighting_shader->material;
        if (!base_material) { continue; }

        auto* material = static_cast<BSLightingShaderMaterialBase*>(static_cast<BSShaderMaterial*>(base_material));
        if (!material) { continue; }

        auto texture_set_ptr = material->GetTextureSet();

        auto* texture_set = texture_set_ptr.get();
        if (!texture_set) { continue; }

        for(int i = 0; i < 8; i++)
        {
            const char* raw_texture_path = texture_set->GetTexturePath(static_cast<BSTextureSet::Texture>(i));
            if (!raw_texture_path || raw_texture_path[0] == '\0') { continue; }

            Validator::AddTexturePath(raw_texture_path);
        }
        
    }
    
    Validator::ValidateTexturePaths();

    auto* bound_obj = ref->GetObjectReference();
    if (!bound_obj) { return cont; }

    auto* model = bound_obj->As<TESModel>();
    if (!model) { return cont; }

    const char* raw_model_path = model->GetModel();
    if (!raw_model_path || raw_model_path[0] == '\0') { return cont; }

    Validator::AddMeshPath(raw_model_path);

    Validator::ValidateMeshPaths();

    return cont;
}