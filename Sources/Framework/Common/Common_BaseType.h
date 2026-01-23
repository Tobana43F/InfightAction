#pragma once

//! 頻繁に使用する型を宣言する

#include	"Math\Math_Type.h"

namespace NCommon {
	// 使用する型の別名を設定
	typedef unsigned int	uint;	// 省略
	typedef unsigned char	uchar;
	typedef const char		cchar;
	typedef wchar_t			wchar;
	typedef const wchar_t	cwchar;

	typedef NMath::SInt2	int2;	// 名前を統一するため
	typedef NMath::SInt3	int3;
	typedef NMath::SInt4	int4;

	typedef NMath::SVector2	float2;	// ベクトルを意識せずに使用する場合はこちらを使用する
	typedef NMath::SVector3	float3;	// ただし sizeof でのサイズが float * 4 になることに注意
	typedef NMath::SVector4	float4;

}
