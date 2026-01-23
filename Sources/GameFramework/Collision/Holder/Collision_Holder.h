#pragma once

#include	<vector>
#include	"Collision_Sequence.h"
#include	"Object\Object_GameObject.h"

#include	"Model\Model_Object.h"

namespace NCollision {

	//!	@brief		1オブジェクトの1種類の当たり判定をすべて管理するクラス
	//!	@note		種類とは、攻撃判定、被攻撃判定などを指す。
	class CHolder {
	public:

		//!	@brief	参照するモデルデータの型
		//!	@note	変更する予定があるので、対応しやすいように別名をつける
		using ModelData = NModel::CObject;

		//!	@brief		当たり判定情報を読み込む
		//!	@param[in]	_pRefOwner					所有者(Null を許容しない)
		//!	@param[in]	_seqCount					管理する状態の数(状態がない場合は1)
		//!	@param[in]	_ppCollisionInfoFilePath	当たり判定情報を持つファイルへのパス(null NG)
		void Load(
			NObject::CGameObject*	_pRefOwner,
			const uint				_seqCount,
			const char**			_ppCollisionInfoFilePath,
			const ModelData*		_pRefModelData = nullptr);

		//!	@brief		同じ状態を初期化
		//!	@warning	基本的な使用法では使用することはありません。
		void Init();

		//!	@brief		更新
		void Update(const uint _stateIndex);

		//!	@brief		デバッグ用描画
		void RenderDebug();

		//!	@brief	有効状態を設定
		void SetIsEnable(bool _state) { m_isEnable = _state; }

		//!	@brief		同じ状態から同じ状態に遷移する際に必要な処理
		//!	@note		これを行わないとあたり判定が初期化されず判定が出ない
		void Reset() { m_isReset = true; }

		// アクセッサ
		NObject::CGameObject*	GetOwner() { return m_pRefOwner; }
		CSequence&				GetCurSequence() { return m_seqArray[m_curStateIndex]; }

		uint					GetSeqCount() const { return m_seqCount; }
		uint					GetStateIndex() const { return m_curStateIndex; }

		bool					GetIsEnable() const { return m_isEnable; }

	private:

		// あたり判定関係
		NObject::CGameObject*	m_pRefOwner;
		const ModelData*		m_pRefModel;

		uint					m_curStateIndex;

		uint					m_seqCount;	// (=状態の数)
		std::vector<CSequence>	m_seqArray;

		bool					m_isEnable;	// 管理するあたり判定全体の有効状態を制御

		// 制御用
		bool					m_isLoadOK;
		uint					m_prevStateIndex;
		bool					m_isReset;

		//!	@brief	当たり判定に必要な情報を読み込む
		void			LoadData(
			NObject::CGameObject*	_pRefOwner,
			const char**			_ppCollisionInfoFilePath,
			const ModelData*		_pRefModelData);

		//!	@brief	ステートの変化を監視して、当たり判定を初期化する
		void CheckStateChange(const uint _curStateIndex);

		//!	@brief	リセット状態を判断して、すべてのあたり判定を初期化する
		void CheckReset();

	public:
		CHolder();
		~CHolder();
	};
}