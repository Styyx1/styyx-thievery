export module inputhandler;

export struct InputManager : public REX::Singleton<InputManager>, public RE::BSTEventSink<RE::InputEvent*> 
{
    void Register();
    void SetEditorKey();
    static void ToggleEditMode(const hotkeys::KeyCombination* key);
    static void ToggleClockVisibility(const hotkeys::KeyCombination* key);
private:
    
    
    hotkeys::KeyCombination activate_editor_key{ ToggleEditMode };
    hotkeys::KeyCombination toggle_clock_visibility{ ToggleClockVisibility };

protected:
    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;
};