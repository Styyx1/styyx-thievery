#include "formloader.h"

template <typename... T>
bool AreAllFormsValid(T*... forms)
{
	return (... && (forms != nullptr));
}

template <typename... T>
bool CheckFormsAndLog(const char* context, T*... forms)
{
	bool allValid = AreAllFormsValid(forms...);
	if (!allValid) {
		REX::ERROR("[{}] One or more required forms failed to load!", context);
	}
	return allValid;
}

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
	void Loader::LoadForms() {
		const auto& dataHandler = RE::TESDataHandler::GetSingleton();
		using namespace LoaderConstants;

		if (CheckModLoaded(LoaderConstants::thievery_mod_name)) {
			night_reputation_global = dataHandler->LookupForm<RE::TESGlobal>(night_reputation_ID, thievery_mod_name);
			total_amount_item_fenced = dataHandler->LookupForm<RE::TESGlobal>(total_amount_item_ID, thievery_mod_name);
			total_value_item_fenced = dataHandler->LookupForm<RE::TESGlobal>(total_value_item_ID, thievery_mod_name);	
			fence_rep_special_item_global = dataHandler->LookupForm<RE::TESGlobal>(fence_rep_special_item_ID, thievery_mod_name);
			player_heat_system_global = dataHandler->LookupForm<RE::TESGlobal>(player_heat_ID, thievery_mod_name);

			//gold_rush_imad = dataHandler->LookupForm<RE::TESImageSpaceModifier>(gold_rush_ID, thievery_mod_name);
			gold_rush_sound = dataHandler->LookupForm<RE::BGSSoundDescriptorForm>(gold_rush_sound_ID, thievery_mod_name); // Optional if sound played by EDID
			glow_shader = dataHandler->LookupForm<RE::TESEffectShader>(glow_shader_ID, thievery_mod_name);

			gold_rush_perk = dataHandler->LookupForm<RE::BGSPerk>(gold_rush_perk_ID, thievery_mod_name); // Gold Rush
			sleight_of_hand_perk = dataHandler->LookupForm<RE::BGSPerk>(sleight_of_hand_perk_ID, thievery_mod_name); // Sleight of Hand
			fence_price_increase_perk = dataHandler->LookupForm<RE::BGSPerk>(fence_price_increase_perk_ID, thievery_mod_name); // Shady Merchant
			lower_bounty_perk = dataHandler->LookupForm<RE::BGSPerk>(lower_bounty_perk_ID, thievery_mod_name); // Friends in High Places
			no_bounty_perk = dataHandler->LookupForm<RE::BGSPerk>(no_bounty_perk_ID, thievery_mod_name); // Lucky Dodge

			sleight_of_hand_effect = dataHandler->LookupForm<RE::EffectSetting>(sleight_of_hand_effect_ID, thievery_mod_name);
			sleight_of_hand_spell = dataHandler->LookupForm<RE::SpellItem>(sleight_of_hand_spell_ID, thievery_mod_name);

			exception_cell_formlist = dataHandler->LookupForm<RE::BGSListForm>(exception_cell_formlist_ID, thievery_mod_name);

			// From Skyrim.esm
			lockpick_item = dataHandler->LookupForm<RE::TESObjectMISC>(lockpicks_ID, skyrim_esm);

			if (!CheckFormsAndLog("Thievery Init",
				night_reputation_global,
				total_amount_item_fenced,
				total_value_item_fenced,
				fence_rep_special_item_global,
				player_heat_system_global,
				//gold_rush_imad,
				glow_shader,
				gold_rush_perk,
				sleight_of_hand_effect,
				sleight_of_hand_spell,
				sleight_of_hand_perk,
				fence_price_increase_perk,
				lower_bounty_perk,
				no_bounty_perk,
				//extra_kill_loot_rep_perk,
				//lockpick_item,
				exception_cell_formlist)) 
			{
				REX::FAIL("Failed to load all required forms for Thievery mod. Please check your installation.");
			}
			return;
		}

		auto fail_text_mod_loader = std::format("{} not loaded, please enable the plugin. It is mandatory for the mod to work.", LoaderConstants::thievery_mod_name);
		REX::FAIL(fail_text_mod_loader.c_str());
	}
}