import config;
import Pickpocket;
import heat;
#include "infamyHUD.h"
#include "formloader.h"
#include "papyrus.h"

void InitListener(SKSE::MessagingInterface::Message* a_msg) {

	switch (a_msg->type) {
	case SKSE::MessagingInterface::kInputLoaded:

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
	InfamyHUD::InfamyBar::RegisterInfamyBar();
	Menu::RegisterThiefMenu();
	SKSE::GetPapyrusInterface()->Register(Papyrus::Register);
	SKSE::GetMessagingInterface()->RegisterListener(InitListener);
	return true;
}
