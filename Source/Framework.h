#pragma once

#include <windows.h>
#include <sstream>
#include "misc.h"

#include "high_resolution_timer.h"
#include "Graphics/ImGuiRenderer.h"
#include "DeviceManager.h"
#include "Input/InputManager.h"
#include "Audio/AudioManager.h"

#include <d3d11.h>
#include <wrl.h>

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };

class GameScene;

class Framework
{
public:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	struct scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection;
		DirectX::XMFLOAT4 options;	//	xy : マウスの座標値, z : タイマー, w : フラグ
		DirectX::XMFLOAT4 camera_position;
	};

	//scene_constants scene_constants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;
	//float timer{ 0.0f };
	bool flag{ false };

	//フォグ
	struct fog_constants
	{
		DirectX::XMFLOAT4 fog_color;
		DirectX::XMFLOAT4 fog_range;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> fog_constant_buffer;
	DirectX::XMFLOAT4 fog_color{ 0.2f, 0.2f, 0.2f, 1.0f }; // クリアカラー
	DirectX::XMFLOAT4 fog_range{ 0.1f, 100.0f, 0, 0 };

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> scene_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scene_shader_resource_view;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> ramp_sampler_state;

public:
	Framework(HWND hwnd);
	~Framework();

private:
	bool initialize();
	void update(float elapsedTime/*Elapsed seconds from last frame*/);
	void imgui(float elapsedTime);
	void render(float elapsedTime/*Elapsed seconds from last frame*/);
	bool uninitialize();

	void calculateFrameStats();

public:
	int run();
	LRESULT CALLBACK handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	const HWND				hwnd;
	
	high_resolution_timer	timer;
	unsigned int frames = 0 ;
	float elapsed_time = 0.0f;

	DeviceManager* deviceMgr;
	InputManager* inputMgr;
	AudioManager* audioMgr;

	std::unique_ptr<ImGuiRenderer>	imguiRenderer;

	const int						syncInterval = 1;		// 垂直同期間隔設定
};