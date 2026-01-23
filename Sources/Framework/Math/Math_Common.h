#pragma once

#include	"Math_Type.h"

//!	@brief	基本的な計算を行う関数群
namespace NMath {

	//----------------------------------------------------------
	// 型変換
	//----------------------------------------------------------

	float	ToRad(float _deg);
	float	ToDeg(float _rad);


	//----------------------------------------------------------
	// 行列計算
	//----------------------------------------------------------

	//!	@brief	平行移動行列を作成する
	void MakeMatrixTranslation(SMatrix44& _rDest, const float _x, const float _y, const float _z);
	void MakeMatrixTranslation(SMatrix44& _rDest, const SVector3& _rPos);

	//!	@brief	回転行列を作成する
	void MakeMatrixRotationX(SMatrix44& _rDest, const float _deg);
	void MakeMatrixRotationY(SMatrix44& _rDest, const float _deg);
	void MakeMatrixRotationZ(SMatrix44& _rDest, const float _deg);
	void MakeMatrixRotation(SMatrix44& _rDest, const float _degX, const float _degY, const float _degZ);

	//!	@brief	スケーリング行列を作成する
	void MakeMatrixScaling(SMatrix44& _rDest, const SVector3& _scale);
	void MakeMatrixScaling(SMatrix44& _rDest, const float _x, const float _y, const float _z);
	void MakeMatrixScaling(SMatrix44& _rDest, const float _scale);

	//!	@brief	プロジェクション変換行列を作成する
	void MakeMatrixPerspectiveFovLH(SMatrix44& _rDest, const float _fovAngleYRad, const float _aspectWDivH, const float _nearZ, const float _farZ);

	//!	@brief	ビュー変換行列を作成する
	void MakeMatrixLookAtLH(SMatrix44& _rDest, const SVector3& _rEyePosition, const SVector3& _rFocusPosition, const SVector3& _rUpDirection);

	//!	@brief	平行透視変換行列を作成する
	void MakeMatrixOrthographicLH(SMatrix44& _rDest, const float _left, const float _top, const float _right, const float _bottom, const float _near, const float _far);
	void MakeMatrixOrthographicLH(SMatrix44& _rDest, const float _width, const float _height, const float _near, const float _far);

	//!	@brief	クオータニオンをもとに行列を作成する
	void MakeMatrixQuaternion(SMatrix44& _rDest, const SQuaternion& _rSrc);

	//----------------------------------------------------------
	// ベクトル計算
	//----------------------------------------------------------

	void Cross(SVector2& _rDest, const SVector2& _rVec1, const SVector2& _rVec2);
	void Cross(SVector3& _rDest, const SVector3& _rVec1, const SVector3& _rVec2);

	float Dot(const SVector2& _rVec1, const SVector2& _rVec2);
	float Dot(const SVector3& _rVec1, const SVector3& _rVec2);
	float Dot(const SVector4& _rVec1, const SVector4& _rVec2);
	float Dot(const SQuaternion& _rQt1, const SQuaternion& _rQt2);

	void TransformVector(SVector2& _rInOut, const SMatrix44& _rMatrix);
	void TransformVector(SVector3& _rInOut, const SMatrix44& _rMatrix);
	void TransformVector(SVector4& _rInOut, const SMatrix44& _rMatrix);

	void TransformVector(SVector2& _rDest, const SVector2& _rSrc, const SMatrix44& _rMatrix);
	void TransformVector(SVector3& _rDest, const SVector3& _rSrc, const SMatrix44& _rMatrix);
	void TransformVector(SVector4& _rDest, const SVector4& _rSrc, const SMatrix44& _rMatrix);
	
	//!	@brief	線形補間
	//!	@todo	ベクトル２、４も追加する
	void Lerp(SVector3&	_rDest, const SVector3&	_rStart, const SVector3& _rEnd, const float _ratio);

	//----------------------------------------------------------
	// クオータニオン
	//----------------------------------------------------------

	//!	@brief		2つのベクトルからクォータニオンを作成する
	void MakeQuaternionRotationArc(
		SQuaternion&	_dest,
		const SVector3&	_v0,
		const SVector3&	_v1,
		float*			_pDestAngleRadOut = nullptr);

	//!	@brief	回転行列から、クオータニオンを作成する
	void MakeQuaternionMatrix(SQuaternion& _rDest, const SMatrix44& _rSrc);
	
	//!	@brief	球面線形補間
	void Slerp(SQuaternion& _rDest, const SQuaternion& _rStart, const SQuaternion& _rEnd, const float _ratio);

	//----------------------------------------------------------
	// 三角関数
	//----------------------------------------------------------

	//!	@brief	例外防止のacos関数
	//!	@brief	_cosTheta	内積などででた、cosΘの値
	//!	@retval	Θの値(ラジアン)
	float ArcCos(float _cosTheta);



	//!	@brief		3軸のベクトルを回転行列に設定する
	//!	@param[in]	_isNormal	正規化したベクトルを渡す場合はtrue(計算処理の省略)
	void MakeMatrixRotation(
		SMatrix44&	_dest,
		SVector3&	_x,
		SVector3&	_y,
		SVector3&	_z,
		const bool	_isNormalized = false);

	//!	@brief		座標から注視点を向く回転行列を作成する
	void MakeMatrixLookAt(
		SMatrix44&			_dest, 
		const SVector3&		_pos,
		const SVector3&		_targetPos,
		const SVector3&		_upDir);

	//!	@brief		座標から指定したベクトル方向を向く回転行列を作成する
	void MakeMatrixLookDirection(
		SMatrix44&			_dest,
		const SVector3&		_pos,
		const SVector3&		_dir,
		const SVector3&		_upDir);
	
}
