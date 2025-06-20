module Pickpocket;

import utility;
import config;

static Timer timer{}; 

float PickpocketTimer::ActorValueLevelTimeModifier(RE::PlayerCharacter* player) {
	float pickpocket_level = player->GetActorValue(RE::ActorValue::kPickpocket);
	const auto& settings = Config::Settings::GetSingleton();
	float pickpocket_time = std::clamp(settings->pickpocket_min_time.GetValue() * (1.5f + pickpocket_level / 50.0f), settings->pickpocket_min_time.GetValue(), settings->pickpocket_max_time.GetValue());
	return pickpocket_time;
}

void PickpocketTimer::StartPickPocketTimer() {
	float time = ActorValueLevelTimeModifier(RE::PlayerCharacter::GetSingleton());
	timer.Start(time);
	REX::DEBUG("Started Timer for {}, it runs {} seconds", typeid(PickpocketTimer).name(), time);
}

RE::UI_MESSAGE_RESULTS PickpocketTimer::ProcessMenu(RE::ContainerMenu* a_this, RE::UIMessage& a_message) {

	if (a_this->GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket) {
		a_this->menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
		if (a_message.type == RE::UI_MESSAGE_TYPE::kShow) {
			StartPickPocketTimer();
		}
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide) {
			timer.Stop();
		}

		if (timer.IsRunning() && timer.Elapsed() >= timer.GetExpectedRuntime()) {
			const auto npcrefhandle = a_this->GetTargetRefHandle();
			auto player = RE::PlayerCharacter::GetSingleton();	
			RE::ActorPtr actor_ref;
			RE::TESFaction* faction = nullptr;
			RE::LookupReferenceByHandle(npcrefhandle, actor_ref);
			timer.Stop();
			HideMenu(a_this->MENU_NAME);
			REX::DEBUG("closing menu");
			if (actor_ref) {
				auto actor = actor_ref.get();
				faction = actor->GetActorBase()->crimeFaction;
				auto value = Crimes::GetCrimeValue(player, RE::PackageNS::CRIME_TYPE::kPickpocket, faction, 50);
				REX::DEBUG("crime value is: {}", value);
				player->ModCrimeGoldValue(faction, false, value);
				Crimes::unk_Assault(actor->currentProcess, actor, player, 0, 0);
			}			
		}
	}
	else {
		a_this->menuFlags.reset(RE::UI_MENU_FLAGS::kRequiresUpdate);
	}
	return _Hook0(a_this, a_message);
}