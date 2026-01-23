#pragma once

//	ゲーム開発で使用するよく使う汎用的な関数を作成する場所
//	テスト用のカメラ操作などを定義する。

#include	"Camera\Camera_GlobalManager.h"

namespace NUtil {

	//!	@brief		ベクトルをデバッグフォントで出力
	void PrintVector(
		const float _x,
		const float _y,
		const NMath::SVector2&	_rSrc);

	//!	@brief		ベクトルをデバッグフォントで出力
	void PrintVector(
		const float _x,
		const float _y,
		const NMath::SVector3&	_rSrc);

	//!	@brief		ベクトルをデバッグフォントで出力
	void PrintVector(
		const float _x,
		const float _y,
		const NMath::SVector4&	_rSrc);


	//!	@brief		行列をデバッグフォントで出力
	//!	@param[in]	_pos	表示座標
	//!	@param[in]	_mat	表示する行列
	void PrintMatrix(
		const float2&			_pos,
		const NMath::SMatrix44&	_mat);


	//!	@brief		指定したベクトルを現在のカメラからの相対ベクトルにする
	void ToVectorFromCamera(
		NMath::SVector3&		_dest,
		const NMath::SVector3&	_src,
		const NCamera::EManagerIndex camIndex = NCamera::EManagerIndex::MAIN);

	//!	@brief		指定したベクトルを指定したカメラからの相対ベクトルにする
	//!	@note		XZ平面のみ適用する
	void ToVectorFromCameraXZ(
		NMath::SVector3&		_dest,
		const NMath::SVector3&	_src,
		const NCamera::EManagerIndex camIndex = NCamera::EManagerIndex::MAIN);
}