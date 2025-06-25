export module lockpicking;

export struct LockpickingMenu {
	static float GetRemainingLockpickTimer();
	static float GetCalcTimerForLockpicking();
	static bool GetIsTimerRunning();
	static bool IsTimerEnabled();

	static inline bool _timerEnabled;

private:
	static RE::UI_MESSAGE_RESULTS ProcessMessage(RE::LockpickingMenu* a_this, RE::UIMessage& a_message);
	static inline REL::HookVFT _Hook5{ RE::VTABLE_LockpickingMenu[0], 0x4, ProcessMessage };
	static void StartLockpickTimer();
	static void StopLockpickTimer();
	static void LockpickConsequeces(RE::Actor* player, bool isCrime);
	
};