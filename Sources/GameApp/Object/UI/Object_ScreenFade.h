#pragma once

#include	"Object\Object_GameObject.h"

#include	"Util\Util_StateManager.h"
#include	"Polygon\Polygon_Object2D.h"

namespace NObject {

	class CScreenFade : public CGameObject {
	public:

		//!	@brief	フェードの状態遷移
		enum class EState {
			WAIT,		// 何もしない
			FADE_IN,	// 不透明 -> 透明
			FADE_OUT,	// 透明 　-> 不透明

			MAX
		};

		//!	@brief		フェードインを開始する
		//!	@note		不透明->透明
		void FadeIn();

		//!	@brief		フェードアウトを開始する
		//!	@note		透明->不透明
		void FadeOut();

		//!	@brief		フェード時間を設定
		void SetFadeSec(const float _fadeSec);

		//!	@brief		フェード色を設定
		//!	@note		透明度は無視される
		void SetFadeColor(const float4& _fadeColor);


		//!	@brief		フェードの終了状態を取得する
		bool	GetIsFadeEnd();

	protected:

		//!	@brief	初期化
		virtual void InitProc() override;

		//!	@brief	フェード更新
		virtual void UpdateProc() override;

		//!	@brief	描画
		virtual void RenderProc() override;

	private:

		// フェード用ポリゴン
		NPolygon::CObject2D		m_polyScreen;

		// 制御用
		float4					m_fadeColor;			// フェード時の色(alphaは1)
		float					m_fadeSec;				// フェード時間(秒)
		int						m_cntFade;				// フェード開始からのカウンタ


		// 状態管理
		using StateParam = NUtil::CStateManager<CScreenFade, EState>::Param;
		NUtil::CStateManager<CScreenFade, EState>	m_state;

		// 制御用
		void UpdateStateFadeWait(StateParam _param);
		void UpdateStateFadeIn(StateParam _param);
		void UpdateStateFadeOut(StateParam _param);

		// 汎用状態取得関数
		float	GetFadeRatio();

		// ユーティリティ
		float	LerpAlpha(
			const float _s, 
			const float _e, 
			const float _ratio);

	public:
		CScreenFade();
		virtual ~CScreenFade();
	};
}
