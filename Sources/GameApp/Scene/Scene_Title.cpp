#include	"Scene_Title.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"System\System_Def.h"
#include	"Input\Input_Common.h"
#include	"Input\Input_Game.h"

#include	"Math\Math_Common.h"

namespace NScene {

	CTitle::CTitle() : 
		m_sndBGMLabel(NSound::ELabel::BGM_TITLE),
		m_isSceneReady(false),
		m_isSceneChangeReady(false)
	{

	}

	CTitle::~CTitle()
	{

	}

	void CTitle::Load()
	{
		CBase::Load();

		m_pTexLogo = NAsset::CTextureFactory::GetInst().Load("Title/Logo.tga");
		m_pTexBack = NAsset::CTextureFactory::GetInst().Load("Title/BackGround.tga");

		m_model.Load("SkyDome/SkyDome.fbx");
		//m_model.Load("Effect/Hit/Model.fbx");

		NMath::SMatrix44	mat;
		NMath::MakeMatrixLookAtLH(mat, float3(0, -2, -3), float3(0, 0, 0), float3(0, 1, 0));
		NCamera::RegisterViewMatrix(mat);

		// サウンド再生
		NSound::Stop(m_sndBGMLabel);
		NSound::Play(m_sndBGMLabel);
	}

	void CTitle::Init()
	{
		CBase::Init();

		float hDivByW;
		float width;
		float height;

		// ロゴの設定
		hDivByW = m_pTexLogo->GetSize().y / m_pTexLogo->GetSize().x;
		width = NSystem::WINDOW_WIDTH * 0.75f;
		height = width * hDivByW;
		m_polyLogo.SetSize(width, height);
		m_polyLogo.SetTexture(m_pTexLogo);
		m_polyLogo.SetAlignY(NPolygon::EAlignY::UP);
		m_objLogo.GetTransform().SetPos(NSystem::WINDOW_WIDTH * 0.5f, NSystem::WINDOW_HEIGHT * 0.0f);

		// 背景の設定
		width = NSystem::WINDOW_WIDTH;
		height = NSystem::WINDOW_HEIGHT;
		m_polyBack.SetSize(width, height);
		m_polyBack.SetTexture(m_pTexBack);
		m_objBack.GetTransform().SetPos(NSystem::WINDOW_WIDTH * 0.5f, NSystem::WINDOW_HEIGHT * 0.5f);

		m_isSceneChangeReady = false;
		m_isSceneReady = false;

		// フェード
		m_fade.SetFadeSec(1.5);
		m_fade.SetFadeColor(float4(1, 1, 1, 1));
		m_fade.FadeIn();
	}

	void CTitle::Destroy()
	{
		CBase::Destroy();
	}

	void CTitle::Release()
	{
		CBase::Release();
		NSound::Stop(m_sndBGMLabel);
	}

	void CTitle::Update()
	{
		CBase::Update();

		if (m_fade.GetIsFadeEnd() && !m_isSceneChangeReady) {
			m_isSceneReady = true;
		}

		if (NInput::GetStart() && 
			m_isSceneReady && 
			!m_isSceneChangeReady) {
			m_isSceneChangeReady = true;
			m_fade.FadeOut();
		}

		if (m_isSceneChangeReady &&
			m_isSceneReady &&
			m_fade.GetIsFadeEnd()) {
			NScene::ChangeScene("CGame");
		}

		m_polyLogo.Update();
		m_fade.Update();


	}

	void CTitle::Render()
	{
		CBase::Render();	

		NCamera::SetViewMatrix();
		NCamera::SetProjMatrix();
		NMath::SMatrix44 mat;
		m_model.RenderDebug(mat);

		NCamera::SetOrthoMatrix();

		m_objBack.GetTransform().CalcWorldMatrix(mat);
		//m_polyBack.Render(mat);

		m_objLogo.GetTransform().CalcWorldMatrix(mat);
		m_polyLogo.Render(mat);

		m_fade.Render();

	}
}