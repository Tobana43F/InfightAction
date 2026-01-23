#pragma once

#include	<DirectXMath.h>
#include	<array>

#include	"Math_Int2.h"
#include	"Math_Int3.h"

namespace NMath {

	//!	@brief	Int構造体
	struct SInt4 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMVECTOR		simd;

			//!	@brief	要素アクセス用
			struct {
				int x, y, z, w;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<int, 4>	array;
			};

			//!	@brief	キャスト代入用
			SInt2	int2;
			SInt3	int3;
		};

		SInt4();
		SInt4(const SInt4& _rhs);
		SInt4(
			const int _x,
			const int _y, 
			const int _z, 
			const int _w);



		SInt4&	operator =	(const SInt4& _rhs);
		SInt4&	operator +=	(const SInt4& _rhs);
		SInt4&	operator -= (const SInt4& _rhs);
		SInt4&	operator *= (const SInt4& _rhs);
		SInt4&	operator *= (const int& _rhs);
		SInt4&	operator /= (const SInt4& _rhs);
		SInt4&	operator /= (const int& _rhs);



		//!	@brief	配列アクセスで要素を取得
		int		Get(const unsigned int _idx);

		//!	@brief	全ての要素を0にする
		void	SetZero();
	};
}

// 単項演算子の定義
const NMath::SInt4 operator + (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs);
const NMath::SInt4 operator - (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs);

const NMath::SInt4 operator * (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs);
const NMath::SInt4 operator * (const NMath::SInt4& _lhs, const int& _rhs);
const NMath::SInt4 operator * (const int& _lhs, const NMath::SInt4& _rhs);

const NMath::SInt4 operator / (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs);
const NMath::SInt4 operator / (const NMath::SInt4& _lhs, const int& _rhs);
const NMath::SInt4 operator / (const int& _lhs, const NMath::SInt4& _rhs);
