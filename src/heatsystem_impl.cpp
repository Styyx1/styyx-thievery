module heat;
#include "formloader.h"
import config;
import utility;

constexpr float MIN_HEAT_INCREASE = 2.0f;
constexpr float MAX_HEAT_INCREASE = 10.0f;
constexpr float HEAT_MAX_ITEM_VALUE = 1200.0f;
constexpr float MAX_HEAT_VALUE = 100.0f;
constexpr int MIN_ATTACK_CHANCE = 1;
constexpr int MAX_ATTACK_CHANCE = 50;

float HeatSystem::GetHeatValue()
{
	return FormLoader::Loader::GetSingleton()->player_heat_system_global->value;
}

void HeatSystem::SetHeatValue(float a_value)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	formL->player_heat_system_global->value = a_value;
}

void HeatSystem::IncreaseHeat(float a_value)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	REX::INFO("Heat global before increasing is: {}", GetHeatValue());
	formL->player_heat_system_global->value = std::clamp(formL->player_heat_system_global->value + a_value, 0.0f, MAX_HEAT_VALUE);
	REX::INFO("Heat global is: {}", GetHeatValue());
}

void HeatSystem::DecreaseHeat(float a_value)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	REX::INFO("Heat global before decreasing is: {}", GetHeatValue());
	formL->player_heat_system_global->value = std::clamp(formL->player_heat_system_global->value - a_value, 0.0f, MAX_HEAT_VALUE);
	REX::INFO("Heat global is: {}", GetHeatValue());	
}

void HeatSystem::ClearHeat()
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	formL->player_heat_system_global->value = 0;
}

bool HeatSystem::IsHeatAbove(float a_threshold)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	return formL->player_heat_system_global->value > a_threshold;
}

void HeatSystem::UpdateHeatValueExternal()
{
	float min_value = Config::Settings::GetSingleton()->fence_value_heat_threshold.GetValue();

	if (min_value >= HEAT_MAX_ITEM_VALUE) 
		min_value = HEAT_MAX_ITEM_VALUE - 1.0f;
	
	float value = HeatValues::item_value_fenced;
	if (value >= min_value) {		
		float heat_increase = MIN_HEAT_INCREASE;

		float scale = (MAX_HEAT_INCREASE - MIN_HEAT_INCREASE) / (HEAT_MAX_ITEM_VALUE - min_value);
		heat_increase += (value - min_value) * scale;

		if (heat_increase > MAX_HEAT_INCREASE)
			heat_increase = MAX_HEAT_INCREASE;

		HeatSystem::IncreaseHeat(heat_increase);
	}
}

//timer instance of heat system 

static UtilTimer g_heat_decrease_timer;
static UtilTimer g_wait_in_crime_timer;
static UtilTimer g_heat_increase_timer;
static bool timer_was_run = false;

void AllCrimeTimersStop() {
	g_heat_increase_timer.Stop();
	g_wait_in_crime_timer.Stop();
	timer_was_run = false;
}

bool IsAnyCrimeTimerRunning() {
	return g_heat_increase_timer.IsRunning() || g_wait_in_crime_timer.IsRunning();
}

void StartIngameHourTimer() {
	if (!g_heat_decrease_timer.IsRunning()) {
		RE::Calendar* cal = RE::Calendar::GetSingleton();		
		g_heat_decrease_timer.Start(1800 / cal->GetTimescale());
		REX::INFO("started decay timer for {} seconds", g_heat_decrease_timer.GetExpectedRuntime());
	}
}

void HeatSystem::UpdatePassiveHeatDecay() {
	if (!IsAnyCrimeTimerRunning()) {
		if (!g_heat_decrease_timer.IsRunning()) {
			StartIngameHourTimer();
		} else if (g_heat_decrease_timer.Elapsed() >= g_heat_decrease_timer.GetExpectedRuntime()) {
			g_heat_decrease_timer.Reset();
			DecreaseHeat(5.0f);
			REX::INFO("Decrease heat over time");
		}
	} else {
		g_heat_decrease_timer.Stop();
	}
}

std::int32_t HeatSystem::ReCalculateDetectionLevel(std::int32_t a_level)
{
	if (a_level < 0) {
		a_level += 100;
		return a_level = std::clamp(a_level, 0, 100);
	} else
		return 100;
}


void HeatSystem::PlayerUpdateLoop(RE::PlayerCharacter* a_player, float a_delta)
{
	if (a_player->GetParentCell() != UtilStates::curr_crime_cell && UtilStates::IsInCrimeScene() || !a_player->GetParentCell()) {
		UtilStates::SetInCrimeScene(false);
		AllCrimeTimersStop();
		REX::INFO("left crime scene. stop all waiting timers");
	}
	if (UtilStates::IsInCrimeScene() && !timer_was_run) {
		if (!g_wait_in_crime_timer.IsRunning()) {
			g_wait_in_crime_timer.Start(120);
			REX::INFO("crime scene timer is running");
			timer_was_run = true;
		}			
	}
	if (g_wait_in_crime_timer.IsRunning() && g_wait_in_crime_timer.Elapsed() >= g_wait_in_crime_timer.GetExpectedRuntime()) {
		g_wait_in_crime_timer.Stop();
		REX::INFO("wait time's over, start to gain infamy");
		if (!g_heat_increase_timer.IsRunning()) {
			g_heat_increase_timer.Start(30);
		}
		IncreaseHeat(5.0f);
	}
	if (g_heat_increase_timer.IsRunning() && g_heat_increase_timer.Elapsed() >= g_heat_increase_timer.GetExpectedRuntime()) {
		if (UtilStates::IsInCrimeScene()) {
			g_heat_increase_timer.Reset();
			IncreaseHeat(4.0f);
		}
		else
			g_heat_increase_timer.Stop();
	}
	UpdatePassiveHeatDecay();
	return _Hook9(a_player, a_delta);
}

void HeatSystem::OnGoToPrison(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_removeInventory, bool a_realJail)
{
	if (a_realJail) {
		REX::INFO("went to jail");
		ClearHeat();
	}
	else {
		DecreaseHeat(3.0f);
	}
	_Hook8(a_this, a_faction, a_removeInventory, a_realJail);
}

void HeatSystem::PayCrime(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_goToJail, bool a_removeStolenItems)
{
	auto fine_now = a_this->GetCrimeGoldValue(a_faction);
	_Hook7(a_this, a_faction, a_goToJail, a_removeStolenItems);
	auto& total_crimes = a_this->crimeGoldMap;

	if (total_crimes.empty()) {
		REX::INFO("no more crimes, clear heat");
		ClearHeat();
	}
	if (fine_now >= a_faction->crimeData.crimevalues.murderCrimeGold) {
		// Don't reduce heat for murder bounties — your reputation remains infamous
		return;
	}
	REX::INFO("fine now is {}", fine_now);
	if (fine_now <= 100) {
		DecreaseHeat(2.0f);
	} else if (fine_now <= 150) {
		DecreaseHeat(4.0f);
	} else if (fine_now <= 300) {
		DecreaseHeat(3.0f);
	} else {
		DecreaseHeat(1.0f);
	}
}

void HeatSystem::DoCalculateDetection(RE::Actor* a_this, RE::Actor* target, std::int32_t& score, bool& spotted, bool& hasLOS, std::int32_t& reason, RE::NiPoint3& lastPos,  std::int32_t& soundLvl, float& unk8, float& unk9)
{
	_Hook10(a_this, target, score, spotted, hasLOS, reason, lastPos, soundLvl, unk8, unk9);	
	if (Utility::IsGuardNearby(target, 512) && target->IsSneaking()) {
		RE::DebugNotification("Your presence is noticed...");
		if (spotted) {
			auto infamy = GetHeatValue();
			if (infamy >= 90.0f && a_this->IsGuard()) {
				int random_int = Randomiser::GetRandomInt(MIN_ATTACK_CHANCE, MAX_ATTACK_CHANCE);
				if (random_int == 1) {
					Crimes::unk_Assault(a_this->currentProcess, a_this, target);
				}			
			}
		}
		return;
	}	
}
