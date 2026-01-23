#include	"Math_Int4.h"

#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SInt4::SInt4()
	{
		memset(this, 0, sizeof(*this));
	}

	SInt4::SInt4(const SInt4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SInt4::SInt4(
		const int _x,
		const int _y,
		const int _z,
		const int _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}



	SInt4& SInt4::operator =  (const SInt4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SInt4& SInt4::operator += (const SInt4& _rhs)
	{
		this->x += _rhs.x;
		this->y += _rhs.y;
		this->z += _rhs.z;
		this->w += _rhs.w;
		return *this;
	}

	SInt4& SInt4::operator -= (const SInt4& _rhs)
	{
		this->x -= _rhs.x;
		this->y -= _rhs.y;
		this->z -= _rhs.z;
		this->w -= _rhs.w;
		return *this;
	}

	SInt4& SInt4::operator *= (const SInt4& _rhs)
	{
		this->x *= _rhs.x;
		this->y *= _rhs.y;
		this->z *= _rhs.z;
		this->w *= _rhs.w;
		return *this;
	}

	SInt4& SInt4::operator *= (const int& _rhs)
	{
		this->x *= _rhs;
		this->y *= _rhs;
		this->z *= _rhs;
		this->w *= _rhs;
		return *this;
	}

	SInt4& SInt4::operator /= (const SInt4& _rhs)
	{
		this->x /= _rhs.x;
		this->y /= _rhs.y;
		this->z /= _rhs.z;
		this->w /= _rhs.w;
		return *this;
	}

	SInt4& SInt4::operator /= (const int& _rhs)
	{
		this->x /= _rhs;
		this->y /= _rhs;
		this->z /= _rhs;
		this->w /= _rhs;
		return *this;
	}



	int	SInt4::Get(const unsigned int _idx)
	{
		assert(_idx < array.size());
		return array[_idx];
	}

	void	SInt4::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}
}



const NMath::SInt4 operator + (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs)
{
	return NMath::SInt4(_lhs) += _rhs;
}

const NMath::SInt4 operator - (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs)
{
	return NMath::SInt4(_lhs) -= _rhs;
}

const NMath::SInt4 operator * (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs)
{
	return NMath::SInt4(_lhs) *= _rhs;
}

const NMath::SInt4 operator * (const NMath::SInt4& _lhs, const int& _rhs)
{
	return NMath::SInt4(_lhs) *= _rhs;
}

const NMath::SInt4 operator * (const int& _lhs, const NMath::SInt4& _rhs)
{
	return NMath::SInt4(_rhs) *= _lhs;
}

const NMath::SInt4 operator / (const NMath::SInt4& _lhs, const NMath::SInt4& _rhs)
{
	return NMath::SInt4(_lhs) /= _rhs;
}

const NMath::SInt4 operator / (const NMath::SInt4& _lhs, const int& _rhs)
{
	return NMath::SInt4(_lhs) /= _rhs;
}

const NMath::SInt4 operator / (const int& _lhs, const NMath::SInt4& _rhs)
{
	return NMath::SInt4(_rhs) /= _lhs;
}
