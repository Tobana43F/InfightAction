#pragma once

#include	"Camera_Def.h"
#include	"Math\Math_Type.h"

//!	@brief	カメラ行列を統括管理する
//!	@note	カメラ行列の書き込み読み込みを行える
//!	@todo	PerspectiveとOrthoの扱いを考える
//!			今は完全に分けているが、インデックスで分けるなど

namespace NCamera {

	//!	@brief	使用可能状態にする
	void SetupGlobalManager();



	//!	@brief	ビュー変換行列を登録
	void RegisterViewMatrix(
		const NMath::SMatrix44& _view,
		const EManagerIndex		_index = EManagerIndex::MAIN);

	//!	@brief	プロジェクション変換行列を登録
	void RegisterProjMatrix(
		const NMath::SMatrix44& _proj,
		const EManagerIndex		_index = EManagerIndex::MAIN);

	//!	@brief	ビューポートを登録
	void RegisterViewport(
		const SViewport&		_viewport,
		const EManagerIndex		_index = EManagerIndex::MAIN);

	//!	@brief	正射影変換行列を登録
	void RegisterOrthoMatrix(
		const NMath::SMatrix44& _ortho,
		const EManagerIndex		_index = EManagerIndex::MAIN);



	//!	@brief	指定したインデックスのビュー変換行列を固定パイプラインに設定
	void SetViewMatrix(const EManagerIndex	_index = EManagerIndex::MAIN);

	//!	@brief	指定したインデックスのプロジェクション変換行列を固定パイプラインに設定
	void SetProjMatrix(const EManagerIndex	_index = EManagerIndex::MAIN);

	//!	@brief	指定したインデックスのビューポートぱイプラインに設定
	void SetViewport(const EManagerIndex	_index = EManagerIndex::MAIN);

	//!	@brief	指定したインデックスの正射影変換行列を固定パイプラインに設定
	void SetOrthoMatrix(const EManagerIndex	_index = EManagerIndex::MAIN);



	//!	@brief	ビュー変換行列を取得
	void GetViewMatrix(
		NMath::SMatrix44&		_dest,
		const EManagerIndex		_index = EManagerIndex::MAIN);

	//!	@brief	プロジェクション変換行列を取得
	void GetProjMatrix(
		NMath::SMatrix44&		_dest,
		const EManagerIndex		_index = EManagerIndex::MAIN);

	//!	@brief	ビューポートを取得
	void GetViewport(
		SViewport&				_dest,
		const EManagerIndex		_index = EManagerIndex::MAIN);

	//!	@brief	正射影変換行列を取得する
	void GetOrthoMatrix(
		NMath::SMatrix44& _dest,
		const EManagerIndex		_index = EManagerIndex::MAIN);

}
