#pragma once

#include	<d3dx9.h>
#include	<d3d9.h>

namespace NDirectX {

	// 初期化処理
	void Setup(HINSTANCE _hInstance, HWND _hWnd, UINT _width, UINT _height);

	// メモリ解放
	void TearDown();



	//!	@brief	デバイスを取得
	LPDIRECT3DDEVICE9			GetD3DDevice();

	//!	@brief	デバイスコンテキストを取得
	D3DPRESENT_PARAMETERS		GetD3DParam();



	//!	@brief	バックバッファを初期状態にする
	void ClearBackBuffer(const float4& _color);

	//!	@brief	画面を切り替える
	void Present();



	//!	@brief	固定パイプラインでの描画を開始
	void BeginScene();

	//!	@brief	固定パイプラインの描画を終了
	void EndScene();
}

