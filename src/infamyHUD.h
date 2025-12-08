
#pragma once
import config;
#include <st-ui.h>
#include <windows.h>
#include <wtypes.h>



namespace InfamyHUD
{
	static const std::string FALLBACK = "Data/SKSE/Plugins/InfamyMeter/Flame.png";
using namespace ImGuiMCP;

namespace InfamyBar
{
struct InfamyBarData : REX::Singleton<InfamyBarData>
{
    float icon_size = 128.f;
    float pos_x = 600.f;
    float pos_y = 45.f;
    bool is_visible = true;
    bool is_edit_mode = false;
    std::string tex_path_folder = "Data/SKSE/Plugins/InfamyMeter/";

    bool GetIsVisible() const
    {
        return is_visible;
    }
    void SetIsVisible(bool val)
    {
        is_visible = val;
    }
    bool GetIsEditMode() const
    {
        return is_edit_mode;
    }
    void SetIsEditMode(bool val)
    {
        is_edit_mode = val;
    }

    void InitData()
    {
        if (isInitialized)
            return;
        icon_size = Config::Settings::icon_size.GetValue();
        pos_x = Config::Settings::bar_pos_x.GetValue();
        pos_y = Config::Settings::bar_pos_y.GetValue();
        is_visible = Config::Settings::show_infamy_meter.GetValue();
        isInitialized = true;
    }

  private:
    bool isInitialized = false;
};

struct InfamyBarRenderer : public REX::Singleton<InfamyBarRenderer>
{
};

static inline ImTextureID tex = nullptr;
void RegisterInfamyBar();
void __stdcall RenderOverlay();
bool __stdcall OnInput(RE::InputEvent *event);
void ChangeSizeAndReloadTexture(float newSize);
void LoadBarIcon(const std::string &tex_path, float size);
void DrawColoredIcon(ImDrawList *draw_list, ImVec2 &center, ImVec2 &texSize);
inline bool IsEditorModeActive()
{
    return InfamyBarData::GetSingleton()->GetIsEditMode();
};
static inline MENU_WINDOW EditorWindow;
float HeatFillPct();
ImU32 GetBarColor(float fraction);

}; // namespace InfamyBar
} // namespace InfamyHUD
namespace Menu
{
using ux = StyyxUtil::UIUtil;
template <class T>
bool SettingSlider(const char *label, T &slider_var, const T min, const T max, const char *fmt,
                   REX::TOML::Setting<T> &setting, const char *help)
{
    ImGuiMCP::ImGui::SetNextItemWidth(200.f);
    bool changed = ImGuiMCP::ImGui::SliderScalar(label, ImGuiMCP::ImGuiDataType_::ImGuiDataType_Float, &slider_var,
                                                 &min, &max, fmt);
    if (changed)
    {
        setting.SetValue(slider_var);
    }
    ImGuiMCP::ImGui::SameLine();
    ux::HelpMarker(help);
    return changed;
}

template <class T>
bool SettingSliderINT(const char *label, T &slider_var, const T min, const T max, const char *fmt,
                      REX::TOML::Setting<T> &setting, const char *help)
{

    ImGuiMCP::ImGui::SetNextItemWidth(200.f);
    bool changed =
        ImGuiMCP::ImGui::SliderScalar(label, ImGuiMCP::ImGuiDataType_::ImGuiDataType_S32, &slider_var, &min, &max, fmt);
    if (changed)
    {
        setting.SetValue(slider_var);
    }
    ImGuiMCP::ImGui::SameLine();
    ux::HelpMarker(help);
    return changed;
}

inline bool SettingCheckbox(const char *label, bool &value_var, REX::TOML::Setting<bool> &setting, const char *help)
{
    bool changed = ImGuiMCP::ImGui::Checkbox(label, &value_var);
    if (changed)
    {
        setting.SetValue(value_var);
    }
    ImGuiMCP::ImGui::SameLine();
    ux::HelpMarker(help);
    return changed;
}

namespace Titles
{
inline std::string MOD_TITLE = "Fortunes And Real Trouble - A Thievery Gameplay Overhaul";
inline std::string SETTINGS_SEC = "Settings";
} // namespace Titles
namespace Label
{
inline std::string system = FontAwesome::UnicodeToUtf8(0xf390) + " System";
inline std::string save_settings = FontAwesome::UnicodeToUtf8(0xf0c7) + " Save Settings";
inline std::string restore_defaults = FontAwesome::UnicodeToUtf8(0xf0e2) + " Reset Settings";
} // namespace Label
void RegisterThiefMenu();
void RestoreFromSettings();
void RenderSystem();
void ResetDefaults();

namespace Settings
{

    inline bool capture_key_input = false;
    
namespace Titles
{
inline std::string LOCKPICKING_TIMER = "Lockpicking Timer";
inline std::string PICKPOCKET_TIMER = "Pickpocket Timer";
inline std::string PICKPOCKET_SETTINGS = FontAwesome::UnicodeToUtf8(0xf51e) + " Pickpocket Settings";
inline std::string LOCKPICKING_SETTINGS = FontAwesome::UnicodeToUtf8(0xf3c1) + " Lockpicking Settings";
inline std::string REPUTATION_SETTINGS = FontAwesome::UnicodeToUtf8(0xf6d5) + " Reputation Settings";
inline std::string INFAMY_METER_SETTINGS = FontAwesome::UnicodeToUtf8(0xf06d) + " Infamy Meter Settings";
inline std::string INFAMY_EDITOR_SETTINGS = "Infamy Meter Editor";
inline std::string REPUTATION_PERKS = "Reputation Perks";
inline std::string KEYBINDS = "Keybinds";
} // namespace Titles
namespace Label
{
// Lockpicking
inline std::string enable_lockpick_timer = "Enable Lockpick Timer";
inline std::string remove_lockpick_on_timeout = "Remove Lockpick on Timeout";
inline std::string lockpicking_min_time = "Lockpicking Minimum Time";
inline std::string lockpicking_max_time = "Lockpicking Maximum Time";
inline std::string enable_dynamic_lockpicking = "Enable Dynamic Lock Difficulty";

// Pickpocket 
inline std::string enable_pickpocket_timer = "Enable Pickpocket Timer";
inline std::string enable_dyn_pickpocket_cap = "Enable Dynamic Pickpocket Chance";
inline std::string pickpocket_min_time = "Pickpocket Minimum Time";
inline std::string pickpocket_max_time = "Pickpocket Maximum Time";

// Reputation
inline std::string reputation_min_item_value = "Minimum Stolen Item Value for Reputation Loss";
inline std::string hourly_heat_decrease = "Hourly Heat Decrease";
inline std::string fence_value_heat_threshold = "Fence Value Heat Threshold";
inline std::string show_infamy_meter = "Show Infamy Meter";

// HUD / Infamy Meter
inline std::string bar_pos_x = "Meter Position X";
inline std::string bar_pos_y = "Meter Position Y";
inline std::string icon_size = "Icon Size";

inline std::string texture_name = "Infamy Meter Texture Name";

// Perks
inline std::string show_gold_rush_screen = "Show Gold Rush Screen Effect";
inline std::string screen_notif_text = "Gold Rush Screen Notification Text";
inline std::string enable_gold_rush_sound = "Enable Gold Rush Sound";
inline std::string gold_rush_shader_duration = "Gold Rush Shader Duration";

// Keybinds
inline std::string editor_key = "Key to Open Infamy Editor";
} // namespace Label

namespace Tool
{
// Timers
inline std::string enable_lockpick_timer = "Toggle timed Lockpicking.";
inline std::string enable_pickpocket_timer = "Toggle Pickpocket timer";

// Pickpocket / Lockpicking
inline std::string enable_dyn_pickpocket_cap = "Toggle dynamic pickpocket cap";
inline std::string remove_lockpick_on_timeout = "Removes a lockpick if the timer expires during lockpicking.";
inline std::string pickpocket_min_time = "Minimum time you have to complete a pickpocket attempt.";
inline std::string pickpocket_max_time = "Maximum time you have to complete a pickpocket attempt.";
inline std::string lockpicking_min_time = "Minimum time you have to complete a lockpick attempt.";
inline std::string lockpicking_max_time = "Maximum time you have to complete a lockpick attempt.";
inline std::string enable_dynamic_lockpicking = "Enables dynamic lock difficulty scaling.";

// Reputation
inline std::string reputation_min_item_value = "The minimum value of stolen items that starts generating reputation.";
inline std::string hourly_heat_decrease = "Amount of infamy removed each in-game hour.";
inline std::string fence_value_heat_threshold = "Minimum item value for fencing to build up infamy.";
inline std::string show_infamy_meter = "Displays an on-screen infamy icon.";

// HUD
inline std::string bar_pos_x = "Horizontal position of the infamy meter.";
inline std::string bar_pos_y = "Vertical position of the infamy meter.";
inline std::string icon_size = "Size of the loaded icon.";

inline std::string texture_name = "Filename of the texture used for the Infamy Meter icon.";

// Perks
inline std::string show_gold_rush_screen = "Shows a special screen effect during Gold Rush perk trigger.";
inline std::string screen_notif_text = "Text shown when Gold Rush activates.";
inline std::string enable_gold_rush_sound = "Plays a sound when Gold Rush activates.";
inline std::string gold_rush_shader_duration = "Duration of the Gold Rush shader/screen effect.";

// Keybind
inline std::string editor_key = "Choose the key that opens the infamy editor mode.";
} // namespace Tool

namespace Var
{
inline bool enable_lockpick_timer;
inline bool enable_pickpocket_timer;
inline bool enable_dyn_pickpocket_cap;
inline bool remove_lockpick_on_timeout;
inline float pickpocket_min_time;
inline float pickpocket_max_time;
inline float lockpicking_min_time;
inline float lockpicking_max_time;

inline int reputation_min_item_value;
inline int hourly_heat_decrease;
inline int fence_value_heat_threshold;
inline bool show_infamy_meter;
inline bool enable_dynamic_lockpicking;

// HUD
inline float bar_pos_x;
inline float bar_pos_y;
inline float icon_size;

inline std::string texture_name;

// Perks
inline bool show_gold_rush_screen;
inline std::string screen_notif_text;
inline bool enable_gold_rush_sound;
inline float gold_rush_shader_duration;

// Keybind
inline uint32_t visibility_key;
} // namespace Var
inline void DrawHotkeyConfigUI();

void __stdcall RenderSettings();

} // namespace Settings

} // namespace Menu
