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
		if (CheckModLoaded(LoaderConstants::thievery_mod_name)) {
			night_reputation_global = dataHandler->LookupForm<RE::TESGlobal>(LoaderConstants::night_reputation_ID, LoaderConstants::thievery_mod_name);
			total_amount_item_fenced = dataHandler->LookupForm<RE::TESGlobal>(LoaderConstants::total_amount_item_ID, LoaderConstants::thievery_mod_name);
			total_value_item_fenced = dataHandler->LookupForm<RE::TESGlobal>(LoaderConstants::total_value_item_ID, LoaderConstants::thievery_mod_name);	
			return;
		}
		else {
			auto fail_text_mod_loader = std::format("{} not loaded, please enable the plugin. It is mandatory for the mod to work.", LoaderConstants::thievery_mod_name);
			REX::FAIL(fail_text_mod_loader.c_str());
		}			
	};

}


