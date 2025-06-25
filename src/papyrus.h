#pragma once

#define SCRIPT_ARGS RE::BSScript::Internal::VirtualMachine*, RE::VMStackID, RE::StaticFunctionTag*

namespace Papyrus {
	bool Register(RE::BSScript::Internal::VirtualMachine* a_vm);

    enum class API_VERSION : int32_t
    {
        kVersion1 = 1,
    };

    namespace Functions {

        static constexpr API_VERSION CurrentVersion = API_VERSION::kVersion1;
        static constexpr std::string_view script_name = "styyx_thieveryFunctions";
        //Version 1
        int GetVersion(SCRIPT_ARGS);
        float GetRemainingLockpickTime(SCRIPT_ARGS);
        float GetRemainingPickpocketTime(SCRIPT_ARGS);
        int GetThiefReputation(SCRIPT_ARGS);
        void DecreaseThiefReputationPercentage(SCRIPT_ARGS, float a_pctModifier);
        void IncreaseThiefReputation(SCRIPT_ARGS, int a_amount);
        bool GetIsPickpocketTimerRunning(SCRIPT_ARGS);
        bool GetIsLockpickTimerRunning(SCRIPT_ARGS);

        //Register Function
        void Bind(RE::BSScript::Internal::VirtualMachine& a_vm);
    };
}
