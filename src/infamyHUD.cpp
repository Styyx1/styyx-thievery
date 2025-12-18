#include "infamyHUD.h"
#include "formloader.h"
#include <dxgi.h>

import heat;
import generalStealing;
import lockpicking;
import Pickpocket;

#undef ERROR



namespace InfamyHUD
{

void InfamyBar::RegisterInfamyBar()
{
    if (!SKSEMenuFramework::IsInstalled())
    {
        return;
    }
    SKSEMenuFramework::AddHudElement(InfamyHUD::InfamyBar::RenderOverlay);
    SKSEMenuFramework::AddInputEvent(InfamyHUD::InfamyBar::OnInput);
}
// Runs every frame to render the overlay
void __stdcall InfamyBar::RenderOverlay()
{
    /*if (SKSEMenuFramework::IsAnyBlockingWindowOpened())
        return;*/
    InfamyBarData *data = InfamyBarData::GetSingleton();
    data->InitData();

    auto drawList = ImGui::GetForegroundDrawList();

    ImVec2 center = ImVec2(data->pos_x, data->pos_y);
    ImVec2 texSize(data->icon_size, data->icon_size);
    // Load texture if not already loaded
    if (!tex)
    {
        const std::string texPath = data->tex_path_folder + Config::Settings::texture_name.GetValue();
        REX::DEBUG("Loading texture from path: {}", texPath);
        LoadBarIcon(texPath, data->icon_size);
    }

    DrawColoredIcon(drawList, center, texSize);
}
bool __stdcall InfamyBar::OnInput(RE::InputEvent *event)
{
    bool blockThisUserInput = false;
    
    if (event->device == RE::INPUT_DEVICE::kKeyboard)
    {
        if (auto button = event->AsButtonEvent())
        {
            if (Menu::Settings::capture_key_input) {
                if (button->IsDown()) {
					Menu::Settings::Var::visibility_key = button->GetIDCode();
                }
            }

            if (button->GetIDCode() == Config::Settings::visibility_key.GetValue() && button->IsDown())
            {
                bool isVisible = InfamyBarData::GetSingleton()->GetIsVisible();
                InfamyBarData::GetSingleton()->SetIsVisible(!isVisible);
            }
        }
    }
    return blockThisUserInput;
}

void InfamyBar::ChangeSizeAndReloadTexture(float newSize)
{
    Config::Settings::icon_size.SetValue(newSize);
    ImVec2 texSize(newSize, newSize);
    // Reload texture with new size
    InfamyBarData *data = InfamyBarData::GetSingleton();
    std::string path = data->tex_path_folder + Config::Settings::texture_name.GetValue();
    tex = SKSEMenuFramework::LoadTexture(path, texSize);
    if (!tex)
    {
        path = FALLBACK;
        tex = SKSEMenuFramework::LoadTexture(path, texSize);
    }
    LoadBarIcon(path, data->icon_size);
}

void InfamyBar::LoadBarIcon(const std::string &tex_path, float size)
{
    ImVec2 texSize(size, size);
    tex = SKSEMenuFramework::LoadTexture(tex_path, texSize);
}
void InfamyBar::DrawColoredIcon(ImDrawList *draw_list, ImVec2 &center, ImVec2 &texSize)
{
    float pct = HeatFillPct();

    ImU32 barColor = GetBarColor(pct);

    ImVec2 full0(center.x - texSize.x, center.y - texSize.y);
    ImVec2 full1(center.x + texSize.x, center.y + texSize.y);

    ImVec2 uv10(0.0f, 0.0f);
    ImVec2 uv11(1.0f, 1.0f);
    if (InfamyBarData::GetSingleton()->GetIsVisible())
        ImGui::ImDrawListManager::AddImage(draw_list, tex, full0, full1, uv10, uv11, barColor);
}

float InfamyBar::HeatFillPct()
{
    float ret_val = 0.5f;
    const auto &formL = FormLoader::Loader::GetSingleton();
    if (formL->player_heat_system_global)
    {
        ret_val = std::clamp(HeatSystem::GetHeatValue() / 100.0f, 0.0f, 100.0f);
    }
    return ret_val;
}

ImU32 InfamyBar::GetBarColor(float fraction)
{
    ImVec4 color;

    if (fraction <= 0.5f)
    {
        // Blue -> Yellow
        float t = fraction / 0.5f;
        color.x = (1.0f - t) * 0.0f + t * 1.0f; // R
        color.y = (1.0f - t) * 0.0f + t * 1.0f; // G
        color.z = (1.0f - t) * 1.0f + t * 0.0f; // B
    }
    else if (fraction <= 0.7f)
    {
        // Yellow -> Orange
        float t = (fraction - 0.5f) / 0.2f;
        color.x = (1.0f - t) * 1.0f + t * 1.0f; // R
        color.y = (1.0f - t) * 1.0f + t * 0.5f; // G
        color.z = (1.0f - t) * 0.0f + t * 0.0f; // B
    }
    else if (fraction <= 0.9f)
    {
        // Orange -> Red
        float t = (fraction - 0.7f) / 0.2f;
        color.x = 1.0f;
        color.y = (1.0f - t) * 0.5f + t * 0.0f;
        color.z = 0.0f;
    }
    else
    {
        // Max Red
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    color.w = 0.9f; 
    return ImGui::ColorConvertFloat4ToU32(color);
}

} // namespace InfamyHUD

namespace Menu
{
void RegisterThiefMenu()
{
    if (!SKSEMenuFramework::IsInstalled())
    {
        return;
    }
    SKSEMenuFramework::SetSection(Titles::MOD_TITLE);
    SKSEMenuFramework::AddSectionItem(Titles::SETTINGS_SEC, Settings::RenderSettings);
    RestoreFromSettings();
}
void RestoreFromSettings()
{

    using namespace Menu::Settings::Var;
    using set = Config::Settings;

    enable_lockpick_timer = set::enable_lockpick_timer.GetValue();
    enable_pickpocket_timer = set::enable_pickpocket_timer.GetValue();
    enable_dyn_pickpocket_cap = set::enable_dyn_pickpocket_cap.GetValue();
    remove_lockpick_on_timeout = set::remove_lockpick_on_timeout.GetValue();

    pickpocket_min_time = set::pickpocket_min_time.GetValue();
    pickpocket_max_time = set::pickpocket_max_time.GetValue();
    lockpicking_min_time = set::lockpicking_min_time.GetValue();
    lockpicking_max_time = set::lockpicking_max_time.GetValue();

    reputation_min_item_value = set::reputation_min_item_value.GetValue();
    hourly_heat_decrease = set::hourly_heat_decrease.GetValue();
    fence_value_heat_threshold = set::fence_value_heat_threshold.GetValue();
    show_infamy_meter = set::show_infamy_meter.GetValue();
    enable_dynamic_lockpicking = set::enable_dynamic_lockpicking.GetValue();
    
    bar_pos_x = set::bar_pos_x.GetValue();
    bar_pos_y = set::bar_pos_y.GetValue();
    icon_size = set::icon_size.GetValue();
    texture_name = set::texture_name.GetValue();

    screen_notif_text = set::screen_notif_text.GetValue();
    enable_gold_rush_sound = set::enable_gold_rush_sound.GetValue();
    gold_rush_shader_duration = set::gold_rush_shader_duration.GetValue();

    visibility_key = set::visibility_key.GetValue();
}
void RenderSystem()
{
    ImGuiMCP::ImGui::NewLine();
    ImGuiMCP::ImGui::SeparatorText(Label::system.c_str());

    if (ImGuiMCP::ImGui::Button(Label::save_settings.c_str()))
    {
        Config::Settings::GetSingleton()->Update(true);
    }

    ImGuiMCP::ImGui::SameLine();
    if (ImGuiMCP::ImGui::Button(Label::restore_defaults.c_str()))
    {
        ResetDefaults();
    }
}
void ResetDefaults()
{
    using set = Config::Settings;
    using namespace Settings::Var;

    enable_lockpick_timer = true;
    enable_pickpocket_timer = true;
    enable_dyn_pickpocket_cap = true;
    remove_lockpick_on_timeout = true;

    pickpocket_min_time = 10.0f;
    pickpocket_max_time = 50.0f;
    lockpicking_min_time = 10.0f;
    lockpicking_max_time = 50.0f;

    reputation_min_item_value = 50;
    hourly_heat_decrease = 2;
    fence_value_heat_threshold = 158;
    show_infamy_meter = true;
    enable_dynamic_lockpicking = true;

    bar_pos_x = 500.0f;
    bar_pos_y = 50.0f;
    icon_size = 128.f;
    texture_name = "Flame.png";

    screen_notif_text = "Something catches your eye...";
    enable_gold_rush_sound = true;
    gold_rush_shader_duration = 30.0f;

    visibility_key = 65;

    set::enable_lockpick_timer.SetValue(enable_lockpick_timer);
    set::enable_pickpocket_timer.SetValue(enable_pickpocket_timer);
    set::enable_dyn_pickpocket_cap.SetValue(enable_dyn_pickpocket_cap);
    set::remove_lockpick_on_timeout.SetValue(remove_lockpick_on_timeout);

    set::pickpocket_min_time.SetValue(pickpocket_min_time);
    set::pickpocket_max_time.SetValue(pickpocket_max_time);
    set::lockpicking_min_time.SetValue(lockpicking_min_time);
    set::lockpicking_max_time.SetValue(lockpicking_max_time);

    set::reputation_min_item_value.SetValue(reputation_min_item_value);
    set::hourly_heat_decrease.SetValue(hourly_heat_decrease);
    set::fence_value_heat_threshold.SetValue(fence_value_heat_threshold);
    set::show_infamy_meter.SetValue(show_infamy_meter);
    set::enable_dynamic_lockpicking.SetValue(enable_dynamic_lockpicking);

    set::bar_pos_x.SetValue(bar_pos_x);
    set::bar_pos_y.SetValue(bar_pos_y);
    set::icon_size.SetValue(icon_size);
    set::texture_name.SetValue(texture_name);

    set::screen_notif_text.SetValue(screen_notif_text);
    set::enable_gold_rush_sound.SetValue(enable_gold_rush_sound);
    set::gold_rush_shader_duration.SetValue(gold_rush_shader_duration);

    set::visibility_key.SetValue(visibility_key);

    set::GetSingleton()->Update(true);
}
} // namespace Menu

void Menu::Settings::DrawHotkeyConfigUI()
{
    std::string key_name = hotkeys::details::GetNameByKey(Config::Settings::visibility_key.GetValue()).data();

    //not really needed but looks better in the menu
	std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::toupper);

    ImGui::Text(std::format("Hotkey: {}", key_name).c_str());
    ImGui::SameLine();

    if (!Menu::Settings::capture_key_input) {
        if (ImGui::Button("Rebind")) {
            Menu::Settings::capture_key_input = true;
            Menu::Settings::Var::visibility_key = 0;
        }
        ImGui::SameLine();
		ux::HelpMarker("Press the desired key to rebind the visibility toggle.");
    }
    else {
        ImGui::Text("Press any key");
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            Menu::Settings::capture_key_input = false;
			Settings::Var::visibility_key = Config::Settings::visibility_key.GetValue();
        }
    }
    if (Menu::Settings::Var::visibility_key != 0)
    {
        Config::Settings::visibility_key.SetValue(Menu::Settings::Var::visibility_key);
        Menu::Settings::capture_key_input = false;
    }
}
void __stdcall Menu::Settings::RenderSettings()
{
    using set = Config::Settings;
    FontAwesome::PushSolid();
    ImGui::Text(Menu::Titles::MOD_TITLE.c_str());
    ImGui::NewLine();

    // ---------------------------
    // --------Lockpicking--------
    // ---------------------------
    ImGui::SeparatorText(Menu::Settings::Titles::LOCKPICKING_SETTINGS.c_str());

    SettingCheckbox(Label::enable_lockpick_timer.c_str(), Var::enable_lockpick_timer, set::enable_lockpick_timer,
                    Tool::enable_lockpick_timer.c_str());
	ImGui::SameLine();
    SettingCheckbox(Label::remove_lockpick_on_timeout.c_str(), Var::remove_lockpick_on_timeout,
                    set::remove_lockpick_on_timeout, Tool::remove_lockpick_on_timeout.c_str());

    SettingCheckbox(Label::enable_dynamic_lockpicking.c_str(), Var::enable_dynamic_lockpicking,
                    set::enable_dynamic_lockpicking, Tool::enable_dynamic_lockpicking.c_str());

    SettingSlider(Label::lockpicking_min_time.c_str(), Var::lockpicking_min_time, 5.0f, 30.0f, "%.1fs",
                  set::lockpicking_min_time, Tool::lockpicking_min_time.c_str());

    SettingSlider(Label::lockpicking_max_time.c_str(), Var::lockpicking_max_time, 31.0f, 120.0f, "%.1fs",
                  set::lockpicking_max_time, Tool::lockpicking_max_time.c_str());

    // ---------------------------
    // -------Pickpocketing-------
    // ---------------------------

    ImGui::SeparatorText(Menu::Settings::Titles::PICKPOCKET_SETTINGS.c_str());

    SettingCheckbox(Label::enable_pickpocket_timer.c_str(), Var::enable_pickpocket_timer, set::enable_pickpocket_timer,
                    Tool::enable_pickpocket_timer.c_str());
    ImGui::SameLine();
    SettingCheckbox(Label::enable_dyn_pickpocket_cap.c_str(), Var::enable_dyn_pickpocket_cap,
                    set::enable_dyn_pickpocket_cap, Tool::enable_dyn_pickpocket_cap.c_str());

    SettingSlider(Label::pickpocket_min_time.c_str(), Var::pickpocket_min_time, 5.0f, 30.0f, "%.1fs",
                  set::pickpocket_min_time, Tool::pickpocket_min_time.c_str());

    SettingSlider(Label::pickpocket_max_time.c_str(), Var::pickpocket_max_time, 31.0f, 120.0f, "%.1fs",
                  set::pickpocket_max_time, Tool::pickpocket_max_time.c_str());

    // ---------------------------
    // --------Reputation---------
    // ---------------------------

    ImGui::SeparatorText(Menu::Settings::Titles::REPUTATION_SETTINGS.c_str());

    SettingSliderINT(Label::reputation_min_item_value.c_str(), Var::reputation_min_item_value, 30, 400, "%d",
                     set::reputation_min_item_value, Tool::reputation_min_item_value.c_str());

    SettingSliderINT(Label::hourly_heat_decrease.c_str(), Var::hourly_heat_decrease, 1, 10, "%d",
                     set::hourly_heat_decrease, Tool::hourly_heat_decrease.c_str());

    SettingSliderINT(Label::fence_value_heat_threshold.c_str(), Var::fence_value_heat_threshold, 100, 600, "%d",
                     set::fence_value_heat_threshold, Tool::fence_value_heat_threshold.c_str());

    ImGui::SeparatorText(Menu::Settings::Titles::INFAMY_METER_SETTINGS.c_str());

    if (SettingCheckbox(Label::show_infamy_meter.c_str(), Var::show_infamy_meter, set::show_infamy_meter,
                        Tool::show_infamy_meter.c_str()))
    {
        InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->SetIsVisible(Var::show_infamy_meter);
    };

    // ---------------------------
    // -----------Size------------
    // ---------------------------
    ImGui::SeparatorText("Size");

    if (SettingSlider(Label::icon_size.c_str(), Var::icon_size, 32.0f, 512.0f, "%.0f px", set::icon_size,
                      Tool::icon_size.c_str()))
    {
        InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->icon_size = Var::icon_size;
        InfamyHUD::InfamyBar::ChangeSizeAndReloadTexture(set::icon_size.GetValue());
    };

    // ---------------------------
    // --------Positioning--------
    // ---------------------------
    ImGui::SeparatorText("Position");

    if (SettingSlider(Label::bar_pos_x.c_str(), Var::bar_pos_x, 0.0f, 4000.0f, "%.0f px", set::bar_pos_x,
                      Tool::bar_pos_x.c_str()))
    {
        InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->pos_x = Var::bar_pos_x;
    };

    if (SettingSlider(Label::bar_pos_y.c_str(), Var::bar_pos_y, 0.0f, 4000.0f, "%.0f px", set::bar_pos_y,
                      Tool::bar_pos_y.c_str()))
    {
        InfamyHUD::InfamyBar::InfamyBarData::GetSingleton()->pos_y = Var::bar_pos_y;
    };

    char buffer[256];
    strncpy_s(buffer, Var::texture_name.c_str(), sizeof(buffer));
    if (ImGui::InputText(Label::texture_name.c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        Var::texture_name = std::string(buffer);
        Config::Settings::texture_name.SetValue(Var::texture_name);
        InfamyHUD::InfamyBar::ChangeSizeAndReloadTexture(Config::Settings::icon_size.GetValue());
    }
    ImGui::SameLine();
    ux::HelpMarker(Tool::texture_name.c_str());

    // ---------------------------
    // --------Perk Options-------
    // ---------------------------
    ImGui::SeparatorText("Effects");

    SettingCheckbox(Label::enable_gold_rush_sound.c_str(), Var::enable_gold_rush_sound, set::enable_gold_rush_sound,
                    Tool::enable_gold_rush_sound.c_str());

    SettingSlider(Label::gold_rush_shader_duration.c_str(), Var::gold_rush_shader_duration, 1.0f, 240.0f, "%.0f sec",
                  set::gold_rush_shader_duration, Tool::gold_rush_shader_duration.c_str());
    
	char notifBuffer[256];
    strncpy_s(notifBuffer, Var::screen_notif_text.c_str(), sizeof(notifBuffer));
	if (ImGui::InputText("Gold Rush Notification Text", notifBuffer, sizeof(notifBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        Var::screen_notif_text = std::string(notifBuffer);
	    Config::Settings::screen_notif_text.SetValue(Var::screen_notif_text);
	}
    ImGui::SameLine();
	ux::HelpMarker(Tool::screen_notif_text.c_str());

    //---------------------------
    //Hotkey and Saving/Resetting
    //---------------------------
    DrawHotkeyConfigUI();
    ImGui::NewLine();
    RenderSystem();

    FontAwesome::Pop();
}

