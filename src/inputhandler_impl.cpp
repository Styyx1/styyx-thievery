module inputhandler;

import config;
#include "infamyHUD.h"

void InputManager::Register()
{
    if (const auto scripts = RE::BSInputDeviceManager::GetSingleton()) {
        scripts->AddEventSink<RE::InputEvent*>(GetSingleton());
        REX::INFO("Registered for {}", typeid(RE::InputEvent).name());
    }
}

void InputManager::SetEditorKey()
{
    if (!activate_editor_key.SetPattern(Config::Settings::editor_key.GetValue())) {
        REX::ERROR("failed to set editor key");
    }
}

void InputManager::ToggleEditMode(const hotkeys::KeyCombination* key)
{
    InfamyHUD::InfamyHUDDisplay::GetSingleton()->ToggleEditorMode();
}

void InputManager::ToggleClockVisibility(const hotkeys::KeyCombination* key)
{
}

RE::BSEventNotifyControl InputManager::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
{
    using EventType = RE::INPUT_EVENT_TYPE;
    using Result = RE::BSEventNotifyControl;

    if (!a_event)
        return Result::kContinue;

    activate_editor_key.Process(a_event);	

    return Result::kContinue;
}
