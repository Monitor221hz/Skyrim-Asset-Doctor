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
        AssetDoctor::ObjectLoadEventHandler::GetSingleton()->Register();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		AssetDoctor::Interface::QueueReset();
        break;
	case SKSE::MessagingInterface::kNewGame:
		AssetDoctor::Interface::QueueReset();
		break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();


    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}
	ImGui::Renderer::Install();
	
	
    return true;
}