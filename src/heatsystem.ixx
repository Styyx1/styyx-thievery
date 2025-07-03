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
    static inline std::unordered_set<RE::Actor*> actor_pickpocketed;

private:
    static void PlayerUpdateLoop(RE::PlayerCharacter* a_player, float a_delta); //used to check if you're around a crime scene for x amount of time and for decreasing heat over time
    static void OnGoToPrison(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_removeInventory, bool a_realJail);
    static void PayCrime(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_goToJail, bool a_removeStolenItems);
    static void DoCalculateDetection(RE::Actor* a_this, RE::Actor* target, std::int32_t& score, bool& spotted, bool& hasLOS, std::int32_t& reason, RE::NiPoint3& lastPos,  std::int32_t& soundLvl, float& unk8, float& unk9);

    static inline REL::HookVFT _Hook7{ RE::VTABLE_PlayerCharacter[0], 0xBB, PayCrime };
    static inline REL::HookVFT _Hook8{ RE::VTABLE_PlayerCharacter[0], 0x0b9, OnGoToPrison };
    static inline REL::HookVFT _Hook9{  RE::VTABLE_PlayerCharacter[0], 0xAD, PlayerUpdateLoop };
    static inline REL::Hook _Hook10{ REL::ID(42742), 0x67b, DoCalculateDetection }; //  SE ID: 41659

    static void UpdatePassiveHeatDecay();
    static std::int32_t ReCalculateDetectionLevel(std::int32_t a_level);
    static std::pair<RE::TESBoundObject*, std::int32_t> GetRandomInventoryItem(const RE::TESObjectREFR::InventoryItemMap& inventory);
    static float GetRandomPickpocketChance(float pickpocketSkill, float infamy, float reputation);
    static void AddPickpocketExperience();
    
    
};

export struct ReputationPerkHandler final : public REX::Singleton<ReputationPerkHandler>,
public RE::BSTEventSink<RE::BGSActorCellEvent>
{
    void RegisterCellEvent();
    RE::BSEventNotifyControl ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*) override;
    bool ReferenceIsInventoryItem(RE::TESForm* a_ref);
    bool CellIsDangerousLocation(RE::TESObjectCELL* a_cell, const std::unordered_set<std::string_view>& keywords);
    bool AreValuablesInCell(RE::TESObjectCELL* a_cell);
	std::vector<RE::TESObjectREFR*> GetValuablesInCell(RE::TESObjectCELL* a_cell);


    const std::unordered_set<std::string_view> danger_loc{
        "LocTypeAnimalDen",
        "LocTypeBanditCamp",
        "LocTypeCemetery",
        "LocTypeDragonLair",
        "LocTypeDragonPriestLair",
        "LocTypeDraugrCrypt",
        "LocTypeDungeon",
        "LocTypeDwarvenAutomatons",
        "LocTypeFalmerHive",
        "LocTypeForswornCamp",
        "LocTypeGiantCamp",
        "LocTypeHagravenNest",
        "LocTypeMine",
        "LocTypeSprigganGrove",
        "LocTypeVampireLair",
        "LocTypeWarlockLair",
        "LocTypeWerewolfLair"
    };

};

