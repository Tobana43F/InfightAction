#include	"Input_GameBuffer.h"

#include	"Debug\Debug_Assert.h"
#include	"Input\Input_Game.h"

//!	@brief		バッファの更新の記述を省略するマクロ
//!	@param[in]	_bufArray	更新するバッファ
//!	@param[in]	_funcName	そのバッファの最新情報を取得する関数(NInputの関数)
//!				for分で使用するインデックスは整数の i とする
#define UpdateOldBuffer(_bufArray, _funcName)	{		\
	_bufArray[i] = _bufArray[i - 1];					\
	if(i == 1){											\
		_bufArray[0] = NInput::_funcName();				\
	}													\
}

//!	@brief		バッファオーバーフローを検知するためのアサートの記述を省略するマクロ
#define	AssertBuf(_idx) Assert(_idx < BUF_SIZE && "バッファーのサイズ外アクセスです。")

//!	@brief		boolのゲッターの記述を省略するマクロ
//!	@param[in]	_boolDest	結果の代入さき
//!	@param[in]	_bufArray	参照するバッファ
//!	@param		その他は、ヘッダを参照
#define	GetBool(_boolDest, _bufArray, _frame, _endFrame, _pDestFrame)	\
	if (_endFrame != UINT_MAX) {										\
		Assert(_endFrame > _frame && "範囲が正しくありません");			\
		for (uint i = _frame; i <= _endFrame; ++i) {					\
			_boolDest |= _bufArray[i];									\
			if (_boolDest == true) {									\
				if (_pDestFrame != nullptr) { *_pDestFrame = i ; }		\
				break;													\
			}															\
		}																\
	}																	\
	else {																\
		_boolDest = _bufArray[_frame];									\
	}																	

namespace {
	// アクセス省略
	const uint BUF_SIZE = NInput::GAME_BUFFER_SIZE + 1;

	//!	@brief	データを管理するグローバル変数
	bool	g_bufUpHold[BUF_SIZE];
	bool	g_bufDownHold[BUF_SIZE];
	bool	g_bufRightHold[BUF_SIZE];
	bool	g_bufLeftHold[BUF_SIZE];

	bool	g_bufUpTrig[BUF_SIZE];
	bool	g_bufDownTrig[BUF_SIZE];
	bool	g_bufRightTrig[BUF_SIZE];
	bool	g_bufLeftTrig[BUF_SIZE];

	float	g_bufLeftAnalogX[BUF_SIZE];
	float	g_bufLeftAnalogY[BUF_SIZE];
	float	g_bufRightAnalogX[BUF_SIZE];
	float	g_bufRightAnalogY[BUF_SIZE];

	bool	g_bufJump[BUF_SIZE];
	bool	g_bufAttack1[BUF_SIZE];
}

namespace NInput {

	void UpdateGameBuffer()
	{
		// 過去バッファを更新
		for (uint i = BUF_SIZE - 1; i > 0; --i) {
			UpdateOldBuffer(g_bufUpHold, GetUpHold);
			UpdateOldBuffer(g_bufDownHold, GetDownHold);
			UpdateOldBuffer(g_bufRightHold, GetRightHold);
			UpdateOldBuffer(g_bufLeftHold, GetLeftHold);

			UpdateOldBuffer(g_bufUpTrig, GetUpTrigger);
			UpdateOldBuffer(g_bufDownTrig, GetDownTrigger);
			UpdateOldBuffer(g_bufRightTrig, GetRightTrigger);
			UpdateOldBuffer(g_bufLeftTrig, GetLeftTrigger);

			UpdateOldBuffer(g_bufLeftAnalogX, GetLeftAnalogX);
			UpdateOldBuffer(g_bufLeftAnalogY, GetLeftAnalogY);
			UpdateOldBuffer(g_bufRightAnalogX, GetRightAnalogX);
			UpdateOldBuffer(g_bufRightAnalogY, GetRightAnalogY);

			UpdateOldBuffer(g_bufJump, GetJump);
			UpdateOldBuffer(g_bufAttack1, GetAttack1);
		}
	}



	bool GetUpHold(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufUpHold, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetDownHold(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufDownHold, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetRightHold(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufRightHold, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetLeftHold(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufLeftHold, _frame, _endFrame, _pDestFrame);
		return sts;
	}



	bool GetUpTrigger(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufUpTrig, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetDownTrigger(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufDownTrig, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetRightTrigger(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufRightTrig, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetLeftTrigger(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufLeftTrig, _frame, _endFrame, _pDestFrame);
		return sts;
	}



	float	GetLeftAnalogX(uint _frame)
	{
		AssertBuf(_frame);
		return g_bufLeftAnalogX[_frame];
	}

	float	GetLeftAnalogY(uint _frame)
	{
		AssertBuf(_frame);
		return g_bufLeftAnalogY[_frame];
	}

	float	GetRightAnalogX(uint _frame)
	{
		AssertBuf(_frame);
		return g_bufRightAnalogX[_frame];
	}

	float	GetRightAnalogY(uint _frame)
	{
		AssertBuf(_frame);
		return g_bufRightAnalogY[_frame];
	}



	bool GetJump(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufJump, _frame, _endFrame, _pDestFrame);
		return sts;
	}

	bool GetAttack1(uint _frame, uint _endFrame, uint* _pDestFrame)
	{
		AssertBuf(_frame);
		bool sts = false;
		GetBool(sts, g_bufAttack1, _frame, _endFrame, _pDestFrame);
		return sts;
	}

}
