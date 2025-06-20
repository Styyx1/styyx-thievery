export module config;

namespace Config
{
    namespace Constants
    {
        inline constexpr std::string_view toml_path_default = "Data/SKSE/Plugins/styyx-thievery.toml";
        inline constexpr std::string_view toml_path_custom  = "Data/SKSE/Plugins/styyx-thievery_custom.toml";
    }

    export struct Settings : REX::Singleton<Settings>
    {
        static inline REX::TOML::Bool enable_lockpick_timer{ "Lockpicking.Timer", "bEnableLockpickTimer", true };
        static inline REX::TOML::Bool enable_pickpocket_timer{ "Pickpocket.Timer", "bEnablePickpocketTimer", true };
        static inline REX::TOML::Bool enable_dyn_pickpocket_cap{ "PickPocket.Settings", "bEnableDynamicPickpocketChance", true };
        static inline REX::TOML::Bool remove_lockpick_on_timeout{ "Lockpicking.Settings", "bRemoveLockpickOnTimeout", true };
        static inline REX::TOML::F32 pickpocket_min_time{ "Pickpocket.Settings", "fPickpocketMinTime", 10.0f };
        static inline REX::TOML::F32 pickpocket_max_time{ "Pickpocket.Settings", "fPickpocketMaxTime", 50.0f };
        static inline REX::TOML::F32 lockpicking_min_time{ "Lockpicking.Settings", "fLockpickingMinTime", 10.0f };
        static inline REX::TOML::F32 lockpicking_max_time{ "Lockpicking.Settings", "fLockpickingMaxTime", 50.0f };
        static inline REX::TOML::I32 reputation_min_item_value{ "Reputation.Settings", "iMinItemValueForReputation", 50 };

        void Update();
    };
    inline void Settings::Update()
    {
        auto toml = REX::TOML::SettingStore::GetSingleton();
        toml->Init(Constants::toml_path_default.data(), Constants::toml_path_custom.data());
        toml->Load();
    }
}