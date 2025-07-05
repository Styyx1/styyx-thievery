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
		static inline RE::FormID gold_rush_ID = 0x7;
		static inline RE::FormID gold_rush_sound_ID = 0x9;
		static inline RE::FormID glow_shader_ID = 0xC;
		static inline RE::FormID gold_rush_perk_ID = 0xD;
		static inline RE::FormID sleight_of_hand_spell_ID = 0x10;
		static inline RE::FormID sleight_of_hand_effect_ID = 0xF;
		static inline RE::FormID exception_cell_formlist_ID = 0x11;
		static inline RE::FormID extra_kill_loot_rep_perk_ID = 0x12;
		static inline RE::FormID sleight_of_hand_perk_ID     = 0x13;
		static inline RE::FormID fence_price_increase_perk_ID = 0x14;
		static inline RE::FormID lower_bounty_perk_ID        = 0x15;
		static inline RE::FormID new_bribes_perk_ID          = 0x16;

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
		RE::TESImageSpaceModifier *gold_rush_imad;
		RE::BGSSoundDescriptorForm* gold_rush_sound;
		RE::TESEffectShader* glow_shader;
		RE::BGSPerk* gold_rush_perk;
		RE::SpellItem* sleight_of_hand_spell;
		RE::EffectSetting* sleight_of_hand_effect;
		RE::BGSListForm* exception_cell_formlist;
		RE::BGSPerk* extra_kill_loot_rep_perk;
		RE::BGSPerk* sleight_of_hand_perk;
		RE::BGSPerk* fence_price_increase_perk;
		RE::BGSPerk* lower_bounty_perk;
		RE::BGSPerk* new_bribes_perk;

		//extern
		RE::TESObjectMISC* lockpick_item;

		bool CheckModLoaded(const char* mod_name);

		void LoadForms();
	};
}
