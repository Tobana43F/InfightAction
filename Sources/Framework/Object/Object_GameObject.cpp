#include	"Object_GameObject.h"

#include	"Debug\Debug_Util.h"

namespace NObject {

	CGameObject::CGameObject() :
		m_isActive(true),
		m_objState(EObjectState::LOAD),
		m_transform(*this)
	{

	}


	CGameObject::~CGameObject()
	{
		Destroy();
		ReleaseProc();
	}

	void CGameObject::Init()
	{
		if (m_objState != EObjectState::LOAD) {
			m_objState = EObjectState::INIT;
		}
	}

	void CGameObject::Destroy()
	{
		if (m_objState == EObjectState::UPDATE) {
			DestroyProc();
			m_objState = EObjectState::INIT;
		}
	}


	void CGameObject::Update()
	{
		switch (m_objState) {
			case EObjectState::LOAD:
				LoadProc();
			case EObjectState::INIT:
				InitProc();
			case EObjectState::UPDATE:
				// 有効状態の時のみ更新する
				if (m_isActive) {
					UpdateFastProc();
					UpdateProc();
					UpdateLateProc();
				}
				break;
		}
	}

	void CGameObject::Render()
	{
		// 有効状態ではないときは描画を行わない
		if (!m_isActive) {
			return;
		}

		// 座標と方向を描画
	#ifdef _GAME_DEBUG
		NMath::SMatrix44 mat;
		GetTransform().CalcWorldMatrix(mat);
		NDebug::RenderAxisDir(mat, 1.0f);
	#endif

		// 継承後の描画
		RenderProc();
		RenderLateProc();
	}

}

