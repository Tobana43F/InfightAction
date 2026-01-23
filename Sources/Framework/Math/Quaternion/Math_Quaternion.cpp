#include	"Math_Quaternion.h"

#include	<memory>
#include	<float.h>
#include	"..\Matrix\Math_Matrix44.h"

namespace NMath {

	SQuaternion::SQuaternion()
	{
		memset(this, 0, sizeof(SQuaternion));
	}

	SQuaternion::SQuaternion(const SQuaternion& _rhs)
	{
		memcpy(this, &_rhs, sizeof(SQuaternion));
	}

	SQuaternion& SQuaternion::operator = (const SQuaternion& _rhs)
	{
		memcpy(this, &_rhs, sizeof(SQuaternion));
		return *this;
	}

	SQuaternion& SQuaternion::operator *= (const SQuaternion& _rhs)
	{
		vec4.simd = DirectX::XMQuaternionMultiply(vec4.simd, _rhs.vec4.simd);
		return *this;
	}

	//----------------------------------------------------------

	void SQuaternion::SetZero()
	{
		memset(this, 0, sizeof(SQuaternion));
	}

	void SQuaternion::SetIdentity()
	{
		vec4.simd = DirectX::XMQuaternionIdentity();
	}

	void SQuaternion::Normalize()
	{
		GetNormalize(*this);
	}

	void SQuaternion::Inverse()
	{
		GetInverse(*this);
	}

	void SQuaternion::GetNormalize(SQuaternion& _rDest)
	{
		DirectX::XMQuaternionNormalize(_rDest.vec4.simd);
	}

	void SQuaternion::GetInverse(SQuaternion& _rDest)
	{
		DirectX::XMQuaternionInverse(_rDest.vec4.simd);
	}
}

const NMath::SQuaternion operator * (const NMath::SQuaternion& _lhs, const NMath::SQuaternion& _rhs)
{
	return NMath::SQuaternion(_lhs) *= _rhs;
}