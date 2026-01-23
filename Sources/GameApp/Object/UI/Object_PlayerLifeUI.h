#pragma once

#include	"Object\Object_GameObject.h"

#include	"Asset\Factory\Asset_TextureFactory.h"
#include	"Object\Actor\Player\Object_Player.h"
#include	"Polygon\Polygon_Object2D.h"


namespace NObject {

	//!	@brief	プレイヤーの体力を画面上に表示するUI
	class CPlayerLifeUI : public CGameObject {
	public:

	protected:

		//!	@brief	参照する値を設定
		virtual void LoadProc() override;

		//!	@brief	初期化
		virtual void InitProc() override;

		//!	@brief	体力UIの更新
		virtual void UpdateProc() override;

		//!	@brief	UIの描画
		virtual void RenderProc() override;

	private:

		const CPlayer*			m_pPlayer;	// 参照するプレイヤー

		NAsset::CTextureFactory::SPAsset	m_pTexLifeBar;
		NPolygon::CObject2D					m_polyLifeBar;

		NAsset::CTextureFactory::SPAsset	m_pTexLifeBarFrame;
		NPolygon::CObject2D					m_polyLifeBarFrame;

		//NAsset::CTextureFactory::SPAsset	m_pTexLifeBarBack;
		NPolygon::CObject2D					m_polyLifeBarBack;


	public:
		CPlayerLifeUI();
		~CPlayerLifeUI();
	};

}