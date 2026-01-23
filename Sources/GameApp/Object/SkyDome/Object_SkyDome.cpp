#include	"Object\SkyDome\Object_SkyDome.h"

#include	"Debug\Debug_Util.h"

namespace NObject {

	void CSkyDome::LoadProc()
	{
		CGameObject::LoadProc();
		m_modelSkyDome.Load("SkyDome/SkyDome.fbx");
	}

	void CSkyDome::ReleaseProc()
	{
		CGameObject::ReleaseProc();
	}


	void CSkyDome::InitProc()
	{
		CGameObject::InitProc();
	}

	void CSkyDome::DestroyProc()
	{
		CGameObject::DestroyProc();
	}

	void CSkyDome::UpdateProc()
	{
		CGameObject::UpdateProc();
	}

	void CSkyDome::RenderProc()
	{
		CGameObject::RenderProc();

		// ステージを描画
		NMath::SMatrix44 matWorld;
		GetTransform().CalcWorldMatrix(matWorld);
		m_modelSkyDome.RenderDebug(matWorld);
	}

	
}