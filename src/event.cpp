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

RE::BSEventNotifyControl AssetDoctor::HotKeyHandler::ProcessEvent(RE::InputEvent *const *a_event, RE::BSTEventSource<RE::InputEvent *> *)
{
    auto cont = RE::BSEventNotifyControl::kContinue;
    for (InputEvent *e = *a_event; e; e = e->next)
    {

        if (!(e->eventType.get() == INPUT_EVENT_TYPE::kButton))
            return cont;

        ButtonEvent *a_event = e->AsButtonEvent();
        uint32_t keyMask = a_event->idCode;
        uint32_t key_code;
        auto device = a_event->device.get();

        if (!a_event->IsUp())
            continue;

        if (device == INPUT_DEVICE::kMouse)
        {
            key_code = static_cast<int>(KeyUtil::KBM_OFFSETS::kMacro_NumKeyboardKeys) + keyMask;
        }
        else if (device == INPUT_DEVICE::kGamepad)
        {
            key_code = KeyUtil::Interpreter::GamepadMaskToKeycode(keyMask);
        }
        else
        {
            key_code = keyMask;
        }
        auto control = std::string(a_event->QUserEvent().c_str());

        if (key_code == cycle_label_hot_key)
        {
            Interface::CycleLabelMode();
        }
        else if (key_code == gui_hot_key)
        {
            Interface::Toggle();
        }
        else if (key_code == label_hot_key)
        {
            Interface::ToggleLabels(); 
        }
        else if (key_code == cycle_label_type_hot_key)
        {
            Interface::CycleLabelType();
        }
    }
    return cont;
}
