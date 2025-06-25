#include "formloader.h"

namespace FormLoader {

	bool Loader::CheckModLoaded(const char* mod_name)
	{
		const auto& dataHandler = RE::TESDataHandler::GetSingleton();
		if (auto file = dataHandler->LookupModByName(mod_name); file && file->compileIndex != 0xFF) {
			REX::INFO("{} is active. Proceed to load data", mod_name);
			return true;
		}
		return false;
	}
	void Loader::LoadForms(){
		const auto& dataHandler = RE::TESDataHandler::GetSingleton();
		using namespace LoaderConstants;
		if (CheckModLoaded(LoaderConstants::thievery_mod_name)) {
			night_reputation_global = dataHandler->LookupForm<RE::TESGlobal>(night_reputation_ID, thievery_mod_name);
			total_amount_item_fenced = dataHandler->LookupForm<RE::TESGlobal>(total_amount_item_ID, thievery_mod_name);
			total_value_item_fenced = dataHandler->LookupForm<RE::TESGlobal>(total_value_item_ID, thievery_mod_name);	
			fence_rep_special_item_global = dataHandler->LookupForm<RE::TESGlobal>(fence_rep_special_item_ID, thievery_mod_name);
			player_heat_system_global = dataHandler->LookupForm<RE::TESGlobal>(player_heat_ID, thievery_mod_name);
			//extern
			lockpick_item = dataHandler->LookupForm<RE::TESObjectMISC>(lockpicks_ID,skyrim_esm);

			return;
		}
		else {
			auto fail_text_mod_loader = std::format("{} not loaded, please enable the plugin. It is mandatory for the mod to work.", LoaderConstants::thievery_mod_name);
			REX::FAIL(fail_text_mod_loader.c_str());
		}			
	};

}


