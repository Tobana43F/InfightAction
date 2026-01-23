#include	"Math_Matrix44.h"
#include	<assert.h>

using namespace DirectX;

namespace NMath {

	SMatrix44::SMatrix44()
	{
		SetIdentity();
	}

	SMatrix44::SMatrix44(const SMatrix44& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SMatrix44::SMatrix44(
		const SVector4& _vec1,
		const SVector4& _vec2,
		const SVector4& _vec3,
		const SVector4& _vec4)
	{
		vec[0] = _vec1;
		vec[1] = _vec2;
		vec[2] = _vec3;
		vec[3] = _vec4;
	}

	SMatrix44::SMatrix44(
		const float _e11, const float _e12, const float _e13, const float _e14,
		const float _e21, const float _e22, const float _e23, const float _e24,
		const float _e31, const float _e32, const float _e33, const float _e34,
		const float _e41, const float _e42, const float _e43, const float _e44)
	{
		e11 = _e11; e12 = _e12; e13 = _e13; e14 = _e14;
		e21 = _e21; e22 = _e22; e23 = _e23; e24 = _e24;
		e31 = _e31; e32 = _e32; e33 = _e33; e34 = _e34;
		e41 = _e41; e42 = _e42; e43 = _e43; e44 = _e44;
	}



	SMatrix44& SMatrix44::operator =  (const SMatrix44& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
		return *this;
	}

	SMatrix44& SMatrix44::operator += (const SMatrix44& _rhs)
	{
		mat += _rhs.mat;
		return *this;
	}

	SMatrix44& SMatrix44::operator -= (const SMatrix44& _rhs)
	{
		mat -= _rhs.mat;
		return *this;
	}

	SMatrix44& SMatrix44::operator *= (const SMatrix44& _rhs)
	{
		mat *= _rhs.mat;
		return *this;
	}

	SMatrix44& SMatrix44::operator *= (const float& _rhs)
	{
		mat *= _rhs;
		return *this;
	}

	SMatrix44& SMatrix44::operator /= (const float& _rhs)
	{
		mat /= _rhs;
		return *this;
	}



	float SMatrix44::Get(const unsigned int _y, const unsigned int _x) const
	{
		assert(_y < flt.size());
		assert(_x < flt[_y].size());
		return flt[_y][_x];
	}

	const SVector4& SMatrix44::GetRow(const unsigned int _row) const
	{
		assert(_row < flt.size());
		return vec[_row];
	}

	void SMatrix44::SetRow(
		const unsigned int		_index,
		const NMath::SVector4&	_src)
	{
		assert(_index < vec.size());
		vec[_index] = _src;
	}

	void SMatrix44::SetRow(
		const unsigned int		_index,
		const NMath::SVector3&	_src)
	{
		assert(_index < vec.size());
		vec[_index].vec3 = _src;
	}

	void SMatrix44::SetZero()
	{
		memset(this, 0, sizeof(*this));
	}

	void SMatrix44::SetIdentity()
	{
		SetZero();
		e11 = 1.0f;
		e22 = 1.0f;
		e33 = 1.0f;
		e44 = 1.0f;
	}

	void SMatrix44::Inverse()
	{
		GetInverse(*this);
	}

	void SMatrix44::Transpose()
	{
		GetTranspose(*this);
	}

	void SMatrix44::GetInverse(SMatrix44& _dest) const
	{
		_dest.mat = XMMatrixInverse(nullptr, this->mat);
	}

	void SMatrix44::GetTranspose(SMatrix44& _dest) const
	{
		_dest.mat = XMMatrixTranspose(_dest.mat);
	}

	void SMatrix44::Decompose(
		SVector3*		_destScale,
		SQuaternion*	_destRotation,
		SVector3*		_destTrans) const
	{
		SVector3	scale;
		SQuaternion	rotation;
		SVector3	trans;

		XMMatrixDecompose(
			&scale.simd,
			&rotation.vec4.simd,
			&trans.simd,
			mat);

		// ポインタがせっていされている時のみ値を取得する
		if (_destScale != nullptr) { *_destScale = scale; }
		if (_destRotation != nullptr) { *_destRotation = rotation; }
		if (_destTrans != nullptr) { *_destTrans = trans; }
	}
}



const NMath::SMatrix44 operator + (const NMath::SMatrix44& _lhs, const NMath::SMatrix44& _rhs)
{
	return NMath::SMatrix44(_lhs) += _rhs;
}

const NMath::SMatrix44 operator - (const NMath::SMatrix44& _lhs, const NMath::SMatrix44& _rhs)
{
	return NMath::SMatrix44(_lhs) -= _rhs;
}

const NMath::SMatrix44 operator * (const NMath::SMatrix44& _lhs, const NMath::SMatrix44& _rhs)
{
	return NMath::SMatrix44(_lhs) *= _rhs;
}

const NMath::SMatrix44 operator * (const NMath::SMatrix44& _lhs, const float& _rhs)
{
	return NMath::SMatrix44(_lhs) *= _rhs;
}

const NMath::SMatrix44 operator * (const float& _lhs, const NMath::SMatrix44& _rhs)
{
	return NMath::SMatrix44(_rhs) *= _lhs;
}

const NMath::SMatrix44 operator / (const NMath::SMatrix44& _lhs, const float& _rhs)
{
	return NMath::SMatrix44(_lhs) /= _rhs;
}

const NMath::SMatrix44 operator / (const float& _lhs, const NMath::SMatrix44& _rhs)
{
	return NMath::SMatrix44(_rhs) /= _lhs;
}
