#pragma once

#include	<DirectXMath.h>
#include	<array>

#include	"Math_Vector2.h"

namespace NMath {

	// 前方宣言
	struct SVector4;

	//!	@brief	Vector構造体
	struct SVector3 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMVECTOR			simd;

			//!	@brief	要素アクセス用
			struct {
				float x, y, z;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<float, 3>	array;
			};

			//!	@brief	キャスト代入用
			SVector2	vec2;
		};

		SVector3();
		SVector3(const SVector3& _rhs);
		SVector3(const SVector4& _rhs);
		SVector3(
			const float _x,
			const float _y, 
			const float _z);



		SVector3&	operator =	(const SVector3& _rhs);
		SVector3&	operator +=	(const SVector3& _rhs);
		SVector3&	operator -= (const SVector3& _rhs);
		SVector3&	operator *= (const SVector3& _rhs);
		SVector3&	operator *= (const float& _rhs);
		SVector3&	operator /= (const SVector3& _rhs);
		SVector3&	operator /= (const float& _rhs);

		SVector3&	operator =	(const SVector4& _rhs);



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
		void 	GetNormal(SVector3& _rDest) const;
	};
}

// 単項演算子の定義
const NMath::SVector3 operator + (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs);
const NMath::SVector3 operator - (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs);

const NMath::SVector3 operator * (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs);
const NMath::SVector3 operator * (const NMath::SVector3& _lhs, const float& _rhs);
const NMath::SVector3 operator * (const float& _lhs, const NMath::SVector3& _rhs);

const NMath::SVector3 operator / (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs);
const NMath::SVector3 operator / (const NMath::SVector3& _lhs, const float& _rhs);
const NMath::SVector3 operator / (const float& _lhs, const NMath::SVector3& _rhs);
