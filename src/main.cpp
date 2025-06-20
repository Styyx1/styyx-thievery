import config;
#include "formloader.h"

void InitListener(SKSE::MessagingInterface::Message* a_msg) {

	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		FormLoader::Loader::GetSingleton()->LoadForms();
		break;

	case SKSE::MessagingInterface::kPostLoadGame:

		break;

	case SKSE::MessagingInterface::kNewGame:

		break;

	}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, {.trampoline = true});
	Config::Settings::GetSingleton()->Update();
	SKSE::GetMessagingInterface()->RegisterListener(InitListener);
	return true;
}
