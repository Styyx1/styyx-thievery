#include "infamyHUD.h"
#include "formloader.h"
#include <dxgi.h>
import inputhandler;
import config;
import heat;
import generalStealing;
import lockpicking;
import Pickpocket;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 



namespace InfamyHUD {

	static InfamyHUDDisplay* g_infamyHUDMenu = nullptr;

	void Renderer::Present(uint32_t a1)
	{
		if (ImGui::GetCurrentContext()) {
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			if(!g_infamyHUDMenu)
				g_infamyHUDMenu = InfamyHUDDisplay::GetSingleton();

			g_infamyHUDMenu->Draw();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		else {
			REX::DEBUG("no imgui context found");
		}
		_Hook11(a1);
	}

	void SwapChainHook::RendererInit()
	{
		_Hook12();  // call original
		if (!bInitialized) {
			const auto settings = Config::Settings::GetSingleton();
			auto renderer = RE::BSGraphics::Renderer::GetSingleton();
			if(!g_infamyHUDMenu)
				g_infamyHUDMenu = InfamyHUDDisplay::GetSingleton();
			if (!renderer) {
				REX::ERROR("Renderer not found!");
				return;
			}

			auto swapChain = (IDXGISwapChain*)renderer->data.renderWindows[0].swapChain;
			if (!swapChain) {
				REX::ERROR("SwapChain not found!");
				return;
			}

			DXGI_SWAP_CHAIN_DESC desc{};
			if (FAILED(swapChain->GetDesc(&desc))) {
				REX::ERROR("SwapChain::GetDesc failed");
				return;
			}

			const auto device = reinterpret_cast<ID3D11Device*>(renderer->data.forwarder);
			const auto context = reinterpret_cast<ID3D11DeviceContext*>(renderer->data.context);

			ImGui::CreateContext();

			if (!bStyleApplied) {
				ApplyHUDStyle();
				REX::DEBUG("applied HUD style");
				bStyleApplied = true;
			}

			auto& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableSetMousePos;
			io.IniFilename = nullptr;

			if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
				REX::ERROR("ImGui Win32 init failed");
				return;
			}
			if (!ImGui_ImplDX11_Init(device, context)) {
				REX::ERROR("ImGui DX11 init failed");
				return;
			}
			REX::INFO("ImGui initialized.");
			bInitialized = true;

			WndProc::func = reinterpret_cast<WNDPROC>(
				SetWindowLongPtrA(
					desc.OutputWindow,
					GWLP_WNDPROC,
					reinterpret_cast<LONG_PTR>(WndProc::thunk)));
			if (!WndProc::func) {
				REX::ERROR("SetWindowLongPtrA failed!");
			}
		}
	}

	void SwapChainHook::ApplyHUDStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 5.0f;
		style.WindowBorderSize = 1.0f;
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);  
		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.9f, 0.7f, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 0.9f, 0.7f, 0.0f);
		style.WindowPadding = ImVec2(8, 6);
		style.FramePadding = ImVec2(4, 2);
		style.ItemSpacing = ImVec2(6, 4);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.AntiAliasedFill = true;
		style.AntiAliasedLines = true;
	}

	bool InfamyHUDDisplay::IsEditorModeActive() const
	{
		return _isEditorModeActive;
	}

	bool InfamyHUDDisplay::FocusRegained()
	{
		return false;
	}

	void InfamyHUDDisplay::SetEditorMode(bool enable)
	{
		_isEditorModeActive = enable;
	}

	bool InfamyHUDDisplay::FocusLost()
	{
		return false;
	}

	void InfamyHUDDisplay::DrawVerticalBar(ImDrawList* draw_list, ImVec2& pos, ImVec2& size, float fraction) {
		draw_list->AddRectFilled(ImVec2(pos.x - 5, pos.y - 5), ImVec2(pos.x + size.x + 5, pos.y + size.y + 5), IM_COL32(30, 30, 30, 180), 0); // Background 
		if (fraction > 0.0f) {
			float fillTopY = pos.y + size.y * (1.0f - fraction);
			draw_list->AddRectFilled(
				ImVec2(pos.x, fillTopY),
				ImVec2(pos.x + size.x, pos.y + size.y),
				GetBarColor(fraction),
				0.0f
			);
		}		
		draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(235,235,235,255), 0, 3);
	}

	void InfamyHUDDisplay::DrawHorizontalBar(ImDrawList* draw_list, ImVec2& pos, ImVec2& size, float fraction)
	{
		draw_list->AddRectFilled(ImVec2(pos.x - 5, pos.y - 5), ImVec2(pos.x + size.x + 5, pos.y + size.y + 5), IM_COL32(30, 30, 30, 180), 0); // Background 
		if (fraction > 0.0f) {
				draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x * fraction, pos.y + size.y),GetBarColor(fraction),0.0f);
		}		
		draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(235,235,235,255), 0, 3);
	}

	bool InfamyHUDDisplay::IsVertical() const
	{
		return _isVertical;
	}

	void InfamyHUDDisplay::SetIsVertical(bool set)
	{ 
		_isVertical = set; 
	}

	ImVec2 InfamyHUDDisplay::GetBarSize(float sizeHorizontalX, float sizeHorizontalY) const
	{
		return IsVertical() ? ImVec2(sizeHorizontalY, sizeHorizontalX) : ImVec2(sizeHorizontalX, sizeHorizontalY);		
	}

	void InfamyHUDDisplay::ToggleEditorMode()
	{
		_isEditorModeActive = !_isEditorModeActive;
		auto cmp = RE::ControlMap::GetSingleton();
		if (cmp) {
			cmp->ignoreKeyboardMouse = _isEditorModeActive;
		}
		if (!_isEditorModeActive) {
			SaveAllSettings();
			REX::INFO("Editor settings saved.");
		}			
	}

	void InfamyHUDDisplay::InitSettings()
	{
		if (!_settingsInitialised) {
			const auto& config = Config::Settings::GetSingleton();
			_barPosX = config->bar_pos_x.GetValue();
			_barPosY = config->bar_pos_y.GetValue();
			_barHorSizeX = config->bar_size_length.GetValue();
			_barHorSizeY = config->bar_size_width.GetValue();
			_isVertical = config->is_bar_vertical.GetValue();
			_editorPosX = config->editor_pos_x.GetValue();
			_editorPosY = config->editor_pos_y.GetValue();
			//Thievery
			PickpocketTimer::_PTimerEnabled = (config->enable_pickpocket_timer.GetValue());
			LockpickingMenu::_timerEnabled = config->enable_lockpick_timer.GetValue();
			_pickpocketMinTime = config->pickpocket_min_time.GetValue();
			_pickpocketMaxTime = config->pickpocket_max_time.GetValue();
			_lockpickMaxTime = config->lockpicking_max_time.GetValue();
			_lockpickMinTime = config->lockpicking_min_time.GetValue();
			_dynamicPickpocketEnable = config->enable_dyn_pickpocket_cap.GetValue();
			//Infamy/Reputation
			_minItemValueForRep = config->reputation_min_item_value.GetValue();
			_minItemValueToIncreaseInfamy = config->fence_value_heat_threshold.GetValue();
			_hourlyInfamyDecrease = config->hourly_heat_decrease.GetValue();



			_settingsInitialised = true;

		}
	}

	void InfamyHUDDisplay::SaveAllSettings() const
	{
		const auto& config = Config::Settings::GetSingleton();

		config->editor_pos_x.SetValue(_editorPosX);
		config->editor_pos_y.SetValue(_editorPosY);
		config->is_bar_vertical.SetValue(_isVertical);
		config->bar_pos_x.SetValue(_barPosX);
		config->bar_pos_y.SetValue(_barPosY);
		config->bar_size_length.SetValue(_barHorSizeX);
		config->bar_size_width.SetValue(_barHorSizeY);
		//lockpicking
		config->enable_lockpick_timer.SetValue(LockpickingMenu::_timerEnabled);
		config->remove_lockpick_on_timeout.SetValue(_lockpickConsequence);
		config->lockpicking_min_time.SetValue(_lockpickMinTime);
		config->lockpicking_max_time.SetValue(_lockpickMaxTime);
		//pickpocket
		config->enable_pickpocket_timer.SetValue(PickpocketTimer::_PTimerEnabled);
		config->pickpocket_min_time.SetValue(_pickpocketMinTime);
		config->pickpocket_max_time.SetValue(_pickpocketMaxTime);
		config->enable_dyn_pickpocket_cap.SetValue(_dynamicPickpocketEnable);
		//Infamy/Reputation
		config->reputation_min_item_value.SetValue(_minItemValueForRep);
		config->fence_value_heat_threshold.SetValue(_minItemValueToIncreaseInfamy);
		config->hourly_heat_decrease.SetValue(_hourlyInfamyDecrease);

		config->Save();
	}

	void InfamyHUDDisplay::DrawHUDElement(ImDrawList* draw_list, ImVec2& pos, ImVec2& size)
	{
		float fraction = HeatFillPct();
		if (IsVertical())
			DrawVerticalBar(draw_list, pos, size, fraction);
		else
			DrawHorizontalBar(draw_list, pos, size, fraction);
		// move cursor to avoid overlapping with next element
		ImGui::Dummy(size);
	}

	void InfamyHUDDisplay::DrawEditor()
	{
		auto& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = false;
		ImGui::SetNextFrameWantCaptureMouse(true);
		ImGui::SetNextFrameWantCaptureKeyboard(true);
		if(!ImGui::IsWindowFocused())
			ImGui::SetNextWindowFocus();

		io.MouseDrawCursor = true;
		io.AddMouseButtonEvent(0, (GetKeyState(VK_LBUTTON) & 0x80) != 0);

		

		ImGui::SetNextWindowPos(ImVec2(_editorPosX, _editorPosY), ImGuiCond_Always);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::Begin("StyyxThieveryEditor", nullptr, windowFlags);

		// move only if this window is hovered and mouse is down
		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
			_editorPosX += io.MouseDelta.x;
			_editorPosY += io.MouseDelta.y;
		}
		

		ImVec2 pos = ImGui::GetCursorScreenPos();
		auto draw_list = ImGui::GetWindowDrawList();
		//title
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
		ImGui::PushItemWidth(300.0f);
		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.1f, 1.0f), "Styyx Thievery Editor");
		ImGui::Separator();
		//Infamy Bar
		ImGui::Checkbox("Infamy Bar: Vertical", &_isVertical);
		ImGui::SliderFloat("Infamy Bar: Length", &_barHorSizeX, 50.0f, 400.0f);
		ImGui::SliderFloat("infamy Bar: Width", &_barHorSizeY, 10.0f, 50.0f);
		ImGui::Separator();
		ImGui::Text("Thievery Settings");
		//Lockpicking
		ImGui::SeparatorText("Lockpicking");
		ImGui::Checkbox("Lockpicking: Timer Toggle", &LockpickingMenu::_timerEnabled);
		ImGui::SameLine();
		ImGui::Checkbox("Lockpicking: Remove Lockpick on Fail", &_lockpickConsequence);
		ImGui::SliderFloat("Lockpicking: Minimum Time", &_lockpickMinTime, 5.0f, 30.0f);
		ImGui::SliderFloat("Lockpicking: Maximum Time", &_lockpickMaxTime, 31.0f, 120.0f);
		//Pickpocketing
		ImGui::SeparatorText("Pickpocketing");
		ImGui::Checkbox("Pickpocket: Timer Toggle", &PickpocketTimer::_PTimerEnabled);
		ImGui::SameLine();
		if (ImGui::Checkbox("Pickpocket: Toggle Dynamic Pickpocketing", &_dynamicPickpocketEnable)) {
			if(!_dynamicPickpocketEnable)
				PickpocketTimer::ResetPickpocketChance();
			else {
				PickpocketTimer::InitPickpocketCapChange();
			}
		}
		ImGui::SliderFloat("Pickpocket: Minimum Time", &_pickpocketMinTime, 5.0f, 30.0f);
		ImGui::SliderFloat("Pickpocket: Maximum Time", &_pickpocketMaxTime, 31.0f, 120.0f);
		//Reputation
		ImGui::SeparatorText("Reputation/Infamy");
		ImGui::SliderInt("Reputation: Min Item Value to gain reputation", &_minItemValueForRep, 30, 400);
		ImGui::SliderInt("Infamy: Hourly Infamy Decrease", &_hourlyInfamyDecrease, 1, 4);
		ImGui::SliderInt("Infamy: Item Value threshold to increase Infamy",&_minItemValueToIncreaseInfamy, 100, 600);
		//end
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void InfamyHUDDisplay::Draw()
	{	
		InitSettings();
		auto& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = !_isEditorModeActive;
		//_isEditorModeActive = true;
		if (IsEditorModeActive()) {
			io.MouseDrawCursor = true;
			io.AddMouseButtonEvent(0, (GetKeyState(VK_LBUTTON) & 0x80) != 0);
		}
		else {
			io.MouseDrawCursor = false;
		}

		ImGui::SetNextWindowPos(ImVec2(_barPosX, _barPosY), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f); 		

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar;
		if (!_isEditorModeActive) {
			windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
		} else {
			windowFlags |=  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
		}
		
		ImGui::Begin("InfamyHUD", nullptr, windowFlags);

		// move only if this window is hovered and mouse is down
		if (_isEditorModeActive && ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
			_barPosX += io.MouseDelta.x;
			_barPosY += io.MouseDelta.y;
		}

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size = GetBarSize(_barHorSizeX, _barHorSizeY);
		auto draw_list = ImGui::GetWindowDrawList();
		DrawHUDElement(draw_list, pos, size);
		ImGui::End();

		if (IsEditorModeActive()) {
			DrawEditor();
		}
	}

	float InfamyHUDDisplay::HeatFillPct()
	{
		float ret_val = 0.5f;
		const auto& formL = FormLoader::Loader::GetSingleton();
		if (formL->player_heat_system_global) {
			ret_val = std::clamp(HeatSystem::GetHeatValue() / 100.0f, 0.0f, 100.0f);
		}
		return ret_val;
	}

	ImU32 InfamyHUDDisplay::GetBarColor(float fraction)
	{
		ImVec4 color;

		if (fraction <= 0.5f) {
			// Blue -> Yellow
			float t = fraction / 0.5f;
			color.x = (1.0f - t) * 0.0f + t * 1.0f; // R
			color.y = (1.0f - t) * 0.0f + t * 1.0f; // G
			color.z = (1.0f - t) * 1.0f + t * 0.0f; // B
		}
		else if (fraction <= 0.7f) {
			// Yellow -> Orange
			float t = (fraction - 0.5f) / 0.2f;
			color.x = (1.0f - t) * 1.0f + t * 1.0f; // R
			color.y = (1.0f - t) * 1.0f + t * 0.5f; // G
			color.z = (1.0f - t) * 0.0f + t * 0.0f; // B
		}
		else if (fraction <= 0.9f) {
			// Orange -> Red
			float t = (fraction - 0.7f) / 0.2f;
			color.x = 1.0f;
			color.y = (1.0f - t) * 0.5f + t * 0.0f;
			color.z = 0.0f;
		}
		else {
			// Max Red
			color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		color.w = 0.9f; // Alpha
		return ImGui::ColorConvertFloat4ToU32(color);
	}

	LRESULT WndProc::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(!g_infamyHUDMenu)
			g_infamyHUDMenu = InfamyHUDDisplay::GetSingleton();
		if (g_infamyHUDMenu->IsEditorModeActive()) {
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);				
			return true;				

		}
		if (uMsg == WM_SETFOCUS) {
			g_infamyHUDMenu->FocusRegained();
			const auto& inputHandler = InputManager::GetSingleton();
			inputHandler->SetEditorKey();
		}

		if (uMsg == WM_KILLFOCUS) {
			g_infamyHUDMenu->SetEditorMode(false);
			g_infamyHUDMenu->FocusLost();
		}

		return func(hWnd, uMsg, wParam, lParam);
	}
}