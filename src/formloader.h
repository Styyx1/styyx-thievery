#pragma once

namespace FormLoader{

	namespace LoaderConstants {
		inline const char* thievery_mod_name = "styyx-thievery.esl";
		static inline RE::FormID night_reputation_ID = 0x1;
		static inline RE::FormID total_amount_item_ID = 0x2;
		static inline RE::FormID total_value_item_ID = 0x3;
	}

	struct Loader : REX::Singleton<Loader> 
	{
		RE::TESGlobal* night_reputation_global;
		RE::TESGlobal* total_amount_item_fenced;
		RE::TESGlobal* total_value_item_fenced;

		bool CheckModLoaded(const char* mod_name);

		void LoadForms();
	};
}
