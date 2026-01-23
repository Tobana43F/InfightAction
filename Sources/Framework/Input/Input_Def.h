#pragma once

#include	"DirectX\DirectX_Input.h"

namespace NInput {

	//!	@brief	ジョイスティックの入力に使用する定数
	enum EJoystickDef {
		JOY_A,
		JOY_B,
		JOY_X,
		JOY_Y,
		JOY_L,
		JOY_R,
		JOY_BACK,
		JOY_START,
		JOY_L3,
		JOY_R3,
		JOY_DEF_MAX
	};

	//!	@brief	システムで使用するボタンの定義
	enum ESystemKeyDef {
		SYS_DEBUG_SWITCH_FREE_CAM	= DIK_F2,	// デバッグカメラに切り替え
		SYS_DEBUG_SWITCH_FREE_CAM_LOCK = DIK_F1,	// デバッグカメラの固定

		SYS_DEBUG_PAUSE_SWITCH		= DIK_F6,	// ポーズ切り替えボタン
		SYS_DEBUG_PAUSE_STEP		= DIK_F7,	// ポーズ中にステップするボタン
		SYS_DEBUG_SLOW_SWITCH		= DIK_F5,	// スロー切り替えボタン
		SYS_DEBUG_SLOW_SPEED_UP		= DIK_F4,	// スロー加速ボタン
		SYS_DEBUG_SLOW_SPEED_DOWN	= DIK_F3	// スロー減速ボタン
	};

}