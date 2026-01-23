#pragma once

#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Collision\Holder\TypeData\Collision_TypeDataAttack.h"

namespace NGame {

	//!	@brief	Object_Actorが保持するデータ
	//!	@note	体力等の情報を保持する
	struct SActorParam {
		int			life;			// 残り体力
		int			maxLife;		// 最大体力

		bool		isEliminated;	// 消滅状態
									// (Actor(主に敵)が消滅する状態)
									// (体力がなくなった後のアニメーション後に削除可能になったらtrueを設定する。)

		void Setup(NAsset::CJsonFactory::Key _key);

		//!	@brief	初期化
		void Init();

		//!	@brief	生存状態を取得する
		bool		GetIsAlive() const { return (life > 0); }

		//!	@brief	初期化用コンストラクタ
		SActorParam();
	};

	//!	@brief	Object_Actorが保持するダメージデータ
	//!			ActorParamに実際に反映するためのダメージデータ
	struct SDamageParam {
		bool		isReciveDamage;	// 攻撃を受けたフラグ
									// falseならこの構造体の内容は意味を持たない

		float3		ownerPos;
		float3		collidePos;

		int			damage;
		float3		knockBackDir;
		float		knockBackPow;

		NCollision::CAttackTypeData::EStrengthType	receiveAttackStrengthType;

		void Init()
		{
			isReciveDamage = false;
			damage = 0;
		}

		SDamageParam() : 
			isReciveDamage(false),
			damage(0)
		{}
	};

}
