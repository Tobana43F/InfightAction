#pragma once

#include	<stdio.h>

namespace NDebug {

	//!	@brief		デバッグ時に文字列をコンソールへ表示
	//!	@details	512文字まで表示可能
	void PrintDebug(const char* _pFmt, ...);

	//!	@brief		文字を出力するかを指定してコンソールへ表示
	void Print(bool _isEnable, cchar* _pFmt, ...);
	
	//!	@brief		状態を指定して文字列を出力する
	void PrintSwitch(bool _isEnable, bool _state, cchar* _msg1, cchar* _msg2);


	//!	@brief		VisualStudioの出力ウィンドウにログを表示
	void OutLogMessage(cchar* _pMessage);

	//!	@brief		出力するかを指定してログを表示
	void OutLogMessage(bool _isEnable, cchar* _pMessage);
}