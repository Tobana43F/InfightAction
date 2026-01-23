#pragma once

//!	@brief	入力の初期化、更新などの基本的な関数を定義する
//!			- すべての入力の更新などの管理
//!			- キーボードの入力

#include	"Windows\Windows_Include.h"
#include	"DirectX\DirectX_Input.h"
#include	"Input_Def.h"

namespace NInput {

	//!	@brief	入力の初期化
	bool Setup(HINSTANCE _hInstance, HWND _hWnd);

	//!	@brief	入力の解放処理
	void TearDown();

	//!	@brief	更新処理
	void Update();

	//!	@brief		キーの状態を取得
	//!	@param[in]	_key	DirectXの定義(DIK_)を引数にとる
	bool GetKeyPress(int _key);
	bool GetKeyDown(int _key);
	bool GetKeyUp(int _key);
	bool GetKeyRepeat(int _key);
}