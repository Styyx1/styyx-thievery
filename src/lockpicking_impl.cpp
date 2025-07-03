module lockpicking;
#include "formloader.h"

#undef min
#undef max

import utility;
import config;
import heat;

static UtilTimer lockpick_timer{};

bool LockpickingMenu::_timerEnabled = true;

float LockpickingMenu::GetRemainingLockpickTimer()
{
	return std::max(0.0f, lockpick_timer.GetExpectedRuntime() - lockpick_timer.Elapsed());
}

float LockpickingMenu::GetCalcTimerForLockpicking()
{
	RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
	Config::Settings* const& settings = Config::Settings::GetSingleton();
	float min_time = settings->lockpicking_min_time.GetValue();
	float max_time = settings->lockpicking_max_time.GetValue();
	float timer_time = Utility::CalculateTimerForSkill(player, RE::ActorValue::kLockpicking, min_time, max_time);
	return timer_time;
}

bool LockpickingMenu::GetIsTimerRunning()
{
	return lockpick_timer.IsRunning();
}

bool LockpickingMenu::IsTimerEnabled()
{
	return _timerEnabled;
}

static bool angle_adjusted = false;

RE::UI_MESSAGE_RESULTS LockpickingMenu::ProcessMessage(RE::LockpickingMenu* a_this, RE::UIMessage& a_message)
{
	static const auto& config = Config::Settings::GetSingleton();
	if (IsTimerEnabled()) {
		if (a_message.type == RE::UI_MESSAGE_TYPE::kShow) {
			StartLockpickTimer();
			angle_adjusted = false;
		}
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide) {
			StopLockpickTimer();
		}
		if (lockpick_timer.IsRunning() && lockpick_timer.Elapsed() >= lockpick_timer.GetExpectedRuntime()) {
			StopLockpickTimer();
			Utility::HideMenu(a_this->MENU_NAME);
			auto isCrimeToAct = a_this->GetTargetReference()->IsCrimeToActivate();
			LockpickConsequeces(RE::PlayerCharacter::GetSingleton(), isCrimeToAct); 	
		}
	}
	if (config->enable_dynamic_lockpicking.GetValue() && !angle_adjusted && a_this->sweetSpotAngle > 0) {
		float infamyLevel = HeatSystem::GetHeatValue();
		a_this->sweetSpotAngle *= std::lerp(0.85f, 1.15f, ( infamyLevel) / 100.0f);
		angle_adjusted = true;
	}
	return _Hook5(a_this, a_message);
}

void LockpickingMenu::StartLockpickTimer()
{
	if (!lockpick_timer.IsRunning()) {
		float time = GetCalcTimerForLockpicking();
		lockpick_timer.Start(time);
		REX::INFO("started lockpick timer for {} seconds", time);
	}
}

void LockpickingMenu::StopLockpickTimer()
{
	lockpick_timer.Stop();
	REX::INFO("stopped lockpick timer");
}

void LockpickingMenu::LockpickConsequeces(RE::Actor *a_player, bool isCrime)
{
	static const auto& formLoader = FormLoader::Loader::GetSingleton();
	static const auto& config = Config::Settings::GetSingleton();
	if(config->remove_lockpick_on_timeout.GetValue())
		a_player->RemoveItem(formLoader->lockpick_item, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
	bool unk1 = false;
	RE::Actor* crime_reporter = Utility::GetClosestNonTeammate(a_player, 1024.0);
	if (crime_reporter && isCrime) {
		REX::INFO("crime reporter is: {}", crime_reporter->GetDisplayFullName());
		if (crime_reporter->HasLineOfSight(a_player, unk1)) {
			auto faction = crime_reporter->GetActorBase()->crimeFaction;
			if (faction) {
				auto value = Crimes::GetCrimeValue(a_player, RE::PackageNS::CRIME_TYPE::kTrespass, faction);
				REX::INFO("crime value is: {}", value);
				a_player->ModCrimeGoldValue(faction, false, value);
				Crimes::SendTrespassingAlarm(a_player, crime_reporter, crime_reporter->GetActorBase(), RE::PackageNS::CRIME_TYPE::kTrespass);
			}			
		}		
	}
}
