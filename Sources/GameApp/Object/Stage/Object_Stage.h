#pragma once

#include	"Object\Object_GameObject.h"

#include	"Object\SkyDome\Object_SkyDome.h"	// 内包するオブジェクト
#include	"Model\Model_Object.h"
#include	"Collision\Collision_Mesh.h"
#include	"Sound\Sound_Common.h"

namespace NObject {


	//!	@brief		ステージオブジェクトクラス
	//!	@details	描画用ステージモデル、ステージのあたり判定メッシュを管理するオブジェクト
	//!	@todo		もし、複数パターンのステージを作る場合は、
	//!				このクラスを継承して読み込むモデルを変更するようにする
	class CStage : public CGameObject {
	public:

		//!	@brief	当たり判定の設定タイミングをはっきりさせるために、
		//!			初期化とは別の処理で行う
		void SetStageCollision();

	protected:
		virtual void	LoadProc() override;
		virtual void	ReleaseProc() override;
		virtual void	InitProc() override;
		virtual void	DestroyProc() override;
		virtual void	UpdateProc() override;
		virtual void	RenderProc() override;

		// 描画用オブジェクト
		NModel::CObject			m_modelGround;	// 地面描画用モデル
		NObject::CSkyDome		m_skyDome;		// スカイドーム

		// 地形とのあたり判定用メッシュ
		NCollision::CMesh		m_colGround;
		NCollision::CMesh		m_colWall;

		// 使用するBGM
		NSound::ELabel			m_sndBGMLabel;

	public:
		CStage();
		virtual ~CStage();
	};
}
