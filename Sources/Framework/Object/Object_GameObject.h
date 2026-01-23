#pragma once

#include	"Object_Transform.h"

namespace NObject {

	//!	@brief		ゲーム内のすべてのオブジェクトの基底クラスとなるクラス
	class CGameObject  {
	public:

		//!	@brief	更新の制御状態を表す定数
		enum class EObjectState {
			LOAD,			// ロードを行う
			INIT,			// 初期化を行う
			UPDATE,			// 更新を行う
		};

		//!	@brief	初期化する
		void Init();

		//!	@brief	更新処理
		void Update();

		//!	@brief	描画処理
		void Render();

		//!	@brief	終了処理を行う(何度も実行できること)
		//!	@note	使用することはあまりない
		void Destroy();


		//!	@brief	有効状態を設定
		void SetIsActive(const bool _state)		{ m_isActive = _state; }

		//!	@brief	有効状態を取得
		bool GetIsActive() const				{ return m_isActive; }

		//!	@brief	Transformを取得
		CTransform&			GetTransform()		 { return m_transform; }
		const CTransform&	GetTransform() const { return m_transform; }

	protected:

		//!	@brief	ロードをマルチスレッドで行うための

		//!	@brief	読み込みを行う(1度だけ実行する)
		//!	@note	ロード後は状態を「初期化」にする
		virtual void	LoadProc() { m_objState = EObjectState::INIT; }

		//!	@brief	解放を行う(1度だけ実行する)
		virtual void	ReleaseProc(){ }

		//!	@brief	初期化を行う(何度も実行できること)
		//!	@note	初期化後は状態を「更新」にする
		virtual void	InitProc() { m_objState = EObjectState::UPDATE; }

		//!	@brief	終了処理を行う(何度も実行できること)
		virtual void	DestroyProc(){ }

		//!	@brief	処理順の制御のために、通常のUpdateより早く実行される更新関数
		//!	@note	できる限り使用せず、UpdateProcを使用すること。
		virtual void	UpdateFastProc() { }

		//!	@brief	継承後の更新処理
		virtual void	UpdateProc(){  }

		//!	@brief	処理順の制御のために、通常のUpdateより遅く実行される更新関数
		//!	@note	できる限り使用せず、UpdateProcを使用すること。
		virtual void	UpdateLateProc() { }

		//!	@brief	継承後の描画処理
		virtual void	RenderProc(){ }

		//!	@brief	継承後の通常のタイミングより遅い描画
		//!	@note	デバッグ描画用や、アルファ値を含む描画の描画順を制御するときに使用する
		virtual void	RenderLateProc() { }

	private:

		bool			m_isActive;		// オブジェクトの有効状態(更新と描画を行うフラグ)
		EObjectState	m_objState;
		CTransform		m_transform;

	public:
		CGameObject();
		virtual ~CGameObject();
	};
}
