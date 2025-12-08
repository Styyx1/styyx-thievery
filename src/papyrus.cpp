#include "papyrus.h"
#include "infamyHUD.h"
import Pickpocket;
import lockpicking;
import config;
import generalStealing;
import heat;

namespace Papyrus {
	int Functions::GetVersion(SCRIPT_ARGS)
	{
		return static_cast<int>(CurrentVersion);
	}
	float Functions::GetRemainingLockpickTime(SCRIPT_ARGS)
	{
		return LockpickingMenu::GetRemainingLockpickTimer();
	}
	float Functions::GetRemainingPickpocketTime(SCRIPT_ARGS)
	{
		return PickpocketTimer::GetRemainingPickpocketTime();
	}
	int Functions::GetThiefReputation(SCRIPT_ARGS)
	{
		return Stealing::NightThief::GetNightReputation();
	}
	void Functions::DecreaseThiefReputationPercentage(SCRIPT_ARGS, float a_pctModifier)
	{		
		Stealing::NightThief::DecreaseReputation(a_pctModifier);
	}
	void Functions::IncreaseThiefReputation(SCRIPT_ARGS, int a_amount)
	{
		Stealing::NightThief::IncreaseNightReputation(static_cast<std::uint16_t>(a_amount));
	}
	bool Functions::GetIsPickpocketTimerRunning(SCRIPT_ARGS)
	{
		return PickpocketTimer::GetIsPTimerRunning();
	}
	bool Functions::GetIsLockpickTimerRunning(SCRIPT_ARGS)
	{
		return LockpickingMenu::GetIsTimerRunning();
	}

	bool Functions::IsSomethingOfValueHere(SCRIPT_ARGS)
	{
		RE::PlayerCharacter* const& player = RE::PlayerCharacter::GetSingleton();
		auto cell = player->GetParentCell();
		if (cell) {
			return cell->IsInteriorCell() ? ReputationPerkHandler::GetSingleton()->AreValuablesInCell(cell) : false;
		}
		return false;
		
	}

	void Functions::DecreaseAllBounties(SCRIPT_ARGS, float a_amount)
	{
		if (a_amount <= 0.0f) {
			REX::ERROR("DecreaseAllBounties called with non-positive amount: {}", a_amount);
			return;
		}
		const auto& player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			REX::ERROR("Player character not found.");
			return;
		}
		Stealing::CrimeTracker::LowerAllBounties(a_amount);

		REX::DEBUG("Decreased all bounties by {}", a_amount);
	}

	void Functions::ToggleWidget(SCRIPT_ARGS, bool enable)
	{
		InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->SetIsVisible(enable);
	}

	void Functions::Bind(RE::BSScript::Internal::VirtualMachine& a_vm)
	{
		if constexpr (CurrentVersion >= API_VERSION::kVersion1) {
			a_vm.RegisterFunction("GetVersion", script_name, GetVersion, true);
			a_vm.RegisterFunction("GetRemainingLockpickTime", script_name, GetRemainingLockpickTime);
			a_vm.RegisterFunction("GetRemainingPickpocketTime", script_name, GetRemainingPickpocketTime);
			a_vm.RegisterFunction("GetThiefReputation", script_name, GetThiefReputation);
			a_vm.RegisterFunction("DecreaseThiefReputationPercentage", script_name, DecreaseThiefReputationPercentage);
			a_vm.RegisterFunction("IncreaseThiefReputation", script_name, IncreaseThiefReputation);
			a_vm.RegisterFunction("GetIsPickpocketTimerRunning", script_name, GetIsPickpocketTimerRunning);
			a_vm.RegisterFunction("GetIsLockpickTimerRunning", script_name, GetIsLockpickTimerRunning);
			a_vm.RegisterFunction("IsSomethingOfValueHere", script_name, IsSomethingOfValueHere);
			a_vm.RegisterFunction("DecreaseAllBounties", script_name, DecreaseAllBounties);
		}
		if constexpr (CurrentVersion >= API_VERSION::kVersion2) {
			a_vm.RegisterFunction("ToggleInfamyWidget", script_name, ToggleWidget);
		}

		REX::INFO("Registered Papyrus functions for {}", script_name);
	}
	bool Register(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		if (!a_vm) {
			REX::CRITICAL("couldn't get VM State");
			return false;
		}
		REX::INFO("{:*^30}", "FUNCTIONS");
		Functions::Bind(*a_vm);
		return true;
	}
}