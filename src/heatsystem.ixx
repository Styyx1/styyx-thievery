export module heat;

namespace HeatValues {
    export uint32_t item_value_fenced;
}

export class HeatSystem
{
public:
    static float GetHeatValue();
    static void SetHeatValue(float a_value);
    static void IncreaseHeat(float a_value);
    static void DecreaseHeat(float a_value);
    static void ClearHeat();
    static bool IsHeatAbove(float a_threshold);
    static void UpdateHeatValueExternal();

private:
    static void PlayerUpdateLoop(RE::PlayerCharacter* a_player, float a_delta); //used to check if you're around a crime scene for x amount of time and for decreasing heat over time
    static void OnGoToPrison(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_removeInventory, bool a_realJail);
    static void PayCrime(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_goToJail, bool a_removeStolenItems);
    static void DoCalculateDetection(RE::Actor* a_this, RE::Actor* target, std::int32_t& score, bool& spotted, bool& hasLOS, std::int32_t& reason, RE::NiPoint3& lastPos,  std::int32_t& soundLvl, float& unk8, float& unk9);

    static inline REL::HookVFT _Hook7{ RE::VTABLE_PlayerCharacter[0], 0xBB, PayCrime };
    static inline REL::HookVFT _Hook8{ RE::VTABLE_PlayerCharacter[0], 0x0b9, OnGoToPrison };
    static inline REL::HookVFT _Hook9{  RE::VTABLE_PlayerCharacter[0], 0xAD, PlayerUpdateLoop };
    static inline REL::Hook _Hook10{ REL::ID(42742), 0x67b, DoCalculateDetection };

    static void UpdatePassiveHeatDecay();
    static std::int32_t ReCalculateDetectionLevel(std::int32_t a_level);
};

export void ToggleHUD();
export void EnterEditMode();



