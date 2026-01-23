#include	"Object_PlayerLifeUI.h"

#include	"Game\Manager\Game_GlobalAccessManager.h"
#include	"Camera\Camera_GlobalManager.h"

namespace {

	float2	LIFE_BAR_BASE_POS(20, 20);
	float2	LIFE_BAR_SIZE_MAX(400, 50);

	float4	LIFE_COLOR_GOOD(0, 1, 0, 1);
	float4	LIFE_COLOR_BAD(1, 0, 0, 1);

	float4	LIFE_COLOR_WARNING_ADD(0.8f, 0.8f, 0.8f, 1.0f);

}

namespace NObject {

	CPlayerLifeUI::CPlayerLifeUI() :
		m_pPlayer(nullptr)
	{

	}

	CPlayerLifeUI::~CPlayerLifeUI()
	{
		m_pPlayer = nullptr;
	}


	void CPlayerLifeUI::LoadProc()
	{
		// 参照する値を設定
		m_pPlayer = NGame::GetPlayer();

		// texture読み込み
		m_pTexLifeBar = NAsset::CTextureFactory::GetInst().Load("Common/NoImage.tga");
		m_pTexLifeBarFrame = NAsset::CTextureFactory::GetInst().Load("UI/LifeBarFrame.tga");
	}

	void CPlayerLifeUI::InitProc()
	{

		// バー
		m_polyLifeBar.SetTexture(m_pTexLifeBar);
		m_polyLifeBar.SetAlignX(NPolygon::EAlignX::LEFT);
		m_polyLifeBar.SetAlignY(NPolygon::EAlignY::UP);
		m_polyLifeBar.SetSize(LIFE_BAR_SIZE_MAX);
		GetTransform().SetPos(LIFE_BAR_BASE_POS);

		// フレーム
		m_polyLifeBarFrame.SetTexture(m_pTexLifeBarFrame);
		m_polyLifeBarFrame.SetAlignX(NPolygon::EAlignX::LEFT);
		m_polyLifeBarFrame.SetAlignY(NPolygon::EAlignY::UP);
		m_polyLifeBarFrame.SetSize(LIFE_BAR_SIZE_MAX);

		// 背景
		m_polyLifeBarBack.SetAlignX(NPolygon::EAlignX::LEFT);
		m_polyLifeBarBack.SetAlignY(NPolygon::EAlignY::UP);
		m_polyLifeBarBack.SetSize(LIFE_BAR_SIZE_MAX);
		m_polyLifeBarBack.SetColor(float4(1, 1, 1, 0.25f));

	}

	void CPlayerLifeUI::UpdateProc()
	{
		int curLife = m_pPlayer->GetLife();
		int maxLife = m_pPlayer->GetLifeMax();

		float ratio = StaticCast<float>(curLife) / StaticCast<float>(maxLife);

		m_polyLifeBar.SetSize(LIFE_BAR_SIZE_MAX.x * ratio, LIFE_BAR_SIZE_MAX.y);

		// 体力が低くなるにつれて、色を変える
		float ratioGood = ratio;
		float ratioBad = 1 - ratio;
		float4 lifeColor =
			LIFE_COLOR_GOOD * ratioGood +
			LIFE_COLOR_BAD * ratioBad;

		m_polyLifeBar.SetColor(lifeColor);
	}

	void CPlayerLifeUI::RenderProc()
	{
		// 2D描画
		NCamera::SetOrthoMatrix();

		NMath::SMatrix44	matWorld;
		GetTransform().CalcWorldMatrix(matWorld);
		m_polyLifeBarBack.Render(matWorld);
		m_polyLifeBar.Render(matWorld);
		m_polyLifeBarFrame.Render(matWorld);
	}

}