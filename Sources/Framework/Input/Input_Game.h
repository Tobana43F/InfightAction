#include	"Input_Def.h"

//	ゲーム内で使用する入力関数
//	キーボード、ジョイスティックなどの入力をまとめて扱える

//!	@note	新しい入力を追加した場合は、必要であるなら
//!			Input_GameBuffer に関数を追加する。

namespace NInput {
	//!	@brief	仮想アナログ入力を更新する
	void UpdateVirtualAnalog();

	//!	@brief	汎用方向入力
	bool GetUpHold();
	bool GetDownHold();
	bool GetRightHold();
	bool GetLeftHold();

	bool GetUpTrigger();
	bool GetDownTrigger();
	bool GetRightTrigger();
	bool GetLeftTrigger();

	//!	@brief		アナログ入力
	//!	@details	ジョイスティックの入力が優先される
	//!	@return		ジョイスティックのアナログ値
	//!				-1 ~ 1 
	float	GetLeftAnalogX();
	float	GetLeftAnalogY();
	float	GetRightAnalogX();
	float	GetRightAnalogY();

	//!	@brief	スタートボタンを取得
	//!	@todo	GameBufferの方に追加していない(Bufferを使用することがあるなら追加する)
	bool GetStart();


	//!	@brief	決定キー
	bool GetDecide();

	//!	@brief	キャンセルキー
	bool GetCancel();

	//!	@brief	リセットボタン(システムキー)
	//!	@note	ゲームのタイトル画面に戻るなどに使用する
	bool GetReset();

	//!	@brief	ジャンプボタン
	bool GetJump();

	//!	@brief	攻撃ボタン１
	bool GetAttack1();

}
