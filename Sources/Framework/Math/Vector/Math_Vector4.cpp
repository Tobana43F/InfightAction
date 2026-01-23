#include	"Math_Vector4.h"

#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SVector4::SVector4()
	{
		memset(this, 0, sizeof(*this));
	}

	SVector4::SVector4(const SVector4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SVector4::SVector4(
		const float _x,
		const float _y,
		const float _z,
		const float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}



	SVector4& SVector4::operator =  (const SVector4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SVector4& SVector4::operator += (const SVector4& _rhs)
	{
		simd += _rhs.simd;
		return *this;
	}

	SVector4& SVector4::operator -= (const SVector4& _rhs)
	{
		simd -= _rhs.simd;
		return *this;
	}

	SVector4& SVector4::operator *= (const SVector4& _rhs)
	{
		simd *= _rhs.simd;
		return *this;
	}

	SVector4& SVector4::operator *= (const float& _rhs)
	{
		simd *= _rhs;
		return *this;
	}

	SVector4& SVector4::operator /= (const SVector4& _rhs)
	{
		simd /= _rhs.simd;
		return *this;
	}

	SVector4& SVector4::operator /= (const float& _rhs)
	{
		simd /= _rhs;
		return *this;
	}



	float	SVector4::Get(const unsigned int _idx) const
	{
		assert(_idx < array.size());
		return array[_idx];
	}

	void	SVector4::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}

	void	SVector4::Normalize()
	{
		GetNormal(*this);
	}

	float	SVector4::GetLength() const
	{
		return XMVector4Length(simd).m128_f32[0];
	}

	float	SVector4::GetLengthSquare() const
	{
		return XMVector4LengthSq(simd).m128_f32[0];
	}

	void 	SVector4::GetNormal(SVector4& _rDest) const
	{
		_rDest.simd = XMVector4Normalize(simd);
	}
}



const NMath::SVector4 operator + (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs)
{
	return NMath::SVector4(_lhs) += _rhs;
}

const NMath::SVector4 operator - (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs)
{
	return NMath::SVector4(_lhs) -= _rhs;
}

const NMath::SVector4 operator * (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs)
{
	return NMath::SVector4(_lhs) *= _rhs;
}

const NMath::SVector4 operator * (const NMath::SVector4& _lhs, const float& _rhs)
{
	return NMath::SVector4(_lhs) *= _rhs;
}

const NMath::SVector4 operator * (const float& _lhs, const NMath::SVector4& _rhs)
{
	return NMath::SVector4(_rhs) *= _lhs;
}

const NMath::SVector4 operator / (const NMath::SVector4& _lhs, const NMath::SVector4& _rhs)
{
	return NMath::SVector4(_lhs) /= _rhs;
}

const NMath::SVector4 operator / (const NMath::SVector4& _lhs, const float& _rhs)
{
	return NMath::SVector4(_lhs) /= _rhs;
}

const NMath::SVector4 operator / (const float& _lhs, const NMath::SVector4& _rhs)
{
	return NMath::SVector4(_rhs) /= _lhs;
}
