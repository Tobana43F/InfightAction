#pragma once

//!	@file	
//!	@brief	デバッグに便利なユーティリティ関数群

namespace NDebug {

	//!	@brief	ラインを描画
	void	RenderLine(
		const float3& _rStart,
		const float3& _rEnd,
		const float4& _rColor = float4(0, 0, 0, 1));

	//!	@brief	座標を3軸のクロスで描画
	void	RenderPoint(
		const float3&	_rPos,
		const float		_len = 0.1f,
		const float		_alpha = 1.0f);

	//!	@brief	軸方向を描画
	void RenderAxisDir(
		const NMath::SMatrix44&	_rMat, 
		const float				_length = 0.1f,
		const float				_alpha = 1.0f);

	//!	@brief	簡易的な球をワイヤーフレームで描画する
	void	RenderWireFrameSphere(
		const float3&		_centerPos,
		const float			_radius,
		const float			_alpha = 1.0f);
}