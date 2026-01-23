#include	"Math_Int2.h"

#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SInt2::SInt2()
	{
		memset(this, 0, sizeof(*this));
	}

	SInt2::SInt2(const SInt2& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SInt2::SInt2(const SInt3& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SInt2::SInt2(const SInt4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SInt2::SInt2(
		const int _x,
		const int _y)
	{
		x = _x;
		y = _y;
	}



	SInt2& SInt2::operator =  (const SInt2& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SInt2& SInt2::operator += (const SInt2& _rhs)
	{
		this->x += _rhs.x;
		this->y += _rhs.y;
		return *this;
	}

	SInt2& SInt2::operator -= (const SInt2& _rhs)
	{
		this->x -= _rhs.x;
		this->y -= _rhs.y;
		return *this;
	}

	SInt2& SInt2::operator *= (const SInt2& _rhs)
	{
		this->x *= _rhs.x;
		this->y *= _rhs.y;
		return *this;
	}

	SInt2& SInt2::operator *= (const int& _rhs)
	{
		this->x *= _rhs;
		this->y *= _rhs;
		return *this;
	}

	SInt2& SInt2::operator /= (const SInt2& _rhs)
	{
		this->x /= _rhs.x;
		this->y /= _rhs.y;
		return *this;
	}

	SInt2& SInt2::operator /= (const int& _rhs)
	{
		this->x /= _rhs;
		this->y /= _rhs;
		return *this;
	}

	SInt2& SInt2::operator =  (const SInt3& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SInt2& SInt2::operator =  (const SInt4& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}



	int	SInt2::Get(const unsigned int _idx)
	{
		assert(_idx < array.size());
		return array[_idx];
	}

	void	SInt2::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}
}



const NMath::SInt2 operator + (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs)
{
	return NMath::SInt2(_lhs) += _rhs;
}

const NMath::SInt2 operator - (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs)
{
	return NMath::SInt2(_lhs) -= _rhs;
}

const NMath::SInt2 operator * (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs)
{
	return NMath::SInt2(_lhs) *= _rhs;
}

const NMath::SInt2 operator * (const NMath::SInt2& _lhs, const int& _rhs)
{
	return NMath::SInt2(_lhs) *= _rhs;
}

const NMath::SInt2 operator * (const int& _lhs, const NMath::SInt2& _rhs)
{
	return NMath::SInt2(_rhs) *= _lhs;
}

const NMath::SInt2 operator / (const NMath::SInt2& _lhs, const NMath::SInt2& _rhs)
{
	return NMath::SInt2(_lhs) /= _rhs;
}

const NMath::SInt2 operator / (const NMath::SInt2& _lhs, const int& _rhs)
{
	return NMath::SInt2(_lhs) /= _rhs;
}

const NMath::SInt2 operator / (const int& _lhs, const NMath::SInt2& _rhs)
{
	return NMath::SInt2(_rhs) /= _lhs;
}
