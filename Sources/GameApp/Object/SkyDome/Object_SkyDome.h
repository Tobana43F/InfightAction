#pragma once

#include	"Object\Object_GameObject.h"
#include	"Model\Model_Object.h"

namespace NObject {

	//!	@brief	スカイドームを管理するクラス
	class CSkyDome : public CGameObject {
	public:

	protected:
		virtual void	LoadProc() override;
		virtual void	ReleaseProc() override;
		virtual void	InitProc() override;
		virtual void	DestroyProc() override;
		virtual void	UpdateProc() override;
		virtual void	RenderProc() override;

	private:
		NModel::CObject	m_modelSkyDome;	// 描画用スカイドーム

	public:
		CSkyDome() : CGameObject() { }
		virtual ~CSkyDome() { }
	};
}
