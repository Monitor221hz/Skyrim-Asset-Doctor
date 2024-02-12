#pragma once

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
}