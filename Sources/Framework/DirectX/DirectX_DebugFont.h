#pragma once

//!	デバッグフォントを管理する

#include	<d3dx9.h>

namespace NDirectX {

	//!	@brief	デバッグフォントの初期化処理
	void SetupDebugFont(LPDIRECT3DDEVICE9	_lpd3ddevice);

	//!	@brief	デバッグフォントの解放処理
	void TearDownDebugFont();

	//!	@brief	デバッグフォントを用いた描画
	void PrintDebugFont(float _x, float _y, const char* _pStr);

	//!	@brief	フォーマットを指定したデバッグフォント描画
	void PrintDebugFontF(float _x, float _y, const char* _fmt, ...);

	//!	@brief	デバッグフォントの色を指定
	void SetColorDebugFont(float _r, float _g, float _b, float _a = 1);
}
