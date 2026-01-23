#pragma once

#include	<DirectXMath.h>
#include	<array>

#include	"Math_Vector2.h"
#include	"Math_Vector3.h"

namespace NMath {

	//!	@brief	Vector構造体
	struct SVector4 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMVECTOR			simd;

			//!	@brief	要素アクセス用
			struct {
				float x, y, z, w;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<float, 4>	array;
			};

			//!	@brief	キャスト代入用
			SVector2	vec2;
			SVector3	vec3;
		};

		SVector4();
		SVector4(const SVector4& _rhs);
		SVector4(
			const float _x,
			const float _y, 
			const float _z, 
			const float _w);



		SVector4&	operator =	(const SVector4& _rhs);
		SVector4&	operator +=	(const SVector4& _rhs);
		SVector4&	operator -= (const SVector4& _rhs);
		SVector4&	operator *= (const SVector4& _rhs);
		SVector4&	operator *= (const float& _rhs);
		SVector4&	operator /= (const SVector4& _rhs);
		SVector4&	operator /= (const float& _rhs);



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
		void 	GetNormal(SVector4& _rDest) const;
	};
}

// 単項演算子の定義
const NMath::SVector4 operator + (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs);
const NMath::SVector4 operator - (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs);

const NMath::SVector4 operator * (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs);
const NMath::SVector4 operator * (const NMath::SVector4& _lhs, const float& _rhs);
const NMath::SVector4 operator * (const float& _lhs, const NMath::SVector4& _rhs);

const NMath::SVector4 operator / (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs);
const NMath::SVector4 operator / (const NMath::SVector4& _lhs, const float& _rhs);
const NMath::SVector4 operator / (const float& _lhs, const NMath::SVector4& _rhs);
