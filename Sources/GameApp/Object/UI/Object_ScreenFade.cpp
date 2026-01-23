#include	"Object\UI\Object_ScreenFade.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"Debug\Debug_Assert.h"
#include	"Math\Math_Common.h"
#include	"System\System_Def.h"

namespace {
	float4		COLOR_TRANSPARENT(0, 0, 0, 0);	// 透明色
}

namespace NObject {

	CScreenFade::CScreenFade() :
		m_state(this, EState::WAIT),
		m_fadeColor(1,1,1,1),
		m_fadeSec(1),
		m_cntFade(0)
	{
		m_state.Register(EState::WAIT, &CScreenFade::UpdateStateFadeWait);
		m_state.Register(EState::FADE_IN, &CScreenFade::UpdateStateFadeIn);
		m_state.Register(EState::FADE_OUT, &CScreenFade::UpdateStateFadeOut);
	}

	CScreenFade::~CScreenFade()
	{

	}

	void CScreenFade::FadeIn()
	{
		m_cntFade = 0;
		m_polyScreen.SetColor(m_fadeColor);
		m_state.ChangeState(EState::FADE_IN);
	}

	void CScreenFade::FadeOut()
	{
		m_cntFade = 0;
		m_polyScreen.SetColor(COLOR_TRANSPARENT);
		m_state.ChangeState(EState::FADE_OUT);
	}

	void CScreenFade::SetFadeSec(const float _fadeSec)
	{
		Assert(_fadeSec > 0);
		m_fadeSec = _fadeSec;
	}

	void CScreenFade::SetFadeColor(const float4& _fadeColor)
	{
		m_fadeColor = _fadeColor;
		m_fadeColor.w = 1;
	}

	//----------------------------------------------------------

	void CScreenFade::InitProc()
	{
		CGameObject::InitProc();

		m_cntFade = 0;

		// 原点から描画するように設定する
		m_polyScreen.SetAlignX(NPolygon::EAlignX::LEFT);
		m_polyScreen.SetAlignY(NPolygon::EAlignY::UP);

		// ポリゴンをスクリーンサイズにする
		m_polyScreen.SetSize(
			NSystem::WINDOW_WIDTH,
			NSystem::WINDOW_HEIGHT);

		m_polyScreen.SetColor(m_fadeColor);
	}

	void CScreenFade::UpdateProc()
	{
		CGameObject::UpdateProc();

		m_state.Update();
	}

	void CScreenFade::RenderProc()
	{
		CGameObject::RenderProc();

		NCamera::SetViewMatrix();
		NCamera::SetOrthoMatrix();

		NMath::SMatrix44	mat;
		GetTransform().CalcWorldMatrix(mat);
		m_polyScreen.Render(mat);
	}

	//----------------------------------------------------------

	void CScreenFade::UpdateStateFadeWait(StateParam _param)
	{
		// 何もしない
	}

	void CScreenFade::UpdateStateFadeIn(StateParam _param)
	{
		if (_param.GetIsEntry())
		{
			m_cntFade = 0;
			m_polyScreen.SetColor(m_fadeColor);
		}

		// 徐々に透明にする
		// アルファ値の線形補間
		float ratio = GetFadeRatio();
		float alpha = LerpAlpha(m_fadeColor.w, COLOR_TRANSPARENT.w, ratio);

		float4 col = m_fadeColor;
		col.w = alpha;
		m_polyScreen.SetColor(col);

		// 終了
		if (GetIsFadeEnd()) {
			m_state.ChangeState(EState::WAIT);
		}

		// カウンタ更新
		m_cntFade++;
	}

	void CScreenFade::UpdateStateFadeOut(StateParam _param)
	{
		// 徐々に不透明にする		
		if (_param.GetIsEntry())
		{
			m_cntFade = 0;
			m_polyScreen.SetColor(COLOR_TRANSPARENT);
		}

		// 徐々に透明にする
		// アルファ値の線形補間
		float ratio = GetFadeRatio();
		float alpha = LerpAlpha(COLOR_TRANSPARENT.w, m_fadeColor.w, ratio);

		float4 col = m_fadeColor;
		col.w = alpha;
		m_polyScreen.SetColor(col);

		// 終了
		if (GetIsFadeEnd()) {
			m_state.ChangeState(EState::WAIT);
		}

		m_cntFade++;
	}

	//----------------------------------------------------------

	float CScreenFade::GetFadeRatio()
	{
		int fadeFrame = StaticCast<int>(m_fadeSec * NSystem::FRAME_PER_SECOND);
		float ratio = StaticCast<float>(m_cntFade) / StaticCast<float>(fadeFrame);
		if (ratio > 1) { ratio = 1; }
		else if (ratio < 0) { ratio = 0; }
		
		return ratio;
	}

	bool CScreenFade::GetIsFadeEnd()
	{
		int fadeFrame = StaticCast<int>(m_fadeSec * NSystem::FRAME_PER_SECOND);
		if (m_cntFade >= fadeFrame) {
			return true;
		}
		else {
			return false;
		}
	}

	//----------------------------------------------------------
	float	CScreenFade::LerpAlpha(
			const float _s,
			const float _e,
			const float _ratio)
	{
		float alpha = _s + ((_e - _s) * _ratio);
		return alpha;
	}
}