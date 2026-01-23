#pragma once

#include	<vector>

#include	"_External\json11\json11.hpp"
#include	"Collision_Game.h"
#include	"Model\Model_Object.h"
#include	"TypeData\Collision_TypeDataInclude.h"

namespace NCollision {

	//!	@brief		複数の当たり判定をまとめて1つの当たり判定を構成する
	//!	@details	タイプごとの情報をすべて持ち、管理する。
	//!				メモ)もともとは、タイプごとに継承して、追加情報を保持する予定だったが、
	//!				　　 継承後クラスをNewするとメモリの断面かも増えそうなので、まとめてデータを保持するようにする。
	class CGroup final {
	public:
		//!	@brief	参照するモデルデータの型
		//!	@note	変更する予定があるので、対応しやすいように別名をつける
		using ModelData = NModel::CObject;


		//!	@brief	判定属性をあらわすID
		enum EType {
			NONE,
			ATTACK,
			DAMAGE,
			MAX
		};

		//!	@brief	読み込み
		void Load(
			NObject::CGameObject*	_pRefOwner,
			const uint				_groupID,
			const char*				_pCollisionInfoFilePath,
			const ModelData*		_pRefModelData);

		//!	@brief	初期化
		void Init();

		//!	@brief	更新
		void Update();

		//!	@brief	デバッグ用の描画
		void RenderDebug();

		// アクセッサ
		bool			GetIsEnable() const { return m_isEnable; }
		bool			GetIsHit() const { return m_isHit; }
		uint			GetGroupID() const { return m_groupID; }
		uint			GetEventFrame() const { return m_eventFrame; }
		EType			GetType() const { return m_colType; }
		uint			GetCollisionCount() const { return m_colGameCount; }

		const CAttackTypeData&	GetAttackTypeData() const;
		const CDamageTypeData&	GetDamageTypeData() const;


		void			SetIsEnable();
		void			SetIsHit() { m_isHit = true; }

		//!	@brief	当たり判定データを取得する
		CGame&			GetCollision(uint _idx) { return m_colGames[_idx]; }

	private:
		// プログラム依存データ
		bool					m_isEnable;
		bool					m_isHit;
		uint					m_groupID;			// シーケンス内のグループを特定する識別子
		bool					m_isPermanent;		// 永続的有効有効フラグ
		uint					m_curEnableFrame;	// 現在の有効フレーム(有効フレーム残り時間)

		// プログラム非依存データ
		// Groupクラスの基本的な情報
		uint					m_eventFrame;		// 当たり判定が有効になるまでの時間
		uint					m_enableFrame;		// 有効時間(EventFrameからの有効時間)
		EType					m_colType;			// 当たり判定情報

		// タイプによる情報(m_colTypeに対応した情報のみ有効)
		CNoneTypeData			m_typeDataNone;
		CAttackTypeData			m_typeDataAttack;
		CDamageTypeData			m_typeDataDamage;

		// あたり判定情報群
		uint					m_colGameCount;
		std::vector<CGame>		m_colGames;



		//!	@brief		このクラスの判定情報を読み込む
		//!	@param[in]	_allData	全ての判定情報を含んだ、jsonデータ
		//!	@param[in]
		//!	@note		例外を投げる。Load関数で捕捉する
		void LoadBaseData(
			json11::Json&	_groupData,
			uint			_groupID);

		//!	@brief	継承後の追加情報を読み込む
		//!	@note	例外を投げる。Load関数で捕捉する
		void	LoadAddData(json11::Json&	_groupData);
		
		//!	@brief	グループに属するあたり判定をすべての有効状態を変更する
		void SetCollisionGameEnable(bool _state);

	public:
		CGroup();
		~CGroup();
	};
}
