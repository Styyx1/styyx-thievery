import config;
import Pickpocket;
import heat;
#include "infamyHUD.h"
#include "formloader.h"
#include "papyrus.h"
#include "API/styyx-thievery-api.h"

extern "C" __declspec(dllexport) Thievery* RequestThieveryAPI()
{
	static Thievery g_thieveryAPI;

	g_thieveryAPI.version = THIEVERY_API_VERSION;
	g_thieveryAPI.IsWidgetVisible = []() {return InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->GetIsVisible(); };
	g_thieveryAPI.SetWidgetVisible = [](bool a_visible) {InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->SetIsVisible(a_visible); };

	return &g_thieveryAPI;
}


void InitListener(SKSE::MessagingInterface::Message* a_msg) {

	switch (a_msg->type) {
	case SKSE::MessagingInterface::kInputLoaded:

		break;
	case SKSE::MessagingInterface::kDataLoaded:
		InfamyHUD::InfamyBar::InfamyBarData::GetSingleton();
		FormLoader::Loader::GetSingleton()->LoadForms();
		PickpocketTimer::InitPickpocketCapChange();
		ReputationPerkHandler::GetSingleton()->RegisterCellEvent();
		break;

	default:
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
