#include	"Math_Int3.h"

#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SInt3::SInt3()
	{
		memset(this, 0, sizeof(*this));
	}

	SInt3::SInt3(const SInt3& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SInt3::SInt3(const SInt4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SInt3::SInt3(
		const int _x,
		const int _y,
		const int _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}



	SInt3& SInt3::operator =  (const SInt3& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SInt3& SInt3::operator += (const SInt3& _rhs)
	{
		this->x += _rhs.x;
		this->y += _rhs.y;
		this->z += _rhs.z;
		return *this;
	}

	SInt3& SInt3::operator -= (const SInt3& _rhs)
	{
		this->x -= _rhs.x;
		this->y -= _rhs.y;
		this->z -= _rhs.z;
		return *this;
	}

	SInt3& SInt3::operator *= (const SInt3& _rhs)
	{
		this->x *= _rhs.x;
		this->y *= _rhs.y;
		this->z *= _rhs.z;
		return *this;
	}

	SInt3& SInt3::operator *= (const int& _rhs)
	{
		this->x *= _rhs;
		this->y *= _rhs;
		this->z *= _rhs;
		return *this;
	}

	SInt3& SInt3::operator /= (const SInt3& _rhs)
	{
		this->x /= _rhs.x;
		this->y /= _rhs.y;
		this->z /= _rhs.z;
		return *this;
	}

	SInt3& SInt3::operator /= (const int& _rhs)
	{
		this->x /= _rhs;
		this->y /= _rhs;
		this->z /= _rhs;
		return *this;
	}

	SInt3&	SInt3::operator =	(const SInt4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}



	int	SInt3::Get(const unsigned int _idx)
	{
		assert(_idx < array.size());
		return array[_idx];
	}

	void	SInt3::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}
}



const NMath::SInt3 operator + (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs)
{
	return NMath::SInt3(_lhs) += _rhs;
}

const NMath::SInt3 operator - (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs)
{
	return NMath::SInt3(_lhs) -= _rhs;
}

const NMath::SInt3 operator * (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs)
{
	return NMath::SInt3(_lhs) *= _rhs;
}

const NMath::SInt3 operator * (const NMath::SInt3& _lhs, const int& _rhs)
{
	return NMath::SInt3(_lhs) *= _rhs;
}

const NMath::SInt3 operator * (const int& _lhs, const NMath::SInt3& _rhs)
{
	return NMath::SInt3(_rhs) *= _lhs;
}

const NMath::SInt3 operator / (const NMath::SInt3& _lhs, const NMath::SInt3& _rhs)
{
	return NMath::SInt3(_lhs) /= _rhs;
}

const NMath::SInt3 operator / (const NMath::SInt3& _lhs, const int& _rhs)
{
	return NMath::SInt3(_lhs) /= _rhs;
}

const NMath::SInt3 operator / (const int& _lhs, const NMath::SInt3& _rhs)
{
	return NMath::SInt3(_rhs) /= _lhs;
}
