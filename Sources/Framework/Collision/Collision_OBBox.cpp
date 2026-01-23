#include	"Collision_OBBox.h"

#include	"Math\Math_Common.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"

namespace NCollision {

	CTransformOBBox::CTransformOBBox()
	{
	}

	CTransformOBBox::~CTransformOBBox()
	{
	}

	void CTransformOBBox::SetData(
		const float3&			_localCenterPos,
		const float3&			_size,
		const NMath::SMatrix44&	_matRot)
	{
		m_localOBBox.centerPos = _localCenterPos;
		m_localOBBox.size = _size;
		
		NMath::SMatrix44	matRot;
		matRot = _matRot;
		matRot.vec[3] = float4(0, 0, 0, 1);
		m_localOBBox.axisDir = matRot;

		m_worldOBBox = m_localOBBox;
	}

	void CTransformOBBox::Update(const NMath::SMatrix44& matParent)
	{
		// 中心座標をワールド座標系に変換
		NMath::TransformVector(
			m_worldOBBox.centerPos, 
			m_localOBBox.centerPos, 
			matParent);

		// 回転ベクトルをワールド座標系に変換
		NMath::SMatrix44 matRot = m_localOBBox.axisDir * matParent;
		matRot.vec[3].SetZero();
		matRot.e44 = 1;
		m_worldOBBox.axisDir = m_localOBBox.axisDir * matRot;
	}

}
