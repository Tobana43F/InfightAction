#pragma once

//!	@brief		ゲーム用入力の履歴を保持する
//!	@note		共通事項
//!				- Input_Game の入力を毎フレーム取得し、履歴を保持する
//!				  取得できる値についてはInput_Gameを参照
//!				- 履歴をとる必要のない値は保持しない(決定キーなど)
//!	@param[in]	_frame		何フレーム前の情報を取得するかを指定(0 = 現在の情報を取得する)
//!	@param[in]	_endFrame	_frameから_endFrameの間で入力がある場合、trueを返す
//!							_frameと_endFrameを含む	
//!							_endFrameにUINT_MAXを指定すると、範囲指定を無効にする
//!	@param[out]	_pDestFrame	範囲を指定した場合、trueになった一番直近のフレームを取得する

namespace NInput {

	const uint	GAME_BUFFER_SIZE = 300;	// +1のサイズ分の配列が確保される(この値をループに使える)

	//----------------------------------------------------------

	//!	@brief	バッファの更新
	void UpdateGameBuffer();

	//----------------------------------------------------------

	bool GetUpHold(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
	bool GetDownHold(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
	bool GetRightHold(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
	bool GetLeftHold(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);

	bool GetUpTrigger(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
	bool GetDownTrigger(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
	bool GetRightTrigger(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
	bool GetLeftTrigger(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);

	float	GetLeftAnalogX(uint _frame);
	float	GetLeftAnalogY(uint _frame);
	float	GetRightAnalogX(uint _frame);
	float	GetRightAnalogY(uint _frame);

	bool GetJump(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);

	bool GetAttack1(uint _frame, uint _endFrame = UINT_MAX, uint* _pDestFrame = nullptr);
}