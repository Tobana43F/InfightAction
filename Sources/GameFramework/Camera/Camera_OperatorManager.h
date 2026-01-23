#pragma once

//　	カメラの操作を行うクラスを管理するマネージャ

#include	"Camera\Camera_Def.h"
#include	"Camera\Operator\Camera_OperatorBase.h"

namespace NCamera {

	//!	@brief	使用できる状態にする(初期化)
	//!	@note	CameraCommonのUpdate()でよびだす。
	void SetupOperatorManager();

	//!	@brief	登録されたオペレータを更新
	//!	@note	CameraCommonのUpdate()でよびだす。
	void UpdateOperatorManager();

	//!	@brief		オペレータを登録
	//!	@param[in]	_pOperator	継承後のオペレータクラスのポインタを登録
	//!	@param[in]	_index		どのカメラを操作するかを指定
	//!	@return		既に登録してあったオペレータへのポインタ()
	//!	@warning	注意
	//!				- 同じオペレータを複数のカメラ操作に割り当ててはいけない。
	//!				- 登録したオペレータが解放されるときは、必ず
	//!				  UnregisterOperator()で登録を解除すること。
	COperatorBase* RegisterOperator(
		COperatorBase*			_pOperator,
		EManagerIndex			_index = EManagerIndex::MAIN);

	//!	@brief		登録したオペレータを無効化する
	//!	@param[in]	_pOperator	登録していたカメラオペレータ(Not Null)
	//!	@note		登録したオペレータが解放されるときは、必ず呼ぶ。
	//!	@note		他のオペレータで上書きされていた場合は、何もしない。
	void UnregisterOperator(COperatorBase*	_pOperator);

}
