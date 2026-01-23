#include	"Camera_OperatorBase.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"Math\Math_Common.h"

namespace NCamera {

	COperatorBase::COperatorBase() : 
		m_cameraIndex(EManagerIndex::MAIN)
	{
		// 適当な値で初期化しておく
		m_cameraViewData.pos = float3(0, 0, -1);
		m_cameraViewData.foc = float3(0, 0, 0);
		m_cameraViewData.up = float3(0, 1, 0);
	}

	COperatorBase::~COperatorBase()
	{

	}

	void COperatorBase::Update()
	{
		Operate();
		Apply();
	}

	//----------------------------------------------------------
	void COperatorBase::Apply()
	{
		// ビュー変換行列を作成
		NMath::SMatrix44	matView;
		NMath::MakeMatrixLookAtLH(
			matView,
			m_cameraViewData.pos,
			m_cameraViewData.foc,
			m_cameraViewData.up);

		NCamera::RegisterViewMatrix(matView, m_cameraIndex);
	}
}
