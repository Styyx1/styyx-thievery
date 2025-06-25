#include "papyrus.h"
import Pickpocket;
import lockpicking;
import config;
import generalStealing;

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

	void Functions::Bind(RE::BSScript::Internal::VirtualMachine& a_vm)
	{
		if (CurrentVersion >= API_VERSION::kVersion1) {
			a_vm.RegisterFunction("GetVersion", script_name, GetVersion, true);
			a_vm.RegisterFunction("GetRemainingLockpickTime", script_name, GetRemainingLockpickTime);
			a_vm.RegisterFunction("GetRemainingPickpocketTime", script_name, GetRemainingPickpocketTime);
			a_vm.RegisterFunction("GetThiefReputation", script_name, GetThiefReputation);
			a_vm.RegisterFunction("DecreaseThiefReputationBy20Percent", script_name, DecreaseThiefReputationPercentage);
			a_vm.RegisterFunction("IncreaseThiefReputation", script_name, IncreaseThiefReputation);
			a_vm.RegisterFunction("GetIsPickpocketTimerRunning", script_name, GetIsPickpocketTimerRunning);
			a_vm.RegisterFunction("GetIsLockpickTimerRunning", script_name, GetIsLockpickTimerRunning);
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