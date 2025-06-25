export module generalStealing;

export namespace Stealing {
	struct NightThief {

		static uint32_t GetNightReputation();
		static void DecreaseReputation(float a_pctModifier);
		static void IncreaseNightReputation(uint16_t amount);
		static uint16_t CalculateReputationGain(uint32_t itemValue);

	private:
		static void PickupObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound);
		static void OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr);
		static RE::ExtraDataList* GetExtraData(RE::InventoryEntryData* a_inventoryEntry, int a_soldItems, bool is_equipped);
		static void ModCrimeGoldValueHook(RE::Actor* a_this, RE::TESFaction* a_faction, bool a_violent, int32_t a_amount);

		static inline REL::HookVFT _Hook1{ RE::VTABLE_PlayerCharacter[0], 0x5A, OnItemAdded };
		static inline REL::HookVFT _Hook2{ RE::VTABLE_PlayerCharacter[0], 0xCC, PickupObject};
		static inline REL::HookVFT _Hook3{ RE::VTABLE_PlayerCharacter[0], 0x0B6, ModCrimeGoldValueHook};
		static inline REL::Hook _Hook6{ REL::ID(50951), 0x1f9, GetExtraData }; // 0x2ca // 0x121

		static void CalculateReputationLootChance(); // changes a ChanceNone global for leveled lists. used for special loot with high thief reputation.
		static bool IsNight();
	};

	struct NightThiefFencing : RE::GFxFunctionHandler
	{
		static int32_t GetTotalItemCountFenced();
		static int32_t GetTotalItemValueFenced();

		NightThiefFencing(const RE::GFxValue&& old, RE::BarterMenu* menu)
			: oldFunc(old), a_this(menu) {}
		void Call(Params& a_params) override;
		static void IncreaseFencedItemCountAndAmount(uint32_t item_count, uint32_t item_value);

	private:
		static void PostCreate(RE::BarterMenu* a_this);
		static float CalculateNightThiefPrice();
		static float GetBarterPriceFactor(RE::Actor* player);
		static inline REL::HookVFT _Hook4{ RE::VTABLE_BarterMenu[0], 0x2, PostCreate };

		RE::GFxValue oldFunc;
		RE::BarterMenu* a_this;
	};
}