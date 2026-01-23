#include	"Util_GameCommon.h"

#include	"DirectX\DirectX_DebugFont.h"
#include	"Math\Math_Type.h"
#include	"Math\Math_Common.h"

namespace NUtil {

	void PrintVector(
		const float _x,
		const float _y,
		const NMath::SVector2&	_rSrc)
	{
		NDirectX::PrintDebugFontF(
			_x, _y,
			"[%9.4f, %9.4f]",
			_rSrc.x, _rSrc.y);
	}

	void PrintVector(
		const float _x,
		const float _y,
		const NMath::SVector3&	_rSrc)
	{
		NDirectX::PrintDebugFontF(
			_x, _y,
			"[%9.4f, %9.4f, %9.4f]",
			_rSrc.x, _rSrc.y, _rSrc.z);
	}

	void PrintVector(
		const float _x,
		const float _y,
		const NMath::SVector4&	_rSrc)
	{
		NDirectX::PrintDebugFontF(
			_x, _y,
			"[%9.4f, %9.4f, %9.4f, %9.4f]",
			_rSrc.x, _rSrc.y, _rSrc.z, _rSrc.w);
	}

	void PrintMatrix(
		const float2&			_pos,
		const NMath::SMatrix44&	_mat)
	{
		float	h = 20;
		for (int i = 0; i < 4; ++i) {
			float2 pos = _pos;
			pos.y += h * i;
			NDirectX::PrintDebugFontF(
				pos.x, pos.y,
				"[%6.2f  %6.2f  %6.2f  %6.2f]",
				_mat.vec[i].x, _mat.vec[i].y, _mat.vec[i].z, _mat.vec[i].w);
		}
	}

	void ToVectorFromCamera(
		NMath::SVector3&		_dest,
		const NMath::SVector3&	_src,
		const NCamera::EManagerIndex camIndex)
	{
		// ’·‚³‚ª‚O‚Ìê‡‚Í–³Ž‹
		if (_src.GetLengthSquare() < FLT_EPSILON) {
			_dest.SetZero();
			return;
		}

		// ƒrƒ…[•ÏŠ·s—ñ‚Ì‹ts—ñ‚ð‹‚ß‚é
		NMath::SMatrix44 matCameraInv;
		NCamera::GetViewMatrix(matCameraInv);
		matCameraInv.Transpose();

		// ƒxƒNƒgƒ‹‚ð•ÏŠ·
		_dest = _src;
		NMath::TransformVector(_dest, matCameraInv);
	}

	void ToVectorFromCameraXZ(
		NMath::SVector3&		_dest,
		const NMath::SVector3&	_src,
		const NCamera::EManagerIndex camIndex)
	{
		// ’·‚³‚ª‚O‚Ìê‡‚Í–³Ž‹
		if (_src.GetLengthSquare() < FLT_EPSILON) {
			_dest.SetZero();
			return;
		}

		// ƒrƒ…[•ÏŠ·s—ñ‚Ì‹ts—ñ‚ð‹‚ß‚é
		NMath::SMatrix44 matCameraInv;
		NCamera::GetViewMatrix(matCameraInv);
		matCameraInv.Transpose();

		//----
		// ƒJƒƒ‰‚ÌŒü‚«s—ñ‚ðì¬
		const float3 mask(1, 0, 1);
		const float3 camForward = matCameraInv.GetRow(2);
		const float3 camRight = matCameraInv.GetRow(0);
		float3 camForwardXZ = camForward * mask;
		float3 camRightXZ = camRight * mask;

		// Œü‚«‚ð³‹K‰»
		camForwardXZ.Normalize();
		camRightXZ.Normalize();

		// “ü—Í•ûŒü‚ÆƒJƒƒ‰•ûŒü‚©‚çˆÚ“®•ûŒü‚ðŒvŽZ
		_dest =
			camForwardXZ * _src.y +
			camRightXZ * _src.x;

		// •ûŒü‚ð³‹K‰»
		_dest.Normalize();
	}

}