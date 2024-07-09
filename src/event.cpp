#include "event.h"
#include "util.h"
using namespace RE;

RE::BSEventNotifyControl AssetDoctor::ObjectAttachEventHandler::ProcessEvent(const RE::TESCellAttachDetachEvent *a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>* a_eventSource)
{
    auto cont = RE::BSEventNotifyControl::kContinue;
    if (!a_event->attached) { return cont; }

    Validator::ValidateReference(a_event->reference.get()); 

    return cont;    
}

RE::BSEventNotifyControl AssetDoctor::ObjectLoadEventHandler::ProcessEvent(const RE::TESObjectLoadedEvent *a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent> *a_eventSource)
{
    auto cont = RE::BSEventNotifyControl::kContinue;

    if (!a_event->loaded) { return cont; }

    auto* form = TESForm::LookupByID(a_event->formID);

    auto* ref = form->As<TESObjectREFR>();

    Validator::ValidateReference(ref); 

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

RE::BSEventNotifyControl AssetDoctor::CellLoadedEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent *a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent> *a_eventSource)
{
    auto cont = RE::BSEventNotifyControl::kContinue;
    auto* cell = a_event->cell; 
    if (!cell) { return cont; }
    auto cell_rtd = cell->GetRuntimeData(); 

    RE::BSSpinLockGuard locker(cell_rtd.spinLock);
    std::unordered_set<FormID> baseFormIds; 
    for (const auto &ref : cell_rtd.references)
    {
        auto* baseObj = ref->GetBaseObject(); 
        if (!baseObj || baseFormIds.contains(baseObj->GetFormID())) { continue; }
        Validator::ValidateReference(ref.get());
        baseFormIds.emplace(baseObj->GetFormID());
    }
    return cont; 
}
