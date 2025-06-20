export module generalStealing;

export namespace Stealing {
	struct NightThief {

		static uint32_t GetNightReputation();
		static void DecreaseReputation();
		static void IncreaseNightReputation(uint16_t amount);

	private:
		static void PickupObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound);
		static void OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr);
		static void OnGettingCaught(RE::PlayerCharacter* a_this, RE::Actor* a_caughtBy, float a3);

		static inline REL::HookVFT _Hook1{ RE::VTABLE_PlayerCharacter[0], 0x5A, OnItemAdded };
		static inline REL::HookVFT _Hook2{ RE::VTABLE_PlayerCharacter[0], 0xCC, PickupObject};
		static inline REL::Hook _Hook3{ REL::ID(38626), 0x4fc, OnGettingCaught };

		static bool IsNight();


	};

	struct NightThiefFencing : RE::GFxFunctionHandler
	{

	public:
		NightThiefFencing(const RE::GFxValue&& old, RE::BarterMenu* menu)
			: oldFunc(old), a_this(menu) {}

		void Call(Params& a_params) override;

	private:
		static void PostCreate(RE::BarterMenu* a_this);
		static float CalculateNightThiefPrice();
		static void IncreaseFencedItemCountAndAmount(uint32_t item_count, uint32_t item_value);
		static inline REL::HookVFT _Hook4{ RE::VTABLE_BarterMenu[0], 0x2, PostCreate };

		RE::GFxValue oldFunc;
		RE::BarterMenu* a_this;
	};
}




