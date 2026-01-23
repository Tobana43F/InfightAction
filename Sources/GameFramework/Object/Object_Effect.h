#pragma once

#include	"Object\Object_GameObject.h"

#include	"Model\Model_Object.h"

namespace NObject {

	//!	@brief	エフェクトオブジェクトの基底クラス
	//!	@todo	現在はテスト状態(ジャンプエフェクトのテスト)
	//!			複数のエフェクトを作成する際に、このクラスを基底クラスとして、継承するように変更する。
	class CEffect : public CGameObject {
	public:

		enum EAnmIndex {
			ANM_EFFECT1
		};

		//!	@brief	エフェクト再生
		void Play(const float3& _pos, const NMath::SMatrix44& _matRot = NMath::SMatrix44());

	protected:

		//!	@brief	ロードをマルチスレッドで行うための

		virtual void	LoadProc();

		//!	@brief	解放を行う(1度だけ実行する)
		virtual void	ReleaseProc();

		//!	@brief	初期化を行う(何度も実行できること)
		//!	@note	初期化後は状態を「更新」にする
		virtual void	InitProc();

		//!	@brief	終了処理を行う(何度も実行できること)
		virtual void	DestroyProc() { }

		//!	@brief	継承後の更新処理
		virtual void	UpdateProc();

		//!	@brief	継承後の描画処理
		virtual void	RenderProc();

	private:

		bool				m_enable;
		NModel::CObject		m_model;

	public:
		CEffect();
		virtual ~CEffect();
	};

}