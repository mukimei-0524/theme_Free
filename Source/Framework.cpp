#include <memory>
#include <sstream>
#include <d3d11.h>

#include "Graphics/GraphicsManager.h"
#include "Effect/EffectManager.h"
#include "Framework.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "Graphics/shader.h"



// コンストラクタ
Framework::Framework(HWND hwnd) : hwnd(hwnd)
{
	// デバイス管理の初期化
	deviceMgr = DeviceManager::instance()->initialize(hwnd);

	// 入力管理の初期化
	inputMgr = InputManager::instance()->initialize(hwnd);

	// オーディオ管理の初期化
	audioMgr = AudioManager::instance()->initialize();

	// グラフィックス管理の初期化
	GraphicsManager::instance()->initialize(deviceMgr->getDevice(), deviceMgr->getDeviceContext());

	// imgui の作成
	imguiRenderer = std::make_unique<ImGuiRenderer>(hwnd, deviceMgr->getDevice());
	
	// エフェクト管理の初期化
	EffectManager::instance()->initialize();

	// シーンの生成と初期化
	SceneManager::instance()->changeScene(new TitleScene);
}

// デストラクタ
Framework::~Framework()
{
	// シーン終了処理
	SceneManager::instance()->clear();
}

bool Framework::initialize()
{
	HRESULT hr{ S_OK };

	// デバイス＆スワップチェーン生成
	{
		UINT create_device_flags{ 0 };
#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
		swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.OutputWindow = hwnd;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Windowed = !FULLSCREEN;
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
			&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
			swap_chain.GetAddressOf(), device.GetAddressOf(), NULL, immediate_context.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// レンダーターゲットビューの生成
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer{};
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// デプスステンシルビューの生成
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	深度ステンシルビュー生成
		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_buffer.Get(),
			&depth_stencil_view_desc,
			depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}

	// サンプラステートの生成
	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
	}

	// 深度ステンシルステートの生成
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// ブレンドステートの生成
	{
		// アルファブレンド
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// ラスタライザーステートの生成
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	//定数バッファの生成
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		{
			buffer_desc.ByteWidth = sizeof(fog_constants);
			hr = device->CreateBuffer(&buffer_desc, nullptr, fog_constant_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
	}	

	//シーン描画用のバッファ生成
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, color_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	レンダーターゲットビュー生成
		hr = device->CreateRenderTargetView(color_buffer.Get(), NULL, scene_render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	シェーダーリソースビュー生成
		hr = device->CreateShaderResourceView(color_buffer.Get(), NULL, scene_shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}

	//描画オブジェクトの読み込み
	{
		// サンプラステート生成
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampler_desc, ramp_sampler_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	//シェーダーの読み込み
	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		
	}

	return true;
}

// 更新処理
void Framework::update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 入力更新処理
	inputMgr->update();

	// シーン更新処理
	SceneManager::instance()->update(elapsedTime);
}

void Framework::imgui(float elapsedTime)
{
	// 時間経過更新
	//timer += elapsedTime;

#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::SliderFloat("hemisphere_weight", &hemisphere_weight, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::ColorEdit3("fog_color", &fog_color.x);
	ImGui::SliderFloat("fog_near", &fog_range.x, 0.1f, +100.0f);
	ImGui::SliderFloat("fog_far", &fog_range.y, 0.1f, +100.0f);

	ImGui::End();
#endif
}

// 描画処理
void Framework::render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 排他処理
	std::lock_guard<std::mutex> lock(DeviceManager::instance()->getMutex());

	ID3D11DeviceContext* dc = deviceMgr->getDeviceContext();

	// IMGUIフレーム開始処理
	imguiRenderer->newFrame();

	// シーン描画処理
	SceneManager::instance()->render();

	// demo
	//ImGui::ShowDemoWindow();

	// IMGUI描画
	imguiRenderer->render(dc);

	// バックバッファに描画した画を画面に表示する。
	deviceMgr->getSwapChain()->Present(syncInterval, 0);
}

bool Framework::uninitialize()
{
	return false;
}

// フレームレート計算
void Framework::calculateFrameStats()
{
	if (++frames, (timer.timeStamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::ostringstream outs;
		outs.precision(6);
		outs << " : FPS : " << fps << " / " << "Frame Time : " << 1000.0f / fps << " (ms)";
		SetWindowTextA(hwnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

// アプリケーションループ
int Framework::run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.tick();
			calculateFrameStats();

			float elapsedTime = syncInterval == 0 ? timer.timeInterval() : syncInterval / 60.0f;
			
			update(elapsedTime);
			render(elapsedTime);
		}
	}
	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (imguiRenderer->handleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
