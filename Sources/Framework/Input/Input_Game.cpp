#include	"Input_Common.h"
#include	"Input_Game.h"

//!	@brief	Release時にキーボード入力を使用するかどうかを判断するマクロ
#define	_USE_KEYBOARD_GAME_INPUT

namespace {
	//!	@brief	ゲームで使用するキーボードの定義
	enum EGameKey {
		GK_UP = DIK_W,
		GK_DOWN = DIK_S,
		GK_RIGHT = DIK_D,
		GK_LEFT = DIK_A,

		GK_LEFT_ANALOG_UP = DIK_W,
		GK_LEFT_ANALOG_DOWN = DIK_S,
		GK_LEFT_ANALOG_RIGHT = DIK_D,
		GK_LEFT_ANALOG_LEFT = DIK_A,

		GK_RIGHT_ANALOG_UP = DIK_I,
		GK_RIGHT_ANALOG_DOWN = DIK_K,
		GK_RIGHT_ANALOG_RIGHT = DIK_L,
		GK_RIGHT_ANALOG_LEFT = DIK_J,

		GK_A = DIK_F,
		GK_B = DIK_G,
		GK_X = DIK_R,
		GK_Y = DIK_T,

		GK_LB = DIK_E,
		GK_RB = DIK_Y,

		GK_L3 = DIK_X,
		GK_R3 = DIK_M,

		GK_START = DIK_E,
		GK_BACK = DIK_Q,
	};


	const float VIRTUAL_ANALOG_MAX = 1.0f;		// 仮想アナログ入力の最大値
	const float VIRTUAL_ANALOG_MIN = 0.25f;		// 仮想アナログ入力の最小値
	const float VIRTUAL_ANALOG_ACC = 0.125f;	// 仮想アナログ入力の加速度
	const float VIRTUAL_ANALOG_DEC_RATIO = 0.5f;		// 仮想アナログ入力の減速割合
	float		g_virtualLeftAnalogY;
	float		g_virtualLeftAnalogX;
	float		g_virtualRightAnalogY;
	float		g_virtualRightAnalogX;

	//!	@brief	左の仮想アナログ入力を更新
	void UpdateVirtualAnalogLeft();

	//!	@brief	右の仮想アナログ入力を更新
	void UpdateVirtualAnalogRight();
}

namespace NInput {

	void UpdateVirtualAnalog()
	{
	#ifdef	_USE_KEYBOARD_GAME_INPUT
		UpdateVirtualAnalogLeft();
		UpdateVirtualAnalogRight();
	#endif
	}

	//==========================================================
	// 汎用方向入力
	//==========================================================

	// Hold
	bool GetUpHold()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyPress(GK_UP);
	#endif
		sts |= NDirectX::GetJoystickUpHold();
		sts |= NDirectX::GetPOVUpHold();
		return sts;
	}

	bool GetDownHold()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyPress(GK_DOWN);
	#endif
		sts |= NDirectX::GetJoystickDownHold();
		sts |= NDirectX::GetPOVDownHold();
		return sts;
	}

	bool GetRightHold()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyPress(GK_RIGHT);
	#endif
		sts |= NDirectX::GetJoystickRightHold();
		sts |= NDirectX::GetPOVRightHold();
		return sts;
	}

	bool GetLeftHold()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyPress(GK_LEFT);
	#endif
		sts |= NDirectX::GetJoystickLeftHold();
		sts |= NDirectX::GetPOVLeftHold();
		return sts;
	}

	// Trigger
	bool GetUpTrigger()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_UP);
	#endif
		sts |= NDirectX::GetJoystickUpTrigger();
		sts |= NDirectX::GetPOVUpTrigger();
		return sts;
	}

	bool GetDownTrigger()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_DOWN);
	#endif
		sts |= NDirectX::GetJoystickDownTrigger();
		sts |= NDirectX::GetPOVDownTrigger();
		return sts;
	}

	bool GetRightTrigger()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_RIGHT);
	#endif
		sts |= NDirectX::GetJoystickRightTrigger();
		sts |= NDirectX::GetPOVRightTrigger();
		return sts;
	}

	bool GetLeftTrigger()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_LEFT);
	#endif
		sts |= NDirectX::GetJoystickLeftTrigger();
		sts |= NDirectX::GetPOVLeftTrigger();
		return sts;
	}

	//==========================================================
	// アナログ入力
	//==========================================================

	float	GetLeftAnalogX()
	{
		float analog = 0;
		analog = NDirectX::GetLeftJoystickXNormal();

	#ifdef _USE_KEYBOARD_GAME_INPUT
		float analogVirtual = 0;
		analogVirtual = g_virtualLeftAnalogX;
		if (abs(analog) < FLT_EPSILON) {
			analog = analogVirtual;
		}
	#endif

		if (analog > 1) { analog = 1; }
		if (analog < -1) { analog = -1; }
		return analog;
	}

	float	GetLeftAnalogY()
	{
		float analog = 0;
		analog = NDirectX::GetLeftJoystickYNormal();

	#ifdef _USE_KEYBOARD_GAME_INPUT
		float analogVirtual = 0;
		analogVirtual = g_virtualLeftAnalogY;
		if (abs(analog) < FLT_EPSILON) {
			analog = analogVirtual;
		}
	#endif

		if (analog > 1) { analog = 1; }
		if (analog < -1) { analog = -1; }
		return analog;
	}

	float	GetRightAnalogX()
	{
		float analog = 0;
		analog = NDirectX::GetRightJoystickXNormal();

	#ifdef _USE_KEYBOARD_GAME_INPUT
		float analogVirtual = 0;
		analogVirtual = g_virtualRightAnalogX;
		if (abs(analog) < FLT_EPSILON) {
			analog = analogVirtual;
		}
	#endif

		if (analog > 1) { analog = 1; }
		if (analog < -1) { analog = -1; }
		return analog;
	}

	float	GetRightAnalogY()
	{
		float analog = 0;
		analog = NDirectX::GetRightJoystickYNormal();

	#ifdef _USE_KEYBOARD_GAME_INPUT
		float analogVirtual = 0;
		analogVirtual = g_virtualRightAnalogY;
		if (abs(analog) < FLT_EPSILON) {
			analog = analogVirtual;
		}
	#endif

		if (analog > 1) { analog = 1; }
		if (analog < -1) { analog = -1; }
		return analog;
	}

	//==========================================================
	// 意味のあるボタン入力
	//==========================================================

	bool GetStart()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_START);
	#endif
		sts |= NDirectX::GetJoystickButtonTrigger(JOY_START);
		return sts;
	}

	bool GetDecide()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_A);
	#endif
		sts |= NDirectX::GetJoystickButtonTrigger(JOY_A);
		return sts;
	}

	bool GetCancel()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_B);
	#endif
		sts |= NDirectX::GetJoystickButtonTrigger(JOY_B);
		return sts;
	}

	bool GetReset()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyPress(GK_START) &&
			NInput::GetKeyDown(GK_BACK);
		sts |= NInput::GetKeyPress(GK_BACK) &&
			NInput::GetKeyDown(GK_START);
	#endif
		sts |= NDirectX::GetJoystickButtonHold(JOY_START) &&
			NDirectX::GetJoystickButtonTrigger(JOY_BACK);
		sts |= NDirectX::GetJoystickButtonHold(JOY_BACK) &&
			NDirectX::GetJoystickButtonTrigger(JOY_START);
		return sts;
	}

	bool GetJump()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_A);
	#endif
		sts |= NDirectX::GetJoystickButtonTrigger(JOY_A);
		return sts;
	}

	bool GetAttack1()
	{
		bool sts = false;
	#ifdef _USE_KEYBOARD_GAME_INPUT
		sts |= NInput::GetKeyDown(GK_X);
	#endif
		sts |= NDirectX::GetJoystickButtonTrigger(JOY_X);
		return sts;
	}
}

namespace {

	void UpdateVirtualAnalogLeft()
	{
		// Y軸
		if (NInput::GetKeyPress(GK_LEFT_ANALOG_UP)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualLeftAnalogY < 0) {
				g_virtualLeftAnalogY = 0;
			}
			// 加速
			g_virtualLeftAnalogY += VIRTUAL_ANALOG_ACC;
		}
		else if (NInput::GetKeyPress(GK_LEFT_ANALOG_DOWN)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualLeftAnalogY > 0) {
				g_virtualLeftAnalogY = 0;
			}
			// 減速
			g_virtualLeftAnalogY -= VIRTUAL_ANALOG_ACC;
		}
		else {
			// 入力がない場合は0に戻す
			g_virtualLeftAnalogY *= VIRTUAL_ANALOG_DEC_RATIO;

			// 最小値をクランプ
			if (abs(g_virtualLeftAnalogY) < VIRTUAL_ANALOG_MIN) {
				g_virtualLeftAnalogY = 0;
			}
		}

		// X軸
		if (NInput::GetKeyPress(GK_LEFT_ANALOG_RIGHT)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualLeftAnalogX < 0) {
				g_virtualLeftAnalogX = 0;
			}
			// 加速
			g_virtualLeftAnalogX += VIRTUAL_ANALOG_ACC;
		}
		else if (NInput::GetKeyPress(GK_LEFT_ANALOG_LEFT)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualLeftAnalogX > 0) {
				g_virtualLeftAnalogX = 0;
			}
			// 減速
			g_virtualLeftAnalogX -= VIRTUAL_ANALOG_ACC;
		}
		else {
			// 入力がない場合は0に戻す
			g_virtualLeftAnalogX *= VIRTUAL_ANALOG_DEC_RATIO;

			// 最小値をクランプ
			if (abs(g_virtualLeftAnalogX) < VIRTUAL_ANALOG_MIN) {
				g_virtualLeftAnalogX = 0;
			}
		}

		// 最大値をクランプ
		if (g_virtualLeftAnalogY > VIRTUAL_ANALOG_MAX) {
			g_virtualLeftAnalogY = 1;
		}
		else if (g_virtualLeftAnalogY < -VIRTUAL_ANALOG_MAX) {
			g_virtualLeftAnalogY = -1;
		}
		if (g_virtualLeftAnalogX > VIRTUAL_ANALOG_MAX) {
			g_virtualLeftAnalogX = 1;
		}
		else if (g_virtualLeftAnalogX < -VIRTUAL_ANALOG_MAX) {
			g_virtualLeftAnalogX = -1;
		}
	}

	void UpdateVirtualAnalogRight()
	{
		// Y軸
		if (NInput::GetKeyPress(GK_RIGHT_ANALOG_UP)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualRightAnalogY < 0) {
				g_virtualRightAnalogY = 0;
			}
			// 加速
			g_virtualRightAnalogY += VIRTUAL_ANALOG_ACC;
		}
		else if (NInput::GetKeyPress(GK_RIGHT_ANALOG_DOWN)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualRightAnalogY > 0) {
				g_virtualRightAnalogY = 0;
			}
			// 減速
			g_virtualRightAnalogY -= VIRTUAL_ANALOG_ACC;
		}
		else {
			// 入力がない場合は0に戻す
			g_virtualRightAnalogY *= VIRTUAL_ANALOG_DEC_RATIO;

			// 最小値をクランプ
			if (abs(g_virtualRightAnalogY) < VIRTUAL_ANALOG_MIN) {
				g_virtualRightAnalogY = 0;
			}
		}

		// X軸
		if (NInput::GetKeyPress(GK_RIGHT_ANALOG_RIGHT)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualRightAnalogX < 0) {
				g_virtualRightAnalogX = 0;
			}
			// 加速
			g_virtualRightAnalogX += VIRTUAL_ANALOG_ACC;
		}
		else if (NInput::GetKeyPress(GK_RIGHT_ANALOG_LEFT)) {
			// 反対側に入力されていた場合は0に戻す
			if (g_virtualRightAnalogX > 0) {
				g_virtualRightAnalogX = 0;
			}
			// 減速
			g_virtualRightAnalogX -= VIRTUAL_ANALOG_ACC;
		}
		else {
			// 入力がない場合は0に戻す
			g_virtualRightAnalogX *= VIRTUAL_ANALOG_DEC_RATIO;

			// 最小値をクランプ
			if (abs(g_virtualRightAnalogX) < VIRTUAL_ANALOG_MIN) {
				g_virtualRightAnalogX = 0;
			}
		}

		// 最大値をクランプ
		if (g_virtualRightAnalogY > VIRTUAL_ANALOG_MAX) {
			g_virtualRightAnalogY = 1;
		}
		else if (g_virtualRightAnalogY < -VIRTUAL_ANALOG_MAX) {
			g_virtualRightAnalogY = -1;
		}
		if (g_virtualRightAnalogX > VIRTUAL_ANALOG_MAX) {
			g_virtualRightAnalogX = 1;
		}
		else if (g_virtualRightAnalogX < -VIRTUAL_ANALOG_MAX) {
			g_virtualRightAnalogX = -1;
		}
	}
}