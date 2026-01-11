export module config;

namespace Config
{
    namespace Constants
    {
        inline constexpr std::string_view toml_path_default = "Data/SKSE/Plugins/styyx-thievery.toml";
        inline constexpr std::string_view toml_path_custom  = "Data/SKSE/Plugins/styyx-thievery_custom.toml";
        inline constexpr std::string_view POCKET_SEC = "Pickpocket";
        inline constexpr std::string_view LOCK_SEC = "Lockpicking";
        inline constexpr std::string_view REP_SEC = "Reputation";
        inline constexpr std::string_view INF_SEC = "InfamyMeter";
        inline constexpr std::string_view KEY_SEC = "Keybinds";

    }
    using namespace Constants;
    export struct Settings : REX::Singleton<Settings>
    {
        static inline REX::TOML::Bool enable_lockpick_timer{ LOCK_SEC, "bEnableLockpickTimer", true };
        static inline REX::TOML::Bool enable_pickpocket_timer{ POCKET_SEC, "bEnablePickpocketTimer", true };
        static inline REX::TOML::Bool enable_dyn_pickpocket_cap{ POCKET_SEC, "bEnableDynamicPickpocketChance", true };
        static inline REX::TOML::Bool remove_lockpick_on_timeout{ LOCK_SEC, "bRemoveLockpickOnTimeout", true };
        static inline REX::TOML::F32 pickpocket_min_time{ POCKET_SEC, "fPickpocketMinTime", 10.0f };
        static inline REX::TOML::F32 pickpocket_max_time{ POCKET_SEC, "fPickpocketMaxTime", 50.0f };
        static inline REX::TOML::F32 lockpicking_min_time{ LOCK_SEC, "fLockpickingMinTime", 10.0f };
        static inline REX::TOML::F32 lockpicking_max_time{ LOCK_SEC, "fLockpickingMaxTime", 50.0f };
        static inline REX::TOML::I32 reputation_min_item_value{ REP_SEC, "iMinItemValueForReputation", 50 };
        static inline REX::TOML::I32 hourly_heat_decrease{ REP_SEC, "iHourlyHeatDecrease", 2 };
        static inline REX::TOML::I32 fence_value_heat_threshold{ REP_SEC, "iFenceItemValueHeatThreshold", 158 };
        static inline REX::TOML::Bool show_infamy_meter{ REP_SEC, "bShowInfamyMeter", true };
        static inline REX::TOML::Bool enable_dynamic_lockpicking{ LOCK_SEC, "bEnableDynamicLocks", true };
        // HUD Related Settings:
        static inline REX::TOML::F32 bar_pos_x{ INF_SEC, "fInfamyMeterPositionX", 655.0f };
        static inline REX::TOML::F32 bar_pos_y{ INF_SEC, "fInfamyMeterPositionY", 55.0f };
		static inline REX::TOML::F32 icon_size{ INF_SEC, "fInfamyIconSize", 50.0f };
		static inline REX::TOML::Str texture_name{ INF_SEC, "sInfamyMeterTexture", std::string("Flame.png") };

		 // Perk related settings
        static inline REX::TOML::Str screen_notif_text{ REP_SEC, "sGoldRushScreenText", std::string("Something catches your eye...") };
		static inline REX::TOML::Bool enable_gold_rush_sound{ REP_SEC, "bEnableGoldRushSound", true };
		static inline REX::TOML::F32 gold_rush_shader_duration{ REP_SEC, "fGoldRushShaderDuration", 30.0f };

        // keybinds
        static inline REX::TOML::U32 visibility_key{ KEY_SEC, "uToggleVisibility", uint32_t{65} }; // F7


        void Update(bool a_save = false);
    };
    inline void Settings::Update(bool a_save)
    {
        auto toml = REX::TOML::SettingStore::GetSingleton();
        toml->Init(Constants::toml_path_default.data(), Constants::toml_path_custom.data());
        if(!a_save)
            toml->Load();
        else 
			toml->Save();
    }
}