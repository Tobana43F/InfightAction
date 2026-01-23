#pragma once

#include	<DirectXMath.h>
#include	<array>

namespace NMath {

	struct SInt3;
	struct SInt4;

	//!	@brief	Int構造体
	struct SInt2 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMVECTOR		simd;

			//!	@brief	要素アクセス用
			struct {
				int x, y;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<int, 2>	array;
			};
		};

		SInt2();
		SInt2(const SInt2& _rhs);
		SInt2(const SInt3& _rhs);
		SInt2(const SInt4& _rhs);
		SInt2(
			const int _x,
			const int _y);



		SInt2&	operator =	(const SInt2& _rhs);
		SInt2&	operator +=	(const SInt2& _rhs);
		SInt2&	operator -= (const SInt2& _rhs);
		SInt2&	operator *= (const SInt2& _rhs);
		SInt2&	operator *= (const int& _rhs);
		SInt2&	operator /= (const SInt2& _rhs);
		SInt2&	operator /= (const int& _rhs);

		SInt2&	operator =	(const SInt3& _rhs);
		SInt2&	operator =	(const SInt4& _rhs);


		//!	@brief	配列アクセスで要素を取得
		int		Get(const unsigned int _idx);

		//!	@brief	全ての要素を0にする
		void	SetZero();
	};
}

// 単項演算子の定義
const NMath::SInt2 operator + (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs);
const NMath::SInt2 operator - (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs);

const NMath::SInt2 operator * (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs);
const NMath::SInt2 operator * (const NMath::SInt2& _lhs, const int& _rhs);
const NMath::SInt2 operator * (const int& _lhs, const NMath::SInt2& _rhs);

const NMath::SInt2 operator / (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs);
const NMath::SInt2 operator / (const NMath::SInt2& _lhs, const int& _rhs);
const NMath::SInt2 operator / (const int& _lhs, const NMath::SInt2& _rhs);
