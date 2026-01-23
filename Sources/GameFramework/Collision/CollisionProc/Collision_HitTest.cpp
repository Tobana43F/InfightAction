#include	"Collision_HitTest.h"

#include	<stdlib.h>
#include	"Collision\Collision_Common.h"
#include	"Collision\Def\Collision_ReceiveAttackInfo.h"
#include	"Debug\Debug_Assert.h"
#include	"System\System_Error.h"
#include	"Object\Actor\Enemy\Object_SandBag.h"

namespace NCollision {

	void CProcHitTest::Register(CHolder& _rHolder, const EType _type)
	{
		Assert(_type != TYPE_MAX);

		uint type = StaticCast<uint>(_type);
		CProc::Register(_rHolder, type);
	}


	//----------------------------------------------------------

	void CProcHitTest::SetupCollisionMatrix()
	{
		// デフォルトを使用
	}

	void CProcHitTest::ProcBeforeCollide()
	{
		// 何もしない
	}

	void CProcHitTest::ProcCollide(SCollideData& _rColA, SCollideData& _rColB)
	{
		CProc::ProcCollide(_rColA, _rColB);

		// タイプを特定して処理を分岐
		if (_rColA.type == ATTACKER) {
			ProcCollideAttack(_rColA, _rColB);
		}
		else {
			ProcCollideAttack(_rColB, _rColA);
		}
	}

	void CProcHitTest::ProcAfterCollide()
	{
		// 何もしない
	}

	//----------------------------------------------------------
	void CProcHitTest::ProcCollideAttack(SCollideData& _rColAttacker, SCollideData& _rColDefender)
	{
		// CActorクラスにダウンキャスト
		//NObject::CActor* pActorAttacker = DynamicCast<NObject::CActor*>(_rColAttacker.pRefOwner);
		NObject::CActor* pActorDefender = DynamicCast<NObject::CActor*>(_rColDefender.pRefOwner);

		// 被ダメージデータを作成
		CReceiveAttackInfo	receiveAttackInfo;
		receiveAttackInfo.SetData(
			_rColAttacker,
			_rColAttacker.pRefGroup->GetAttackTypeData(),
			_rColDefender,
			_rColDefender.pRefGroup->GetDamageTypeData());

		pActorDefender->AddReceiveAttackInfo(receiveAttackInfo);
	}
}
