#include	"Object_Effect.h"

namespace NObject {

	CEffect::CEffect() : 
		m_enable(false)
	{

	}

	CEffect::~CEffect()
	{

	}

	void CEffect::Play(const float3& _pos, const NMath::SMatrix44& _matRot)
	{
		GetTransform().SetPos(_pos);
		GetTransform().SetRot(_matRot);

		m_enable = true;
		m_model.ChangeAnimation(ANM_EFFECT1, false, 0, 1);
	}

	//----------------------------------------------------------

	void CEffect::LoadProc()
	{
		m_model.Load("Effect/Jump/Model.fbx");
	}

	void CEffect::ReleaseProc()	
	{

	}

	void CEffect::InitProc()
	{


	}

	void CEffect::UpdateProc()
	{
		// エフェクト再生中でない場合は何もしない
		if (!m_enable) { return; }

		m_model.Update();

		if (m_model.GetIsAnimationEnd()) {
			m_enable = false;
		}
	}

	void CEffect::RenderProc()
	{
		// エフェクト再生中でない場合は何もしない
		if (!m_enable) { return; }

		NMath::SMatrix44	mat;
		GetTransform().CalcWorldMatrix(mat);	
		m_model.RenderDebugAlpha(mat);
	}
}