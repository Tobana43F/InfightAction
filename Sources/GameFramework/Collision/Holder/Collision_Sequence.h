#pragma once

#include	<vector>
#include	"Collision_Group.h"
#include	"Object\Object_GameObject.h"

#include	"Model\Model_Object.h"

namespace NCollision {

	//!	@brief		複数の当たり判定グループの発生から消滅までを管理する
	class CSequence {
	public:

		//!	@brief	参照するモデルデータの型
		//!	@note	変更する予定があるので、対応しやすいように別名をつける
		using ModelData = NModel::CObject;

		//!	@brief	読み込み
		void Load(
			NObject::CGameObject*	_pRefOwner,
			const uint				_sequenceID,
			const char*				_pCollisionInfoFilePath,
			const ModelData*		_pRefModel);

		//!	@brief	初期化
		void Init();
		
		//!	@brief	グループをすべて更新
		void Update();

		//!	@brief	デバッグ用描画
		void RenderDebug();

		// アクセッサ
		uint	GetGroupCount() const { return m_groupCount; }
		CGroup&	GetGroup(uint _idx) { return m_colGroups[_idx]; }
		uint	GetSequenceID() const { return m_sequenceID; }

	private:
		
		uint				m_sequenceID;	// シーケンスID
		uint				m_groupCount;	// グループ数
		std::vector<CGroup>	m_colGroups;	// 管理する当たり判定グループ

		uint				m_cnt;			// カウンタ(毎使用時に初期化)

	public:
		CSequence();
		~CSequence();
	};
}