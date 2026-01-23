#include	"Math_Vector3.h"

#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SVector3::SVector3()
	{
		memset(this, 0, sizeof(*this));
	}

	SVector3::SVector3(const SVector3& _rhs)
	{
		memset(this, 0, sizeof(*this));
		memcpy(this, &_rhs, sizeof(float) * 3);
	}

	SVector3::SVector3(const SVector4& _rhs)
	{
		memset(this, 0, sizeof(*this));
		memcpy(this, &_rhs, sizeof(float) * 3);
	}

	SVector3::SVector3(
		const float _x,
		const float _y,
		const float _z)
	{
		memset(this, 0, sizeof(*this));
		x = _x;
		y = _y;
		z = _z;
	}



	SVector3& SVector3::operator =  (const SVector3& _rhs)
	{
		memcpy(this, &_rhs, sizeof(float) * 3);
		return *this;
	}

	SVector3& SVector3::operator += (const SVector3& _rhs)
	{
		simd += _rhs.simd;
		return *this;
	}

	SVector3& SVector3::operator -= (const SVector3& _rhs)
	{
		simd -= _rhs.simd;
		return *this;
	}

	SVector3& SVector3::operator *= (const SVector3& _rhs)
	{
		simd *= _rhs.simd;
		return *this;
	}

	SVector3& SVector3::operator *= (const float& _rhs)
	{
		simd *= _rhs;
		return *this;
	}

	SVector3& SVector3::operator /= (const SVector3& _rhs)
	{
		simd /= _rhs.simd;
		return *this;
	}

	SVector3& SVector3::operator /= (const float& _rhs)
	{
		simd /= _rhs;
		return *this;
	}

	SVector3& SVector3::operator =  (const SVector4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(float) * 3);
		return *this;
	}



	float	SVector3::Get(const unsigned int _idx) const
	{
		assert(_idx < array.size());
		return array[_idx];
	}

	void	SVector3::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}

	void	SVector3::Normalize()
	{
		GetNormal(*this);
	}

	float	SVector3::GetLength() const
	{
		return XMVector3Length(simd).m128_f32[0];
	}

	float	SVector3::GetLengthSquare() const
	{
		return XMVector3LengthSq(simd).m128_f32[0];
	}

	void 	SVector3::GetNormal(SVector3& _rDest) const
	{
		_rDest.simd = XMVector3Normalize(simd);
	}
}



const NMath::SVector3 operator + (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs)
{
	return NMath::SVector3(_lhs) += _rhs;
}

const NMath::SVector3 operator - (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs)
{
	return NMath::SVector3(_lhs) -= _rhs;
}

const NMath::SVector3 operator * (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs)
{
	return NMath::SVector3(_lhs) *= _rhs;
}

const NMath::SVector3 operator * (const NMath::SVector3& _lhs, const float& _rhs)
{
	return NMath::SVector3(_lhs) *= _rhs;
}

const NMath::SVector3 operator * (const float& _lhs, const NMath::SVector3& _rhs)
{
	return NMath::SVector3(_rhs) *= _lhs;
}

const NMath::SVector3 operator / (const NMath::SVector3& _lhs, const NMath::SVector3& _rhs)
{
	return NMath::SVector3(_lhs) /= _rhs;
}

const NMath::SVector3 operator / (const NMath::SVector3& _lhs, const float& _rhs)
{
	return NMath::SVector3(_lhs) /= _rhs;
}

const NMath::SVector3 operator / (const float& _lhs, const NMath::SVector3& _rhs)
{
	return NMath::SVector3(_rhs) /= _lhs;
}
