export module Pickpocket;

export struct PickpocketTimer {
	static float GetRemainingPickpocketTime();
	static bool GetIsPTimerRunning();
	static bool IsPTimerEnabled();
	static inline bool _PTimerEnabled;
	static void InitPickpocketCapChange();
	static void ResetPickpocketChance();
	static void ResetStoredPickpocketChance();

private:
	static RE::UI_MESSAGE_RESULTS ProcessMenu(RE::ContainerMenu* a_this, RE::UIMessage& a_message);
	static inline REL::HookVFT _Hook0{ RE::VTABLE_ContainerMenu[0], 0x04, ProcessMenu };
	static void StartPickPocketTimer();
	static float ActorValueLevelTimeModifier(RE::PlayerCharacter* player);
	static float GetPickpocketChanceFromFile();
	static void SetPickpocketChance(float new_cap);
	
	static void AdjustPickpocketCapWithHeat();
	
	static uint32_t CalculatePickPocketChancePlayer(float a_thiefSkill, float a_targetSkill, std::uint32_t a_valueStolen, float a_weightStolen, RE::Actor* a_thief, RE::Actor* a_target, bool a_isDetected, RE::TESForm* a_itemPickpocketing);
	static uint32_t CalculatePickPocketChance(float a_thiefSkill, float a_targetSkill, std::uint32_t a_valueStolen, float a_weightStolen, RE::Actor* a_thief, RE::Actor* a_target, bool a_isDetected, RE::TESForm* a_itemPickpocketing);

	static inline REL::Hook _Hook13{ REL::ID(40654), 0x12c, CalculatePickPocketChancePlayer };
	static inline REL::Hook _Hook14{ REL::ID(51130), 0x213, CalculatePickPocketChance };

	/*
	
	fPickPocketActorSkillBase 20.0
	fPickPocketActorSkillMult 1.0
	fPickPocketAmountMult -0.1
	fPickPocketMaxChance 90.0
	fPickPocketMinChance 0.0
	fPickpocketSkillUsesCurve 0.8
	fPickPocketTargetSkillBase 20.0
	fPickPocketTargetSkillMult -0.25
	fPickPocketWeightMult -4.0
	
	*/

};