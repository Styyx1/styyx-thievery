export module Pickpocket;

struct PickpocketTimer {
	static RE::UI_MESSAGE_RESULTS ProcessMenu(RE::ContainerMenu* a_this, RE::UIMessage& a_message);

private:
	static inline REL::HookVFT _Hook0{ RE::VTABLE_ContainerMenu[0], 0x04, ProcessMenu };
	static void StartPickPocketTimer();
	static float ActorValueLevelTimeModifier(RE::PlayerCharacter* player);
	static void HideMenu( RE::BSFixedString a_menuName)
	{
		if (const auto UIMsgQueue = RE::UIMessageQueue::GetSingleton(); UIMsgQueue) {
			UIMsgQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
	}
};