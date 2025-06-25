module Pickpocket;
import utility;
import config;
import heat;

static UtilTimer timer{}; 
constexpr float kUninitializedChance = 224.363f;
static float pickpocket_chance_default{ kUninitializedChance };

static float actual_cap;

float PickpocketTimer::ActorValueLevelTimeModifier(RE::PlayerCharacter* player) {

	const auto& settings = Config::Settings::GetSingleton();
	float min_time = settings->pickpocket_min_time.GetValue();
	float max_time = settings->pickpocket_max_time.GetValue();
	float timer_time = Utility::CalculateTimerForSkill(player, RE::ActorValue::kPickpocket, min_time, max_time);
	return timer_time;
}

float PickpocketTimer::GetPickpocketChanceFromFile()
{
	if (pickpocket_chance_default == kUninitializedChance) {
		pickpocket_chance_default = Utility::get_gmst("fPickPocketMaxChance")->GetFloat();
	}
	return pickpocket_chance_default;	
}

void PickpocketTimer::SetPickpocketChance(float new_cap)
{
	Utility::set_gmstFloat("fPickPocketMaxChance", new_cap);
	actual_cap = new_cap;
}

void PickpocketTimer::ResetPickpocketChance()
{
	Utility::set_gmstFloat("fPickPocketMaxChance", pickpocket_chance_default);
	actual_cap = pickpocket_chance_default;
}

void PickpocketTimer::AdjustPickpocketCapWithHeat()
{
	float heat_lvl = HeatSystem::GetHeatValue();
	float new_cap = std::lerp(75.0f, 50.0f, heat_lvl / 100.0f);
	SetPickpocketChance(new_cap);
}

void PickpocketTimer::ResetStoredPickpocketChance()
{
	Utility::set_gmstFloat("fPickPocketMaxChance", pickpocket_chance_default);
	actual_cap = pickpocket_chance_default;
}

uint32_t PickpocketTimer::CalculatePickPocketChancePlayer(float a_thiefSkill, float a_targetSkill, std::uint32_t a_valueStolen, float a_weightStolen, RE::Actor* a_thief, RE::Actor* a_target, bool a_isDetected, RE::TESForm* a_itemPickpocketing)
{
	auto modifier = 1.0f;
	auto result = _Hook13(a_thiefSkill, a_targetSkill, a_valueStolen, a_weightStolen, a_thief, a_target, a_isDetected, a_itemPickpocketing);

	if (Config::Settings::GetSingleton()->enable_dyn_pickpocket_cap.GetValue()) {
		float infamyLevel = HeatSystem::GetHeatValue();
		modifier = std::lerp(0.85f, 1.15f, (100.0f - infamyLevel) / 100.0f);
		result = std::clamp(result * modifier, 0.0f, actual_cap);
	}
	REX::INFO("pickpocket chance for player attempt pickpocket is: {}", result);
	return result;
}

uint32_t PickpocketTimer::CalculatePickPocketChance(float a_thiefSkill, float a_targetSkill, std::uint32_t a_valueStolen, float a_weightStolen, RE::Actor* a_thief, RE::Actor* a_target, bool a_isDetected, RE::TESForm* a_itemPickpocketing)
{
	auto modifier = 1.0f;

	auto result = _Hook14(a_thiefSkill, a_targetSkill, a_valueStolen, a_weightStolen, a_thief, a_target, a_isDetected, a_itemPickpocketing);

	if (Config::Settings::GetSingleton()->enable_dyn_pickpocket_cap.GetValue()) {
		float infamyLevel = HeatSystem::GetHeatValue();
		modifier = std::lerp(0.85f, 1.15f, (100.0f - infamyLevel) / 100.0f);
	}
	result = std::clamp(result * modifier, 0.0f, actual_cap);

	return result;
}

void PickpocketTimer::StartPickPocketTimer() {
	float time = ActorValueLevelTimeModifier(RE::PlayerCharacter::GetSingleton());
	timer.Start(time);
	REX::DEBUG("Started Timer for {}, it runs {} seconds", typeid(PickpocketTimer).name(), time);
}

float PickpocketTimer::GetRemainingPickpocketTime()
{
	return std::max(0.0f, timer.GetExpectedRuntime() - timer.Elapsed());
}

bool PickpocketTimer::GetIsPTimerRunning()
{
	return timer.IsRunning();
}

bool PickpocketTimer::_PTimerEnabled = true;

bool PickpocketTimer::IsPTimerEnabled()
{
	return _PTimerEnabled;
}

void PickpocketTimer::InitPickpocketCapChange()
{
	if (pickpocket_chance_default == kUninitializedChance) {
		pickpocket_chance_default = Utility::get_gmst("fPickPocketMaxChance")->GetFloat();
	}
	REX::INFO("Original Pickpocket cap detected: {}", pickpocket_chance_default);
	SetPickpocketChance(75.0f);
}

RE::UI_MESSAGE_RESULTS PickpocketTimer::ProcessMenu(RE::ContainerMenu* a_this, RE::UIMessage& a_message) {

	if (a_this->GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket) {
		a_this->menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);

		if (Config::Settings::GetSingleton()->enable_dyn_pickpocket_cap.GetValue()) {
			AdjustPickpocketCapWithHeat();
		}

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
			Utility::HideMenu(a_this->MENU_NAME);
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