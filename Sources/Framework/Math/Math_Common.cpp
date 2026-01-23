#include	"Math_Common.h"
#include	"Math_Util.h"

using namespace DirectX;

namespace NMath {

	float	ToRad(float _deg)
	{
		return XMConvertToRadians(_deg);
	}

	float	ToDeg(float _rad)
	{
		return XMConvertToDegrees(_rad);
	}

	void MakeMatrixTranslation(SMatrix44& _rDest, const float _x, const float _y, const float _z)
	{
		_rDest.SetIdentity();
		_rDest.mat = XMMatrixTranslation(_x, _y, _z);
	}

	void MakeMatrixTranslation(SMatrix44& _rDest, const SVector3& _rPos)
	{
		_rDest.SetIdentity();
		MakeMatrixTranslation(_rDest, _rPos.x, _rPos.y, _rPos.z);
	}

	void MakeMatrixRotationX(SMatrix44& _rDest, const float _deg)
	{
		_rDest.SetIdentity();
		_rDest.mat = XMMatrixRotationX(ToRad(_deg));
	}

	void MakeMatrixRotationY(SMatrix44& _rDest, const float _deg)
	{
		_rDest.SetIdentity();
		_rDest.mat = XMMatrixRotationY(ToRad(_deg));
	}

	void MakeMatrixRotationZ(SMatrix44& _rDest, const float _deg)
	{
		_rDest.SetIdentity();
		_rDest.mat = XMMatrixRotationZ(ToRad(_deg));
	}

	void MakeMatrixRotation(SMatrix44& _rDest, const float _degX, const float _degY, const float _degZ)
	{
		SMatrix44 x, y, z;
		MakeMatrixRotationX(x, _degX);
		MakeMatrixRotationY(y, _degY);
		MakeMatrixRotationZ(z, _degZ);
		_rDest = x * y * z;
	}

	void MakeMatrixScaling(SMatrix44& _rDest, const SVector3& _scale)
	{
		MakeMatrixScaling(_rDest, _scale.x, _scale.y, _scale.z);
	}

	void MakeMatrixScaling(SMatrix44& _rDest, const float _x, const float _y, const float _z)
	{
		_rDest.SetZero();
		_rDest.e11 = _x;
		_rDest.e22 = _y;
		_rDest.e33 = _z;
		_rDest.e44 = 1;
	}

	void MakeMatrixScaling(SMatrix44& _rDest, const float _scale)
	{
		_rDest.SetZero();
		_rDest.e11 = _scale;
		_rDest.e22 = _scale;
		_rDest.e33 = _scale;
		_rDest.e44 = 1;
	}

	void MakeMatrixPerspectiveFovLH(SMatrix44& _rDest, const float _fovAngleYRad, const float _aspectWDivH, const float _nearZ, const float _farZ)
	{
		_rDest.mat = XMMatrixPerspectiveFovLH(_fovAngleYRad, _aspectWDivH, _nearZ, _farZ);
	}

	void MakeMatrixLookAtLH(SMatrix44& _rDest, const SVector3& _rEyePosition, const  SVector3& _rFocusPosition, const  SVector3& _rUpDirection)
	{
		_rDest.mat = XMMatrixLookAtLH(
			_rEyePosition.simd,
			_rFocusPosition.simd,
			_rUpDirection.simd);
	}

	void MakeMatrixOrthographicLH(SMatrix44& _rDest, const float _left, const float _top, const float _right, const float _bottom, const float _near, const float _far)
	{
		_rDest.mat = XMMatrixOrthographicOffCenterLH(_left, _right, _bottom, _top, _near, _far);
	}

	void MakeMatrixOrthographicLH(SMatrix44& _rDest, const float _width, const float _height, const float _near, const float _far)
	{
		_rDest.mat = XMMatrixOrthographicLH(_width, _height, _near, _far);
	}

	void MakeMatrixQuaternion(SMatrix44& _rDest, const SQuaternion& _rSrc)
	{
		_rDest.mat = XMMatrixRotationQuaternion(_rSrc.vec4.simd);
	}


	void Cross(SVector2& _rDest, const SVector2& _rVec1, const SVector2& _rVec2)
	{
		_rDest.simd = XMVector2Cross(_rVec1.simd, _rVec2.simd);
	}

	void Cross(SVector3& _rDest, const SVector3& _rVec1, const SVector3& _rVec2)
	{
		_rDest.simd = XMVector3Cross(_rVec1.simd, _rVec2.simd);
	}

	float Dot(const SVector2& _rVec1, const SVector2& _rVec2)
	{
		SVector2	ans;
		ans.simd = XMVector2Dot(_rVec1.simd, _rVec2.simd);
		return ans.x;
	}

	float Dot(const SVector3& _rVec1, const SVector3& _rVec2)
	{
		SVector3	ans;
		ans.simd = XMVector3Dot(_rVec1.simd, _rVec2.simd);
		return ans.x;
	}

	float Dot(const SVector4& _rVec1, const SVector4& _rVec2)
	{
		SVector4	ans;
		ans.simd = XMVector4Dot(_rVec1.simd, _rVec2.simd);
		return ans.x;
	}

	float Dot(const SQuaternion& _rQt1, const SQuaternion& _rQt2)
	{
		SQuaternion	ans;
		ans.vec4.simd = XMQuaternionDot(_rQt1.vec4.simd, _rQt2.vec4.simd);
		return ans.x;
	}


	void TransformVector(SVector2& _rInOut, const SMatrix44& _rMatrix)
	{
		_rInOut.simd = XMVector2Transform(_rInOut.simd, _rMatrix.mat);
	}

	void TransformVector(SVector3& _rInOut, const SMatrix44& _rMatrix)
	{
		_rInOut.simd = XMVector3Transform(_rInOut.simd, _rMatrix.mat);
	}

	void TransformVector(SVector4& _rInOut, const SMatrix44& _rMatrix)
	{
		_rInOut.simd = XMVector4Transform(_rInOut.simd, _rMatrix.mat);
	}

	void TransformVector(SVector2& _rDest, const SVector2& _rSrc, const SMatrix44& _rMatrix)
	{
		_rDest.simd = XMVector2Transform(_rSrc.simd, _rMatrix.mat);
	}

	void TransformVector(SVector3& _rDest, const SVector3& _rSrc, const SMatrix44& _rMatrix)
	{
		_rDest.simd = XMVector3Transform(_rSrc.simd, _rMatrix.mat);
	}

	void TransformVector(SVector4& _rDest, const SVector4& _rSrc, const SMatrix44& _rMatrix)
	{
		_rDest.simd = XMVector4Transform(_rSrc.simd, _rMatrix.mat);
	}

	void Lerp(SVector3&	_rDest, const SVector3&	_rStart, const SVector3& _rEnd, const float _ratio)
	{
		_rDest.simd = XMVectorLerp(_rStart.simd, _rEnd.simd, _ratio);
	}


	void MakeQuaternionRotationArc(
		SQuaternion&	_dest,
		const SVector3&	_v0,
		const SVector3&	_v1,
		float*			_pDestAngleRadOut)
	{
		// ベクトルを正規化する
		SVector3	v0, v1;
		_v0.GetNormal(v0);
		_v1.GetNormal(v1);

		SVector3	axis;

		// 回転軸を求める
		Cross(axis, v0, v1);

		// 誤差で値が無効になることを防止
		float dot = Dot(v0, v1);
		if (dot > 1.0f) {
			dot = 1.0f;
		}
		else if (dot < -1.0f) {
			dot = -1.0f;
		}

		// 回転角度の取得先がある場合はそこに設定
		if (_pDestAngleRadOut != nullptr) {
			*_pDestAngleRadOut = dot;
		}

		// クォータニオンのnan防止処理
		SQuaternion	qt;
		float s = (float)sqrt((1 + dot) * 2);

		qt.x = axis.x / s;
		qt.y = axis.y / s;
		qt.z = axis.z / s;
		qt.w = s / 2.0f;

		_dest = qt;
	}

	void MakeQuaternionMatrix(SQuaternion& _rDest, const SMatrix44& _rSrc)
	{
		_rDest.vec4.simd = XMQuaternionRotationMatrix(_rSrc.mat);
	}

	void Slerp(SQuaternion& _rDest, const SQuaternion& _rStart, const SQuaternion& _rEnd, const float _ratio)
	{
		_rDest.vec4.simd = XMQuaternionSlerp(_rStart.vec4.simd, _rEnd.vec4.simd, _ratio);
	}

	float ArcCos(float _cosTheta)
	{
		if (_cosTheta > 1.0f)
			_cosTheta = 1.0f;
		else if (_cosTheta < -1.0f)
			_cosTheta = -1.0f;

		return acos(_cosTheta);
	}



	void MakeMatrixRotation(
		SMatrix44&	_dest,
		SVector3&	_x,
		SVector3&	_y,
		SVector3&	_z,
		const bool	_isNormalized)
	{
		// 全てのベクトルを正規化する
		if (!_isNormalized) {
			_x.Normalize();
			_y.Normalize();
			_z.Normalize();
		}

		_dest.SetIdentity();
		_dest.e11 = _x.x; _dest.e12 = _x.y; _dest.e13 = _x.z; _dest.e14 = 0;
		_dest.e21 = _y.x; _dest.e22 = _y.y; _dest.e23 = _y.z; _dest.e24 = 0;
		_dest.e31 = _z.x; _dest.e32 = _z.y; _dest.e33 = _z.z; _dest.e34 = 0;
		_dest.e41 = 0;    _dest.e42 = 0;    _dest.e43 = 0;    _dest.e44 = 1	;
	}

	void MakeMatrixLookAt(
		SMatrix44&			_dest,
		const SVector3&		_pos,
		const SVector3&		_targetPos,
		const SVector3&		_upDir)
	{
		// 自分の座標から注視点に向かうベクトルを計算する
		// (= Z軸ベクトル)
		SVector3 vecZ = _targetPos - _pos;
		vecZ.Normalize();

		// X軸ベクトルを求める
		SVector3 vecX;
		Cross(vecX, _upDir, vecZ);
		vecX.Normalize();

		// Y軸ベクトルを求める
		SVector3 vecY;
		Cross(vecY, vecZ, vecX);
		vecY.Normalize();

		// 行列に設定
		MakeMatrixRotation(_dest, vecX, vecY, vecZ);
	}

	void MakeMatrixLookDirection(
		SMatrix44&			_dest,
		const SVector3&		_pos,
		const SVector3&		_dir,
		const SVector3&		_upDir)
	{
		SVector3	target = _pos + _dir;
		MakeMatrixLookAt(_dest, _pos, target, _upDir);
	}
}
