#pragma once

namespace FormLoader{

	namespace LoaderConstants {
		inline const char* skyrim_esm = "Skyrim.esm";
		inline const char* thievery_mod_name = "styyx-thievery.esl";
		static inline RE::FormID night_reputation_ID = 0x1;
		static inline RE::FormID total_amount_item_ID = 0x2;
		static inline RE::FormID total_value_item_ID = 0x3;
		static inline RE::FormID fence_rep_special_item_ID = 0x4;
		static inline RE::FormID player_heat_ID = 0x5;
		//extern
		static inline RE::FormID lockpicks_ID = 0xA;
	}

	struct Loader : REX::Singleton<Loader> 
	{
		RE::TESGlobal* night_reputation_global;
		RE::TESGlobal* total_amount_item_fenced;
		RE::TESGlobal* total_value_item_fenced;
		RE::TESGlobal* fence_rep_special_item_global;
		RE::TESGlobal* player_heat_system_global;
		//extern
		RE::TESObjectMISC* lockpick_item;

		bool CheckModLoaded(const char* mod_name);

		void LoadForms();
	};
}
