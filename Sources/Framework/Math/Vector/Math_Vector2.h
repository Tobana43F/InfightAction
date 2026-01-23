#pragma once

#include	<DirectXMath.h>
#include	<array>

namespace NMath {

	// 前方宣言
	struct SVector3;
	struct SVector4;

	//!	@brief	Vector構造体
	struct SVector2 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMVECTOR			simd;

			//!	@brief	要素アクセス用
			struct {
				float x, y;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<float, 2>	array;
			};
		};

		SVector2();
		SVector2(const SVector2& _rhs);
		SVector2(const SVector3& _rhs);
		SVector2(const SVector4& _rhs);
		SVector2(
			const float _x,
			const float _y);



		SVector2&	operator =	(const SVector2& _rhs);
		SVector2&	operator +=	(const SVector2& _rhs);
		SVector2&	operator -= (const SVector2& _rhs);
		SVector2&	operator *= (const SVector2& _rhs);
		SVector2&	operator *= (const float& _rhs);
		SVector2&	operator /= (const SVector2& _rhs);
		SVector2&	operator /= (const float& _rhs);

		SVector2&	operator =	(const SVector3& _rhs);
		SVector2&	operator =	(const SVector4& _rhs);



		//!	@brief	配列アクセスで要素を取得
		float	Get(const unsigned int _idx) const;

		//!	@brief	全ての要素を0にする
		void	SetZero();

		//!	@brief	要素を正規化する
		void	Normalize();

		//!	@brief	ベクトルの長さを取得する
		float	GetLength() const;

		//!	@brief	ベクトルの長さの2乗を取得する(計算量省略用)
		float	GetLengthSquare() const;

		//!	@brief	正規化した値を取得する
		void 	GetNormal(SVector2& _rDest) const;
	};
}

// 単項演算子の定義
const NMath::SVector2 operator + (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs);
const NMath::SVector2 operator - (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs);

const NMath::SVector2 operator * (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs);
const NMath::SVector2 operator * (const NMath::SVector2& _lhs, const float& _rhs);
const NMath::SVector2 operator * (const float& _lhs, const NMath::SVector2& _rhs);

const NMath::SVector2 operator / (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs);
const NMath::SVector2 operator / (const NMath::SVector2& _lhs, const float& _rhs);
const NMath::SVector2 operator / (const float& _lhs, const NMath::SVector2& _rhs);
