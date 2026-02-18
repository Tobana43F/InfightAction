#pragma once

//!	@brief	デバッグモードの切り替えなどの定数を定義
//!	@note	変更時の影響を抑えるため、値の設定はCPPで行う。
//!	@todo	命名規則が定数のものになっている

namespace NDebug {
	
	//!	@brief		デバッグモードの切り替え
	extern	bool	IS_DEBUG_MODE;

	//!	@brief		一時停止を行うフラグ
	extern	bool	IS_PAUSE;
	
	//!	@brief		一時停止中にステップするフラグ
	extern	bool	IS_PAUSE_STEP;

	//!	@brief		スロー再生フラグ
	extern	bool	IS_SLOW;

	//!	@brief		スロー倍率
	//!	@note		0 < x < 1
	extern	float	SLOW_RATIO;

	//!	@brief		FBXのロード時の情報を出力するかの切り替え
	extern	bool	IS_FBX_LOAD_INFO_ENABLE;
}