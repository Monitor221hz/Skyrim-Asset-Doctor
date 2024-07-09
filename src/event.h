#pragma once

#include "settings.h"
#include "validator.h"

namespace AssetDoctor
{
     class ObjectLoadEventHandler : public RE::BSTEventSink<RE::TESObjectLoadedEvent> 
    {
        public: 
        static ObjectLoadEventHandler* GetSingleton()
        {
            static ObjectLoadEventHandler singleton; 
            return &singleton; 
        }

        void Register()
        {
            RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(this); 
        }

        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent *a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent> *a_eventSource) override;
    };
    class ObjectAttachEventHandler : public RE::BSTEventSink<RE::TESCellAttachDetachEvent>
    {
        public:
        static ObjectAttachEventHandler* GetSingleton()
        {
            static ObjectAttachEventHandler singleton; 
            return &singleton; 
        }
        void Register()
        {
            RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(this); 
        }
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESCellAttachDetachEvent *a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>* a_eventSource) override; 
    };
    class CellLoadedEventHandler : public RE::BSTEventSink<TESCellFullyLoadedEvent>
    {
        public: 
        static CellLoadedEventHandler* GetSingleton()
        {
            static CellLoadedEventHandler singleton; 
            return &singleton; 
        }
        void Register()
        {
            RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(this); 
        }
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent *a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) override; 
    };
    class HotKeyHandler : public RE::BSTEventSink<InputEvent *>
    {
    public:
        static HotKeyHandler *GetSingleton()
        {
            static HotKeyHandler singleton;
            return &singleton;
        }

        void Register()
        {
            BSInputDeviceManager::GetSingleton()->AddEventSink(this);
            label_hot_key = Settings::GetToggleLabelCode(); 
            gui_hot_key = Settings::GetToggleGUICode();
            cycle_label_hot_key = Settings::GetCycleLabelKeyCode();
            cycle_label_type_hot_key = Settings::GetCycleLabelTypeKeyCode();
        }

        void AssignLabelHotKey(uint32_t key_code) { cycle_label_hot_key = key_code; }
        void AssignMenuHotKey(uint32_t key_code) { gui_hot_key = key_code;  }
        void AssignCycleHotKey(uint32_t key_code) { cycle_label_type_hot_key = key_code; }
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent *const *a_evn, RE::BSTEventSource<RE::InputEvent *> *) override;

    private:
        static inline uint32_t cycle_label_type_hot_key = 64;
        static inline uint32_t cycle_label_hot_key = 65;
        static inline uint32_t label_hot_key = 62; 
        static inline uint32_t gui_hot_key = 66;

    };
}