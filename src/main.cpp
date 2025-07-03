import config;
import inputhandler;
import Pickpocket;
import heat;
#include "formloader.h"
#include "papyrus.h"

void InitListener(SKSE::MessagingInterface::Message* a_msg) {

	switch (a_msg->type) {
	case SKSE::MessagingInterface::kInputLoaded:
		InputManager::GetSingleton()->Register();
		InputManager::GetSingleton()->SetEditorKey();
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		FormLoader::Loader::GetSingleton()->LoadForms();
		PickpocketTimer::InitPickpocketCapChange();
		ReputationPerkHandler::GetSingleton()->RegisterCellEvent();
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
	SKSE::GetPapyrusInterface()->Register(Papyrus::Register);
	SKSE::GetMessagingInterface()->RegisterListener(InitListener);
	return true;
}
