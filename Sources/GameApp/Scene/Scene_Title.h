#pragma once

#include	"Scene\Scene_Base.h"

#include	"Asset\Factory\Asset_TextureFactory.h"
#include	"Object\Object_GameObject.h"
#include	"Polygon\Polygon_Object2D.h"
#include	"Object\Stage\Object_Stage.h"
#include	"Object\UI\Object_ScreenFade.h"

#include	"Model\Model_Object.h"
#include	"Sound\Sound_Common.h"

namespace NScene {

	//!	@brief	プログラム中のテスト用のアセットを再読み込みするシーン
	//!	@note	アセットの解放と、元のシーンへの遷移を行う
	class CTitle : public CBase {
	public:

		virtual void Load() override;
		virtual void Init() override;
		virtual void Destroy() override;
		virtual void Release() override;

		virtual void Update() override;
		virtual void Render() override;

	private:

		NAsset::CTextureFactory::SPAsset	m_pTexLogo;
		NAsset::CTextureFactory::SPAsset	m_pTexBack;

		NPolygon::CObject2D					m_polyLogo;
		NObject::CGameObject				m_objLogo;

		NPolygon::CObject2D					m_polyBack;
		NObject::CGameObject				m_objBack;

		bool								m_isSceneReady;
		bool								m_isSceneChangeReady;
		NObject::CScreenFade				m_fade;

		NModel::CObject						m_model;

		NSound::ELabel						m_sndBGMLabel;

	public:
		CTitle();
		virtual ~CTitle();
	};

}