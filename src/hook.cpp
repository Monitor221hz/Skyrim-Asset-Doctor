#include "hook.h"
#include "gui.h"

//all this code is from po3 because imgui scares me
namespace ImGui::Renderer
{
	struct WndProc
	{
		static LRESULT thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			auto& io = ImGui::GetIO();
			if (uMsg == WM_KILLFOCUS) {
				io.ClearInputCharacters();
				io.ClearInputKeys();
			}

			return func(hWnd, uMsg, wParam, lParam);
		}
		static inline WNDPROC func;
	};

	struct CreateD3DAndSwapChain
	{
		static void thunk()
		{
			func();

			if (const auto renderer = RE::BSGraphics::Renderer::GetSingleton()) {
				const auto swapChain = renderer->data.renderWindows[0].swapChain;
				if (!swapChain) {
					SKSE::log::error("couldn't find swapChain");
					return;
				}

				DXGI_SWAP_CHAIN_DESC desc{};
				if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
					SKSE::log::error("IDXGISwapChain::GetDesc failed.");
					return;
				}

				const auto device = renderer->data.forwarder;
				const auto context = renderer->data.context;

				SKSE::log::info("Initializing ImGui...");

				ImGui::CreateContext();

				auto& io = ImGui::GetIO();
				io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
				io.IniFilename = nullptr;

				if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
					SKSE::log::error("ImGui initialization failed (Win32)");
					return;
				}
				if (!ImGui_ImplDX11_Init(device, context)) {
					SKSE::log::error("ImGui initialization failed (DX11)");
					return;
				}

				// MANAGER(IconFont)->LoadIcons();

				SKSE::log::info("ImGui initialized.");

				initialized.store(true);

				WndProc::func = reinterpret_cast<WNDPROC>(
					SetWindowLongPtrA(
						desc.OutputWindow,
						GWLP_WNDPROC,
						reinterpret_cast<LONG_PTR>(WndProc::thunk)));
				if (!WndProc::func) {
					SKSE::log::error("SetWindowLongPtrA failed!");
				}
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	// DXGIPresentHook
	struct StopTimer
	{
		static void thunk(std::uint32_t a_timer)
		{
			func(a_timer);

			// Skip if Imgui is not loaded
			if (!initialized.load()) {
				return;
			}

			// const auto photoMode = MANAGER(PhotoMode);
			if (!AssetDoctor::Interface::IsEnabled())
			{
				return;
			}
			// if (!photoMode->IsActive() || !photoMode->OnFrameUpdate()) {
			// 	return;
			// }
            

			// MANAGER(IconFont)->LoadFonts();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			{
				// disable windowing
				GImGui->NavWindowingTarget = nullptr;

                AssetDoctor::Interface::Draw();
				// photoMode->Draw();
			}
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	float GetResolutionScale()
	{
		return DisplayTweaks::borderlessUpscale ? DisplayTweaks::resolutionScale : 1.0f;
	}

	void LoadSettings(const CSimpleIniA& a_ini)
	{
		DisplayTweaks::resolutionScale = a_ini.GetDoubleValue("Render", "ResolutionScale", DisplayTweaks::resolutionScale);
		DisplayTweaks::borderlessUpscale = a_ini.GetBoolValue("Render", "BorderlessUpscale", DisplayTweaks::borderlessUpscale);
	}

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(75595, 77226), REL::Relocate(0x9, 0x275) };  // BSGraphics::InitD3D
		rstl::write_thunk_call<CreateD3DAndSwapChain>(target.address());

		REL::Relocation<std::uintptr_t> target2{ RELOCATION_ID(75461, 77246), 0x9 };  // BSGraphics::Renderer::End
		rstl::write_thunk_call<StopTimer>(target2.address());
	}
}