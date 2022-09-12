#include <Windows.h>
#include <ShlObj.h>
#include <cinttypes>
#include <filesystem>
#include <string>
#include <mutex>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "vmt_smart_hook.hpp"

#include "CheatManager.hpp"
#include "Hooks.hpp"
#include "Memory.hpp"
#include "SDK/AIBaseCommon.hpp"
#include "SDK/GameState.hpp"

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WINAPI wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (::ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
		cheatManager.gui->is_open = !cheatManager.gui->is_open;
		if (!cheatManager.gui->is_open)
			cheatManager.config->save();
	}

	return ::CallWindowProcW(originalWndProc, window, msg, wParam, lParam);
}

std::once_flag init_device;
std::unique_ptr<::vmt_smart_hook> d3d_device_vmt{ nullptr };
std::unique_ptr<::vmt_smart_hook> swap_chain_vmt{ nullptr };

static const ImWchar ranges[] = {
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x0100, 0x024F, // Latin Extended-A + Latin Extended-B
	0x0300, 0x03FF, // Combining Diacritical Marks + Greek/Coptic
	0x0400, 0x044F, // Cyrillic
	0x0600, 0x06FF, // Arabic
	0x0E00, 0x0E7F, // Thai
	0,
};

static ImWchar* getFontGlyphRangesKr() noexcept
{
	static ImVector<ImWchar> rangesKR;
	if (rangesKR.empty()) {
		ImFontGlyphRangesBuilder builder;
		auto& io{ ImGui::GetIO() };
		builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
		builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
		builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
		builder.BuildRanges(&rangesKR);
	}
	return rangesKR.Data;
}

namespace d3d_vtable {
	ID3D11Device* d3d11_device{ nullptr };
	ID3D11DeviceContext* d3d11_device_context{ nullptr };
	ID3D11RenderTargetView* main_render_target_view{ nullptr };
	IDXGISwapChain* p_swap_chain{ nullptr };

	static void WINAPI create_render_target() noexcept
	{
		ID3D11Texture2D* back_buffer{ nullptr };
		p_swap_chain->GetBuffer(0u, IID_PPV_ARGS(&back_buffer));

		if (back_buffer) {
			d3d11_device->CreateRenderTargetView(back_buffer, NULL, &main_render_target_view);
			back_buffer->Release();
		}
	}

	static void init_imgui(void* device, bool is_d3d11 = false) noexcept
	{
		cheatManager.database->update();
		ImGui::CreateContext();
		auto& style{ ImGui::GetStyle() };

		style.WindowPadding = ImVec2(6.0f, 6.0f);
		style.FramePadding = ImVec2(6.0f, 4.0f);
		style.ItemSpacing = ImVec2(6.0f, 4.0f);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.ScrollbarSize = 12.0f;

		style.WindowBorderSize = 0.5f;
		style.ChildBorderSize = 0.5f;
		style.PopupBorderSize = 0.5f;
		style.FrameBorderSize = 0;

		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.PopupRounding = 0.0f;

		auto colors{ style.Colors };

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		auto& io{ ImGui::GetIO() };
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		if (PWSTR pathToFonts; SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
			const std::filesystem::path path{ pathToFonts };
			::CoTaskMemFree(pathToFonts);
			ImFontConfig cfg;
			cfg.SizePixels = 15.0f;
			io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, &cfg, ranges);
		}

		ImGui_ImplWin32_Init(cheatManager.memory->getRiotWindow());

		if (is_d3d11) {
			p_swap_chain = reinterpret_cast<IDXGISwapChain*>(device);
			p_swap_chain->GetDevice(__uuidof(d3d11_device), reinterpret_cast<void**>(&(d3d11_device)));
			d3d11_device->GetImmediateContext(&d3d11_device_context);
			create_render_target();
			::ImGui_ImplDX11_Init(d3d11_device, d3d11_device_context);
			::ImGui_ImplDX11_CreateDeviceObjects();
		} else
			::ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(device));

		originalWndProc = WNDPROC(::SetWindowLongW(cheatManager.memory->getRiotWindow(), GWLP_WNDPROC, LONG_PTR(&wndProc)));
	}

	static void render(void* device, bool is_d3d11 = false) noexcept
	{
		static const auto client{ cheatManager.memory->client };
		if (client && client->gameState == GGameState_s::Running && cheatManager.gui->is_open) {
			if (is_d3d11)
				::ImGui_ImplDX11_NewFrame();
			else
				::ImGui_ImplDX9_NewFrame();
			::ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			cheatManager.gui->render();
			ImGui::EndFrame();
			ImGui::Render();

			if (is_d3d11) {
				d3d11_device_context->OMSetRenderTargets(1, &main_render_target_view, NULL);
				::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			} else {
				unsigned long colorwrite, srgbwrite;
				const auto dvc{ reinterpret_cast<IDirect3DDevice9*>(device) };
				dvc->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
				dvc->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);
				dvc->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
				dvc->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
				::ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				dvc->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
				dvc->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
			}
		}
	}

	struct dxgi_present {
		static long WINAPI hooked(IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags) noexcept
		{
			std::call_once(init_device, [&]() { init_imgui(p_swap_chain, true); });
			render(p_swap_chain, true);
			return m_original(p_swap_chain, sync_interval, flags);
		}
		static decltype(&hooked) m_original;
	};
	decltype(dxgi_present::m_original) dxgi_present::m_original;

	struct dxgi_resize_buffers {
		static long WINAPI hooked(IDXGISwapChain* p_swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) noexcept
		{
			if (main_render_target_view) { main_render_target_view->Release(); main_render_target_view = nullptr; }
			const auto hr{ m_original(p_swap_chain, buffer_count, width, height, new_format, swap_chain_flags) };
			create_render_target();
			return hr;
		}
		static decltype(&hooked) m_original;
	};
	decltype(dxgi_resize_buffers::m_original) dxgi_resize_buffers::m_original;

	struct end_scene {
		static long WINAPI hooked(IDirect3DDevice9* p_device) noexcept
		{
			std::call_once(init_device, [&]() { init_imgui(p_device); });
			render(p_device);
			return m_original(p_device);
		}
		static decltype(&hooked) m_original;
	};
	decltype(end_scene::m_original) end_scene::m_original;

	struct reset {
		static long WINAPI hooked(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* parametrs) noexcept
		{
			::ImGui_ImplDX9_InvalidateDeviceObjects();
			const auto hr{ m_original(device, parametrs) };
			if (hr >= 0)
				::ImGui_ImplDX9_CreateDeviceObjects();
			return hr;
		}
		static decltype(&hooked) m_original;
	};
	decltype(reset::m_original) reset::m_original;
};

void Hooks::install() const noexcept
{
	if (cheatManager.memory->d3dDevice) {
		d3d_device_vmt = std::make_unique<::vmt_smart_hook>(cheatManager.memory->d3dDevice);
		d3d_device_vmt->apply_hook<d3d_vtable::end_scene>(42);
		d3d_device_vmt->apply_hook<d3d_vtable::reset>(16);
	} else if (cheatManager.memory->swapChain) {
		swap_chain_vmt = std::make_unique<::vmt_smart_hook>(cheatManager.memory->swapChain);
		swap_chain_vmt->apply_hook<d3d_vtable::dxgi_present>(8);
		swap_chain_vmt->apply_hook<d3d_vtable::dxgi_resize_buffers>(13);
	}
}

void Hooks::uninstall() const noexcept
{
	::SetWindowLongW(cheatManager.memory->getRiotWindow(), GWLP_WNDPROC, LONG_PTR(originalWndProc));

	if (d3d_device_vmt)
		d3d_device_vmt->unhook();
	if (swap_chain_vmt)
		swap_chain_vmt->unhook();

	cheatManager.cheatState = false;
}
