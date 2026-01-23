#pragma once

// Fbxで使用する便利な関数群

#include	<fbxsdk.h>
#include	"Math\Math_Type.h"

namespace NModel {
	
	//!	@brief	Fbxの行列を独自定義の行列に変換
	void ToSMatrix44(
		NMath::SMatrix44&	_rDest,
		const FbxAMatrix&	_rSrc)
	{
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				_rDest.flt[y][x] = StaticCast<float>(_rSrc.Get(y, x));
			}
		}
	}

	//!	@brief	Fbxの行列を独自定義の行列に変換
	void ToSMatrix44(
		NMath::SMatrix44&	_rDest,
		const FbxMatrix&	_rSrc)
	{
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				_rDest.flt[y][x] = StaticCast<float>(_rSrc.Get(y, x));
			}
		}
	}

}