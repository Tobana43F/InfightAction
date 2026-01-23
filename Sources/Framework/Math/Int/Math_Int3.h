#pragma once

#include	<DirectXMath.h>
#include	<array>

#include	"Math_Int2.h"

namespace NMath {

	struct SInt4;

	//!	@brief	Int構造体
	struct SInt3 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMVECTOR		simd;

			//!	@brief	要素アクセス用
			struct {
				int x, y, z;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<int, 3>	array;
			};

			//!	@brief	キャスト代入用
			SInt2	int2;
		};

		SInt3();
		SInt3(const SInt3& _rhs);
		SInt3(const SInt4& _rhs);
		SInt3(
			const int _x,
			const int _y,
			const int _z);



		SInt3&	operator =	(const SInt3& _rhs);
		SInt3&	operator +=	(const SInt3& _rhs);
		SInt3&	operator -= (const SInt3& _rhs);
		SInt3&	operator *= (const SInt3& _rhs);
		SInt3&	operator *= (const int& _rhs);
		SInt3&	operator /= (const SInt3& _rhs);
		SInt3&	operator /= (const int& _rhs);

		SInt3&	operator =	(const SInt4& _rhs);



		//!	@brief	配列アクセスで要素を取得
		int		Get(const unsigned int _idx);

		//!	@brief	全ての要素を0にする
		void	SetZero();
	};
}

// 単項演算子の定義
const NMath::SInt3 operator + (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs);
const NMath::SInt3 operator - (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs);

const NMath::SInt3 operator * (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs);
const NMath::SInt3 operator * (const NMath::SInt3& _lhs, const int& _rhs);
const NMath::SInt3 operator * (const int& _lhs, const NMath::SInt3& _rhs);

const NMath::SInt3 operator / (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs);
const NMath::SInt3 operator / (const NMath::SInt3& _lhs, const int& _rhs);
const NMath::SInt3 operator / (const int& _lhs, const NMath::SInt3& _rhs);
