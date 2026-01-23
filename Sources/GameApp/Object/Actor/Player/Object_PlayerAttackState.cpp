#include	"Object_Player.h"

#include	"Collision\Manager\Collision_Manager.h"
#include	"Collision\CollisionProc\Collision_HitTest.h"
#include	"Input\Input_Game.h"
#include	"Input\Input_GameBuffer.h"
#include	"System\System_Def.h"

namespace NObject {

	CPlayer::CAttackState::CAttackState(CPlayer& _bindObject) :
		m_bindObject(_bindObject),
		m_state(this)
	{
		m_state.Register(EState::INFIGHT_PUNCH, &CAttackState::UpdateStateInFightPunch);
		m_state.Register(EState::SWORD_ENTRY1, &CAttackState::UpdateStateSwordEntry1);
		m_state.Register(EState::SWORD_HORIZONTAL1, &CAttackState::UpdateStateSwordHorizontal1);
		m_state.Register(EState::SWORD_DOUBLE_UPPER, &CAttackState::UpdateStateSwordDoubleUpper);
		m_state.Register(EState::SWORD_GROUND_SMASH, &CAttackState::UpdateStateSwordGroundSmash);
		m_state.Register(EState::SWORD_AIR1, &CAttackState::UpdateStateSwordAir1);
		m_state.Register(EState::SWORD_AIR2, &CAttackState::UpdateStateSwordAir2);
		m_state.Register(EState::SWORD_AIR3, &CAttackState::UpdateStateSwordAir3);
		m_state.Register(EState::SWORD_AIR_FINISH_START, &CAttackState::UpdateStateSwordAirFinishStart);
		m_state.Register(EState::SWORD_AIR_FINISH_END, &CAttackState::UpdateStateSwordAirFinishEnd);
		m_state.Register(EState::SWORD_AIR_FINISH_FALL, &CAttackState::UpdateStateSwordAirFinishFall);
	}

	CPlayer::CAttackState::~CAttackState()
	{

	}

	void CPlayer::CAttackState::Load()
	{
		// ----- 被ダメージ判定読み込み ----
		const char* pDamageCollisionPaths[StaticCast<uint>(EState::MAX)] = {
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
		};

		m_colDamageHolder.Load(
			&m_bindObject,
			StaticCast<uint>(EState::MAX),
			pDamageCollisionPaths,
			&m_bindObject.m_model);

		// ---- 攻撃判定読み込み ----
		const char* pAttackCollisionPaths[StaticCast<uint>(EState::MAX)] = {
			"Asset/Data/Collision/Player/Attack_Punch.json",
			"Asset/Data/Collision/Player/Attack_Sword_Entry1.json",
			"Asset/Data/Collision/Player/Attack_Sword_Horizontal1.json",
			"Asset/Data/Collision/Player/Attack_Sword_DoubleUpper.json",
			"Asset/Data/Collision/Player/Attack_Sword_GroundSmath.json",
			"Asset/Data/Collision/Player/Attack_Sword_Air1.json",
			"Asset/Data/Collision/Player/Attack_Sword_Air2.json",
			"Asset/Data/Collision/Player/Attack_Sword_Air3.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Player/Attack_Sword_AirGroundFinish.json",
			"Asset/Data/Collision/Common/None.json",
		};

		m_colAttackHolder.Load(
			&m_bindObject,
			StaticCast<uint>(EState::MAX),
			pAttackCollisionPaths,
			&m_bindObject.m_model);

		//---- 当たり判定の登録 ----	
		auto pPDEAProc = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_DEF_ENEMY_ATK);
		pPDEAProc->Register(m_colDamageHolder, NCollision::CProcHitTest::EType::DEFENDER);

		auto pPAEDProc = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_ATK_ENEMY_DEF);
		pPAEDProc->Register(m_colAttackHolder, NCollision::CProcHitTest::EType::ATTACKER);

	}

	//----------------------------------------------------------

	void CPlayer::CAttackState::UpdateStateInFightPunch(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_PUNCH, false, 3, 1);
		}


		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::IDLE);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordEntry1(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_SWORD_ENTRY1, false, 5, 1);
		}
		else {
			float frame = m_bindObject.m_model.GetAnimationFrame();

			//!	@todo 外部データへ
			const float CANCEL_START	= 17;
			const float CANCEL_END	= 25;
			if (NInput::GetAttack1(0, 10)) {
				if (frame >= CANCEL_START && frame < CANCEL_END) {
				// チェーンコンボへ派生
					m_state.ChangeState(EState::SWORD_HORIZONTAL1);
				}
			}
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordHorizontal1(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_SWORD_HORIZONTAL1, false, 2, 1);
		}
		else {
			float frame = m_bindObject.m_model.GetAnimationFrame();

			//!	@todo 外部データへ
			const float CANCEL_START = 21;
			const float CANCEL_END = 32;
			if (NInput::GetAttack1(0, 10)) {
				if (frame >= CANCEL_START && frame < CANCEL_END) {
				// チェーンコンボへ派生
					m_state.ChangeState(EState::SWORD_DOUBLE_UPPER);
				}
			}
		}


		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordDoubleUpper(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_SWORD_DOUBLE_UPPER, false, 3, 1);
			// 一歩踏み出す
			m_bindObject.m_actorMove.SetMoveDirXZ(m_bindObject.GetTransform().GetForward());
			m_bindObject.m_actorMove.AccelXZDirect(1.0f);
		}
		else {
			float frame = m_bindObject.m_model.GetAnimationFrame();

			//!	@todo 外部データへ
			const float CANCEL_START = 46;
			const float CANCEL_END = 55;
			if (NInput::GetAttack1(0, 20)) {
				if (frame >= CANCEL_START && frame < CANCEL_END) {
				// チェーンコンボへ派生
					m_state.ChangeState(EState::SWORD_GROUND_SMASH);
				}
			}
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordGroundSmash(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_SWORD_GROUND_SMASH, false, 3, 1);
			m_bindObject.m_actorMove.SetMoveDirXZ(m_bindObject.GetTransform().GetForward());
			m_bindObject.m_actorMove.AccelXZDirect(1.0f);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
		}
	}

	// ---- 空中攻撃 ----

	void CPlayer::CAttackState::UpdateStateSwordAir1(StateParam _param)
	{
		const float ANM_SPEED = 1.25f;
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_AIR1, false, 2, ANM_SPEED);

			// ちょっと跳ねる
			const float HOP_POW = 8 * NSystem::DELTA_TIME;
			m_bindObject.m_actorMove.Jump(HOP_POW);
		}
		else {
			float frame = m_bindObject.m_model.GetAnimationFrame();

			//!	@todo 外部データへ
			const float CANCEL_START = 16;
			const float CANCEL_END = 30;
			const uint	INPUT_RANGE = StaticCast<uint>(15.0f / ANM_SPEED);
			if (NInput::GetAttack1(0, INPUT_RANGE)) {
				if (frame >= CANCEL_START && frame < CANCEL_END) {
					// チェーンコンボへ派生
					m_state.ChangeState(EState::SWORD_AIR2);
				}
			}
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::JUMP_DOWN);
		}

		if (m_bindObject.GetIsGrounded()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::GROUNDED);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordAir2(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_AIR2, false, 2, 1);


			// ちょっと跳ねる
			const float HOP_POW = 8 * NSystem::DELTA_TIME;
			m_bindObject.m_actorMove.Jump(HOP_POW);
		}
		else {
			float frame = m_bindObject.m_model.GetAnimationFrame();

			//!	@todo 外部データへ
			const float CANCEL_START = 20;
			const float CANCEL_END = 35;
			if (NInput::GetAttack1(0, 10)) {
				if (frame >= CANCEL_START && frame < CANCEL_END) {
					// チェーンコンボへ派生
					m_state.ChangeState(EState::SWORD_AIR3);
				}
			}
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::JUMP_DOWN);
		}

		if (m_bindObject.GetIsGrounded()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::GROUNDED);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordAir3(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_AIR3, false, 2, 1);

			// ちょっと跳ねる
			const float HOP_POW = 8 * NSystem::DELTA_TIME;
			m_bindObject.m_actorMove.Jump(HOP_POW);
		}
		else {
			float frame = m_bindObject.m_model.GetAnimationFrame();

			//!	@todo 外部データへ
			const float CANCEL_START = 15;
			const float CANCEL_END = 25;
			if (NInput::GetAttack1(0, 10)) {
				if (frame >= CANCEL_START && frame < CANCEL_END) {
					// チェーンコンボへ派生
					m_state.ChangeState(EState::SWORD_AIR_FINISH_START);
				}
			}
		}


		if (m_bindObject.m_model.GetIsAnimationEnd() ||
			m_bindObject.m_model.GetAnimationFrame() > 30) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::JUMP_DOWN);
		}

		if (m_bindObject.GetIsGrounded()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::GROUNDED);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordAirFinishStart(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_AIR_FINISH_START, false, 0, 1);

			// ちょっと跳ねる
			const float HOP_POW = 4 * NSystem::DELTA_TIME;
			m_bindObject.m_actorMove.Jump(HOP_POW);
		}

		// 落下へ移行する
		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::SWORD_AIR_FINISH_FALL);
		}
		// 移行中に着地したらそのままフィニッシュ
		if (m_bindObject.GetIsGrounded()) {
			m_state.ChangeState(EState::SWORD_AIR_FINISH_END);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordAirFinishEnd(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_AIR_FINISH_END, false, 0, 1);
		}

		// 
		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
		}
	}

	void CPlayer::CAttackState::UpdateStateSwordAirFinishFall(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_ATTACK_AIR_FINISH_FALL, true, 2, 1);

			m_bindObject.m_actorMove.AccelXZDirect(5 * NSystem::DELTA_TIME);
			m_bindObject.m_actorMove.AccelYDirect(-20 * NSystem::DELTA_TIME);
		}
	
		if (m_bindObject.GetIsGrounded()) {
			m_state.ChangeState(EState::SWORD_AIR_FINISH_END);
		}
	}
}
