#pragma once

#include	<vector>
#include	"Collision\Collision_OBBox.h"
#include	"Object\Object_GameObject.h"

#include	"Model\Model_Object.h"

namespace NCollision {

	//!	@brief		ゲーム内で使用する当たり判定(1つのOBBを管理)
	//!	@details	継承してに当たり判定に情報を追加する
	class CGame {
	public:

		//!	@brief	参照するモデルデータの型
		//!	@note	変更する予定があるので、対応しやすいように別名をつける
		using ModelData = NModel::CObject;

		//!	@brief	使用可能状態へのセットアップ
		void Load(
			NObject::CGameObject*	_pRefOwner,
			const uint				_groupID,
			const uint				_colGameID,
			const char*				_pCollisionInfoFilePath,
			const ModelData*		_pRefModelData);

		//!	@brief	デバッグ描画用のメッシュの解放を行う
		void Release();

		//!	@brief	初期化
		void Init();

		//!	@brief	当たり判定情報の更新
		void Update();

		//!	@brief	デバッグ用の描画
		void RenderDebug();



		// アクセッサ
		NObject::CGameObject*	GetOwner() { return m_pRefOwner; }
		bool					GetIsEnable() const { return m_isEnable; }
		uint					GetCollisionID() const { return m_collisionID; }
		bool					GetIsHit() const { return m_isHit; }
		bool					GetIsGroupHit() const { return m_isGroupHit; }

		const CTransformOBBox&	GetOBB() const { return m_obb; }

		bool					GetIsRefBone() const { return m_isRefBone; }
		uint					GetRefBoneIndex() const { return  m_refBoneIndex; }

		bool					GetIsRefOwnerTransform() const { return m_isRefOwnerTransform; }

		// セッター
		void					SetIsEnable(const bool _state) { m_isEnable = _state; }
		void					SetIsHit() { m_isHit = true; }
		void					SetIsGroupHit() { m_isGroupHit = true; }

	private:

		// プログラムで設定するデータ
		bool							m_isEnable;				// 有効フラグ
		bool							m_isHit;				// 衝突フラグ
		bool							m_isGroupHit;			// グループ内の他の当たり判定が衝突したフラグ
		uint							m_collisionID;			// グループ内のあたり判定を特定する識別子
		NObject::CGameObject*			m_pRefOwner;			// 所有者への参照
		const ModelData*				m_pRefModel;			// モデルデータへの参照

		// プログラム非依存データ
		CTransformOBBox					m_obb;					// OBBデータの

		bool							m_isRefBone;			// モデルのボーン行列を参照するフラグ
		bool							m_isCreateOBBByBone;	// ボーン情報をもとにOBBを作るフラグ
		uint							m_refBoneIndex;			// 参照するボーンのインデックス

		bool							m_isRefOwnerTransform;	// 所有者の行列を参照するフラグ
																// (ただし、ボーンを参照しない場合)

		// デバッグ用
		LPD3DXMESH			m_boxMesh;
		D3DMATERIAL9		m_materialEnable;
		D3DMATERIAL9		m_materialHit;
		D3DMATERIAL9		m_materialGroupHit;
		D3DMATERIAL9		m_materialDisable;


		//!	@brief	当たり判定に共通で必要なデータを読み込む
		void LoadCollisionInfo(
			const uint				_groupID,
			const uint				_colGameID,
			const char*				_pCollisionInfoFilePath);

		//!	@brief	OBB情報を読み込む
		void LoadOBBData(
			const uint				_groupID,
			const uint				_colGameID);

	public:
		CGame();
		~CGame();
	};
}
