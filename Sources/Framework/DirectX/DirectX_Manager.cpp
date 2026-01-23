#include	"DirectX_Manager.h"

#include	"DirectX\DirectX_Util.h"
#include	"System\System_Error.h"

#pragma comment(lib, "d3d9.lib")			// リンク対象ライブラリにd3d9.libを追加
#pragma comment(lib, "d3dx9.lib")			// リンク対象ライブラリにd3dx9.libを追加

namespace {
	LPDIRECT3D9					m_lpd3d;			// DIRECT3D9オブジェクト
	LPDIRECT3DDEVICE9			m_lpd3ddevice;		// DIRECT3DDEVICE8オブジェクト
	D3DPRESENT_PARAMETERS		m_d3dpp;			// プレゼンテーションパラメータ
	D3DDISPLAYMODE				m_disp;				// ディスプレイモード
	int							m_adapter;			// ディスプレイアダプタ番号
	int							m_width;			// バックバッファＸサイズ
	int							m_height;			// バックバッファＹサイズ
}

namespace NDirectX {

	void Setup(HINSTANCE _hInstance, HWND _hWnd, UINT _width, UINT _height)
	{
		HRESULT	hr = E_FAIL;

		// Direct3D9インターフェース取得
		m_lpd3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_lpd3d == nullptr) {
			ErrorEx("DirectX9のインターフェースの取得に失敗しました。");
		}

		// アダプタの現在のディスプレイモードを取得する
		hr = m_lpd3d->GetAdapterDisplayMode(m_adapter, &m_disp);
		if (FAILED(hr)) {
			ErrorEx("ディスプレイモードの取得に失敗しました。");
		}

		// パラメーターを設定
		memset(&m_d3dpp, 0, sizeof(m_d3dpp));			// ゼロクリア
		m_d3dpp.BackBufferFormat = m_disp.Format;		// 現在のビット深度
		m_d3dpp.BackBufferWidth = _width;				// バックバッファの幅をセット
		m_d3dpp.BackBufferHeight = _height;				// バックバッファの高さをセット
		m_d3dpp.BackBufferCount = 1;					// バックバッファの数
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// バックバッファをロック可能にする
		m_d3dpp.Windowed = TRUE;	// ウインドウモード

		// Ｚバッファの自動作成（ビット深度24）
		m_d3dpp.EnableAutoDepthStencil = 1;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// VSYNCを待たない

		// デバイス作成
		hr = m_lpd3d->CreateDevice(m_adapter,
			D3DDEVTYPE_HAL,
			_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&m_d3dpp,
			&m_lpd3ddevice);

		if (FAILED(hr)) {
			hr = m_lpd3d->CreateDevice(m_adapter,
				D3DDEVTYPE_HAL,
				_hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&m_d3dpp,
				&m_lpd3ddevice);

			if (FAILED(hr)) {
				hr = m_lpd3d->CreateDevice(m_adapter,
					D3DDEVTYPE_REF,
					_hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
					&m_d3dpp,
					&m_lpd3ddevice);

				if (FAILED(hr)) {
					ErrorEx("デバイスの作成に失敗しました。");
				}
			}
		}

		m_width = _width;
		m_height = _height;
	}

	void TearDown()
	{
		ReleaseSafe(m_lpd3ddevice);
		ReleaseSafe(m_lpd3d);
	}



	LPDIRECT3DDEVICE9			GetD3DDevice()
	{
		return m_lpd3ddevice;
	}

	D3DPRESENT_PARAMETERS		GetD3DParam()
	{
		return m_d3dpp;
	}



	void ClearBackBuffer(const float4& _color)
	{
		m_lpd3ddevice->Clear(
			0,
			nullptr,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			ToD3DColor(_color.x, _color.y, _color.z, _color.w),
			1.0f,
			0);
	}

	void Present()
	{
		HRESULT hr = m_lpd3ddevice->Present(nullptr, nullptr, nullptr, nullptr);	// バックバッファからプライマリバッファへ転送
		if (FAILED(hr)) {
			m_lpd3ddevice->Reset(&m_d3dpp);
		}
	}



	void BeginScene()
	{
		HRESULT hr = E_FAIL;
		hr = m_lpd3ddevice->BeginScene();
		if (FAILED(hr))
			ErrorEx("BeginSceneに失敗");
	}

	void EndScene()
	{
		HRESULT hr = E_FAIL;
		hr = m_lpd3ddevice->EndScene();
		if (FAILED(hr))
			ErrorEx("EndSceneに失敗");
	}
}
