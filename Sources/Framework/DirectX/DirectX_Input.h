#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <dinputd.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

namespace NDirectX {

	//!	@brief	入力初期化
	HRESULT InitInput(HINSTANCE hInstance, HWND gHWnd);

	//!	@brief	入力終了処理
	void UninitInput(void);

	//!	@brief	入力の更新
	void UpdateInput(void);

	//----------------------------------------------------------
	//!	@brief	キーボードの入力
	//----------------------------------------------------------
	bool GetKeyboardPress(int key);
	bool GetKeyboardTrigger(int key);
	bool GetKeyboardRepeat(int key);
	bool GetKeyboardRelease(int key);

	//----------------------------------------------------------
	//!	@brief	マウスの入力
	//----------------------------------------------------------
	bool GetMouseLeftPress(void);
	bool GetMouseLeftTrigger(void);
	bool GetMouseRightPress(void);
	bool GetMouseRightTrigger(void);
	bool GetMouseCenterPress(void);
	bool GetMouseCenterTrigger(void);
	long GetMouseAxisX(void);			// 移動量
	long GetMouseAxisY(void);
	long GetMouseAxisZ(void);			// ホイールの回転量
	long GetMouseX(void);				
	long GetMouseY(void);

	//----------------------------------------------------------
	//!	@brief	ジョイスティックの入力
	//----------------------------------------------------------

	//!	@brief	ジョイスティックの入力
	long	GetLeftJoystickX();
	long	GetLeftJoystickY();

	long	GetRightJoystickX();
	long	GetRightJoystickY();

	//!	@brief	ジョイスティックの入力(正規化済み)
	float	GetLeftJoystickXNormal();
	float	GetLeftJoystickYNormal();

	float	GetRightJoystickXNormal();
	float	GetRightJoystickYNormal();

	//!	@brief	左ジョイスティックの入力押下状態
	bool GetJoystickUpHold();
	bool GetJoystickDownHold();
	bool GetJoystickRightHold();
	bool GetJoystickLeftHold();

	bool GetJoystickUpTrigger();
	bool GetJoystickDownTrigger();
	bool GetJoystickRightTrigger();
	bool GetJoystickLeftTrigger();

	bool GetJoystickUpRelease();
	bool GetJoystickDownRelease();
	bool GetJoystickRightRelease();
	bool GetJoystickLeftRelease();

	//!	@brief	POVの入力
	bool GetPOVUpHold();
	bool GetPOVRightHold();
	bool GetPOVDownHold();
	bool GetPOVLeftHold();

	bool GetPOVUpTrigger();
	bool GetPOVDownTrigger();
	bool GetPOVRightTrigger();
	bool GetPOVLeftTrigger();

	bool GetPOVUpRelease();
	bool GetPOVDownRelease();
	bool GetPOVRightRelease();
	bool GetPOVLeftRelease();

	//!	@brief	ジョイスティックのボタン入力
	bool GetJoystickButtonHold(int _no);
	bool GetJoystickButtonTrigger(int _no);
	bool GetJoystickButtonRelease(int _no);
}
