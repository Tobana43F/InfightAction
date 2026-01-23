#pragma once

#include	"Scene\Scene_Base.h"

#include	"Asset\Factory\Asset_TextureFactory.h"
#include	"Object\Object_GameObject.h"
#include	"Polygon\Polygon_Object2D.h"
#include	"Object\Stage\Object_Stage.h"
#include	"Sound\Sound_Common.h"
#include	"Object\UI\Object_ScreenFade.h"

namespace NScene {

	//!	@brief	プログラム中のテスト用のアセットを再読み込みするシーン
	//!	@note	アセットの解放と、元のシーンへの遷移を行う
	class CResult : public CBase {
	public:

		virtual void Load() override;
		virtual void Init() override;
		virtual void Destroy() override;
		virtual void Release() override;

		virtual void Update() override;
		virtual void Render() override;

	private:

		NAsset::CTextureFactory::SPAsset	m_pTexText;
		NAsset::CTextureFactory::SPAsset	m_pTexBackClear;
		NAsset::CTextureFactory::SPAsset	m_pTexBackGameOver;

		NPolygon::CObject2D					m_polyText;
		NObject::CGameObject				m_objText;

		NPolygon::CObject2D					m_polyBack;
		NObject::CGameObject				m_objBack;

		NSound::ELabel						m_sndBGMLabel;

		bool								m_isSceneReady;
		bool								m_isSceneChangeReady;
		NObject::CScreenFade				m_fade;


	public:
		CResult();
		virtual ~CResult();
	};

}