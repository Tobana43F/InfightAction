#pragma once

#include	<unordered_map>
#include	<vector>
#include	"Collision\Holder\Collision_Holder.h"

// 前方宣言
namespace NObject {
	class CGameObject;
}

namespace NCollision {

	//!	@brief	ゲームで使用する衝突判定の処理を行うクラス
	//!	@note	継承して、詳細な処理を記述する
	//!	@todo	ここで当たり判定のTrigger,Release判定もとれるのでは？検討すること。
	class CProc {
	public:
		//!	@brief	衝突時の処理に必要なデータ
		struct SCollideData {
			uint					type;
			NObject::CGameObject*	pRefOwner;
			CHolder*				pRefHolder;
			CSequence*				pRefSequence;
			CGroup*					pRefGroup;
			CGame*					pRefGame;

			uint					sequenceID;
			uint					groupID;
			uint					collisionID;

			//!	@brief	必要データをすべて設定するコンストラクタ
			SCollideData(
				uint		_type,
				CHolder*	_pRefHolder,
				CSequence*	_pRefSequence,
				CGroup*		_pRefGroup,
				CGame*		_pRefGame);
		};


		//!	@brief	使用できる状態にする
		void Setup();

		//!	@brief	更新処理
		void Update();

	protected:

		//!	@brief	管理するデータ
		struct SData {
			CHolder*	pRefHolder;
		};


		// 当たり判定関係
		uint							m_typeCount;		// 判定する種類の数
		std::vector<std::vector<SData>>	m_typeHolder;		// タイプごとに全ての当たり判定を管理
		std::vector<std::vector<bool>>	m_collisionMatrix;	// 判定するかどうかを格納したマトリックス

		// 制御用
		bool							m_isSetupOK;



		//!	@brief		当たり判定を行う判定の種類を設定
		virtual void	SetupTypeCount() PURE_FUNC;

		//!	@brief		どの種類同士の当たり判定を行うかを設定
		virtual void	SetupCollisionMatrix() PURE_FUNC;



		//!	@brief		グループ内の1つの当たり判定が衝突時に行う処理
		virtual	void	ProcCollide(SCollideData& _rColA, SCollideData& _rColB);

		// 各タイミングでの処理
		//!	@brief	衝突判定の前後での処理(関数の入り口と出口での処理)
		virtual void	ProcBeforeCollide() {}
		virtual void	ProcAfterCollide() {}

		//!	@brief		種類の判定を行う前後の処理
		//!	@details	CHolder同士の当たり判定処理の開始、終了時
		virtual void	ProcBeforeTypeCollide(){}
		virtual void	ProcAfterTypeCollide(){}

		//!	@brief		グループ同士の判定を行う前後の処理
		virtual void	ProcBeforeGroupCollide(){}
		virtual void	ProcAfterGroupCollide(){}
		
		//!	@brief	1つのOBBを判定する前後の処理
		virtual void	ProcBeforeCollisionCollide(CGroup& _rGroup1, CGroup& _rGroup2){}
		virtual void	ProcAfterCollisionCollide(CGroup& _rGroup1, CGroup& _rGroup2){}



		//!	@brief		当たり判定を登録
		//!	@details	継承後にTypeをenumで定義し、それを引数とする登録関数を作り、
		//!				オーバーロードして、この関数をラップする
		//!	@param[in]	type 当たり判定のタイプ(敵、プレイヤーなどを識別する)
		void			Register(
			CHolder&	_rHolder,
			const uint	_type);

		//!	@brief		2種類間の当たり判定を行う(行わない)ようにを設定
		void			SetCollisionMatrix(
			const uint	_typeA, 
			const uint	_typeB, 
			const bool	_state = true);

	private:

		//!	@brief		指定した種類同士の判定を行うかを取得
		bool			GetIsActive(
			const uint	_typeA,
			const uint	_typeB)
		{
			return m_collisionMatrix[_typeA][_typeB];
		}


	public:
		CProc() : m_isSetupOK(false) { }
		virtual ~CProc() { }
	};
}