#include	"Scene_Result.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"Game\Manager\Game_GlobalDataManager.h"
#include	"System\System_Def.h"
#include	"Input\Input_Game.h"

namespace NScene {

	CResult::CResult() : 
		m_sndBGMLabel(NSound::ELabel::JINGLE_RESULT), 
		m_isSceneReady(false),
		m_isSceneChangeReady(false)

	{

	}

	CResult::~CResult()
	{

	}

	void CResult::Load()
	{
		CBase::Load();

		m_pTexText = NAsset::CTextureFactory::GetInst().Load("Result/ResultText.tga");
		m_pTexBackClear = NAsset::CTextureFactory::GetInst().Load("Result/BackGroundClear.tga");
		m_pTexBackGameOver = NAsset::CTextureFactory::GetInst().Load("Result/BackGroundGameOver.tga");

		// ÉWÉìÉOÉãçƒê∂
		NSound::Play(m_sndBGMLabel);
	}

	void CResult::Init()
	{
		CBase::Init();

		float hDivByW;
		float width;
		float height;

		// ÉçÉSÇÃê›íË
		hDivByW = m_pTexText->GetSize().y / m_pTexText->GetSize().x;
		width = NSystem::WINDOW_WIDTH * 0.75f;
		height = width * hDivByW;
		m_polyText.SetSize(width, height);
		m_polyText.SetTexture(m_pTexText);
		m_polyText.SetAlignY(NPolygon::EAlignY::UP);
		m_objText.GetTransform().SetPos(NSystem::WINDOW_WIDTH * 0.5f, NSystem::WINDOW_HEIGHT * 0.0f);

		// îwåiÇÃê›íË
		width = NSystem::WINDOW_WIDTH;
		height = NSystem::WINDOW_HEIGHT;
		m_polyBack.SetSize(width, height);
		if (NGame::GetGlobalData().isGameClear) {
			m_polyBack.SetTexture(m_pTexBackClear);
		}
		else {
			m_polyBack.SetTexture(m_pTexBackGameOver);
		}

		m_objBack.GetTransform().SetPos(NSystem::WINDOW_WIDTH * 0.5f, NSystem::WINDOW_HEIGHT * 0.5f);

		m_isSceneChangeReady = false;
		m_isSceneReady = false;

		// ÉtÉFÅ[Éh
		m_fade.SetFadeSec(1);
		float4	fadeColor;
		if (NGame::GetGlobalData().isGameClear) {
			fadeColor = float4(1, 1, 1, 1);
		}
		else {
			fadeColor = float4(0, 0, 0, 0);
		}
		m_fade.SetFadeColor(fadeColor);
		m_fade.FadeIn();
	}

	void CResult::Destroy()
	{
		CBase::Destroy();
	}

	void CResult::Release()
	{
		CBase::Release();
		NSound::Stop(m_sndBGMLabel);
	}
	  
	void CResult::Update()
	{
		CBase::Update();
		m_polyText.Update();

		if (m_fade.GetIsFadeEnd() && !m_isSceneChangeReady) {
			m_isSceneReady = true;
		}

		if (NInput::GetStart() &&
			m_isSceneReady &&
			!m_isSceneChangeReady) {
			m_isSceneChangeReady = true;
			m_fade.SetFadeColor(float4(1, 1, 1, 1));
			m_fade.SetFadeSec(1.5);
			m_fade.FadeOut();
		}

		if (m_isSceneChangeReady &&
			m_isSceneReady &&
			m_fade.GetIsFadeEnd()) {
			NScene::ChangeScene("CTitle");
		}

		m_fade.Update();
	}

	void CResult::Render()
	{
		CBase::Render();

		NCamera::SetOrthoMatrix();
		NMath::SMatrix44	mat;

		m_objBack.GetTransform().CalcWorldMatrix(mat);
		m_polyBack.Render(mat);

		m_objText.GetTransform().CalcWorldMatrix(mat);
		m_polyText.Render(mat);

		m_fade.Render();
	}
}