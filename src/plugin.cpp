#include "log.h"

#include "hook.h"
#include "event.h"
void OnDataLoaded()
{
   
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		AssetDoctor::Settings::Load();
		AssetDoctor::Interface::Load();
        // AssetDoctor::ObjectLoadEventHandler::GetSingleton()->Register();
		AssetDoctor::HotKeyHandler::GetSingleton()->Register();
		// AssetDoctor::ObjectAttachEventHandler::GetSingleton()->Register(); 
		// AssetDoctor::CellLoadedEventHandler::GetSingleton()->Register(); 
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		AssetDoctor::Interface::QueueReset();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
        break;
	case SKSE::MessagingInterface::kNewGame:
		AssetDoctor::Interface::QueueReset();
		break;
	}
}
//2f31b6ae9c100cbff2a6d82e8673ac99ce8a4918
SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();


    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}
	AssetDoctor::Validator::Setup(); 
	ImGui::Renderer::Install();
	AssetDoctor::Load3DHook::Install(); 
	AssetDoctor::FinishSetupGeometryHook::Install(); 
	AssetDoctor::SetupGeometryHook::Install(); 
    return true;
}