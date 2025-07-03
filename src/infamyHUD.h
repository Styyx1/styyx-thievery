#pragma once
#include <wtypes.h>

namespace InfamyHUD {

	struct Renderer {

	private:
		//Hook 11
		static void Present(uint32_t a1);
		static inline REL::Hook _Hook11{ REL::ID(77246), 0x9, &Present };

	};
	struct SwapChainHook {

	private:
		static void RendererInit();
		static void ApplyHUDStyle();
		static inline REL::Hook _Hook12{ REL::ID(77226), 0x275, &RendererInit };
		static inline bool bInitialized = false;
		static inline bool bStyleApplied = false;
		static inline bool bColorApplied = false;		
	};

	struct InfamyHUDDisplay : public REX::Singleton<InfamyHUDDisplay>
	{
		bool IsEditorModeActive() const;
		bool FocusRegained();
		void SetEditorMode(bool enable);
		bool FocusLost();
		void Draw(); // main Draw function
		void DrawHUDElement(ImDrawList* draw_list, ImVec2& pos, ImVec2& size); 
		void DrawEditor();
		void ToggleEditorMode();

		float HeatFillPct();
		ImU32 GetBarColor(float fraction);
		void DrawVerticalBar(ImDrawList* draw_list, ImVec2& pos, ImVec2& size, float fraction);
		void DrawHorizontalBar(ImDrawList* draw_list, ImVec2& pos, ImVec2& size, float fraction);
		bool IsVertical() const ;
		void SetIsVertical(bool set) ;
		ImVec2 GetBarSize(float sizeHorizontalX, float sizeHorizontalY) const;
		
		

	private:
		void InitSettings();
		void SaveAllSettings() const;
		bool _isVertical {true};
		float _barPosX = 600;
		float _barPosY = 45;
		float _barHorSizeX = 200;
		float _barHorSizeY = 20;
		bool _isEditorModeActive = false;
		float _editorPosX = 50;
		float _editorPosY = 500;
		bool _settingsInitialised = false;
		bool _lockpickTimerEnable = true;
		bool _lockpickConsequence = true;
		float _lockpickMinTime = 5.0f;
		float _lockpickMaxTime = 20.0f;
		float _pickpocketMinTime = 5.0f;
		float _pickpocketMaxTime = 20.0f;
		int _minItemValueForRep = 50;
		int _hourlyInfamyDecrease = 2;
		int _minItemValueToIncreaseInfamy = 158;
		bool _dynamicPickpocketEnable = true;
		bool _dynamicLockpickEnable = true;
		bool _showMeter = true;
		//reputation perks:
		bool _goldRushScreen;
		std::string _goldRushNotifText;
		bool _goldRushSoundEnabled;
		float _goldRushShaderDuration;
	};

	struct WndProc
	{
		static LRESULT thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		static inline WNDPROC func;
	};
}
