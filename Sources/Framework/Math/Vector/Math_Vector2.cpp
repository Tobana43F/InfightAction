#include	"Math_Vector2.h"

#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SVector2::SVector2()
	{
		memset(this, 0, sizeof(*this));
	}

	SVector2::SVector2(const SVector2& _rhs)
	{
		memset(this, 0, sizeof(*this));
		memcpy(this, &_rhs, sizeof(float) * 2);
	}

	SVector2::SVector2(const SVector3& _rhs)
	{
		memset(this, 0, sizeof(*this));
		memcpy(this, &_rhs, sizeof(float) * 2);
	}

	SVector2::SVector2(const SVector4& _rhs)
	{
		memset(this, 0, sizeof(*this));
		memcpy(this, &_rhs, sizeof(float) * 2);
	}

	SVector2::SVector2(
		const float _x,
		const float _y)
	{
		memset(this, 0, sizeof(*this));
		x = _x;
		y = _y;
	}



	SVector2& SVector2::operator =  (const SVector2& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SVector2& SVector2::operator += (const SVector2& _rhs)
	{
		simd += _rhs.simd;
		return *this;
	}

	SVector2& SVector2::operator -= (const SVector2& _rhs)
	{
		simd -= _rhs.simd;
		return *this;
	}

	SVector2& SVector2::operator *= (const SVector2& _rhs)
	{
		simd *= _rhs.simd;
		return *this;
	}

	SVector2& SVector2::operator *= (const float& _rhs)
	{
		simd *= _rhs;
		return *this;
	}

	SVector2& SVector2::operator /= (const SVector2& _rhs)
	{
		simd /= _rhs.simd;
		return *this;
	}

	SVector2& SVector2::operator /= (const float& _rhs)
	{
		simd /= _rhs;
		return *this;
	}

	SVector2& SVector2::operator =  (const SVector3& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SVector2& SVector2::operator =  (const SVector4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}



	float	SVector2::Get(const unsigned int _idx) const
	{
		assert(_idx < array.size());
		return array[_idx];
	}

	void	SVector2::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}

	void	SVector2::Normalize()
	{
		GetNormal(*this);
	}

	float	SVector2::GetLength() const
	{
		return XMVector2Length(simd).m128_f32[0];
	}

	float	SVector2::GetLengthSquare() const
	{
		return XMVector2LengthSq(simd).m128_f32[0];
	}

	void 	SVector2::GetNormal(SVector2& _rDest) const
	{
		_rDest.simd = XMVector2Normalize(simd);
	}
}



const NMath::SVector2 operator + (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs)
{
	return NMath::SVector2(_lhs) += _rhs;
}

const NMath::SVector2 operator - (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs)
{
	return NMath::SVector2(_lhs) -= _rhs;
}

const NMath::SVector2 operator * (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs)
{
	return NMath::SVector2(_lhs) *= _rhs;
}

const NMath::SVector2 operator * (const NMath::SVector2& _lhs, const float& _rhs)
{
	return NMath::SVector2(_lhs) *= _rhs;
}

const NMath::SVector2 operator * (const float& _lhs, const NMath::SVector2& _rhs)
{
	return NMath::SVector2(_rhs) *= _lhs;
}

const NMath::SVector2 operator / (const NMath::SVector2& _lhs, const NMath::SVector2& _rhs)
{
	return NMath::SVector2(_lhs) /= _rhs;
}

const NMath::SVector2 operator / (const NMath::SVector2& _lhs, const float& _rhs)
{
	return NMath::SVector2(_lhs) /= _rhs;
}

const NMath::SVector2 operator / (const float& _lhs, const NMath::SVector2& _rhs)
{
	return NMath::SVector2(_rhs) /= _lhs;
}
