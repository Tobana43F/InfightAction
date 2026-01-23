#pragma once

#include	"../Object_Actor.h"

namespace NObject {

	//!	@brief	敵オブジェクトの基底クラスになるオブジェクト
	class CEnemy : public CActor {
	public:

	protected:
		//!	@brief	生成時に1度だけ行う処理を記述する
		virtual void	LoadProc() override;

		//!	@brief	終了時に1度だけ行う処理を記述する
		virtual void	ReleaseProc() override;

		//!	@brief	初期化処理を記述する
		virtual void	InitProc() override;

		//!	@brief	終了処理を記述する
		//!	@note	あまり使うことはない
		virtual void	DestroyProc() override;

		//!	@brief	更新処理を記述する
		virtual void	UpdateProc() override;

		//!	@brief	描画処理を記述する
		virtual void	RenderProc() override;

		//!	@brief	当たり判定のデバッグ描画
		virtual void	RenderLateProc() override;

		//!	@brief	操作関数
		virtual void Operate() override PURE_FUNC;

		//!	@brief		ダメージ処理
		virtual void ProcDamage(const NGame::SDamageParam& _rDamage) override;


	private:
		// 継承元クラスのエイリアス
		using Base = CActor;

	public:
		CEnemy();
		virtual ~CEnemy();
	};


}