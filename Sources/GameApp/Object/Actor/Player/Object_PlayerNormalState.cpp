//!	@brief	プレイヤークラスの内部クラスの実装

#include	"Object_Player.h"
#include	"Input\Input_Game.h"
#include	"System\System_Def.h"
#include	"Collision\Manager\Collision_Manager.h"
#include	"Collision\CollisionProc\Collision_HitTest.h"

namespace NObject {

	CPlayer::CNormalState::CNormalState(CPlayer& _bindObject) : 
		m_bindObject(_bindObject),
		m_state(this, EState::IDLE),
		m_isIdleAttackReady(false),
		m_cntSwordSet(0)
	{
		// 状態と関数の設定
		m_state.Register(EState::IDLE,			&CNormalState::UpdateStateIdle);
		m_state.Register(EState::DASH,			&CNormalState::UpdateStateDash);
		m_state.Register(EState::JUMP_START,	&CNormalState::UpdateStateJumpStart);
		m_state.Register(EState::JUMP_UP,		&CNormalState::UpdateStateJumpUp);
		m_state.Register(EState::JUMP_DOWN,		&CNormalState::UpdateStateJumpDown);
		m_state.Register(EState::GROUNDED,		&CNormalState::UpdateStateGrounded);
		m_state.Register(EState::SWORD_SWITCH, &CNormalState::UpdateStateSwordSwitch);
		m_state.Register(EState::SWORD_GRAB_TO_SET, &CNormalState::UpdateStateSwordGrabToSet);
		m_state.Register(EState::SWORD_GRAB_TO_ATTACK, &CNormalState::UpdateStateSwordGrabToAttack);
		m_state.Register(EState::SWORD_SET, &CNormalState::UpdateStateSwordSet);
		m_state.Register(EState::SWORD_DASH, &CNormalState::UpdateStateSwordDash);
	}


	CPlayer::CNormalState::~CNormalState() 
	{

	}

	void CPlayer::CNormalState::Load()
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

		//---- 当たり判定の登録 ----
		auto pPDEAProc = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_DEF_ENEMY_ATK);
		pPDEAProc->Register(m_colDamageHolder, NCollision::CProcHitTest::EType::DEFENDER);
	}

	//----------------------------------------------------------
	// 状態の関数
	//----------------------------------------------------------

	void CPlayer::CNormalState::UpdateStateIdle(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const int INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_IDLE, true, INTERPOLE_FRAME, 1);
		}

		// ---- 操作 ----
		m_bindObject.UpdateUtilMove();

		// ---- 遷移判断 ----

		// 移動状態へ遷移
		if (!m_bindObject.GetIsStop()) {
			m_state.ChangeState(EState::DASH);
		}

		// ジャンプ状態に遷移
		if (NInput::GetJump()) {
			m_state.ChangeState(EState::JUMP_START);
		}

		// 攻撃状態に遷移
		if (NInput::GetAttack1())
		{
			//m_bindObject.m_attackState.Entry(CAttackState::EState::INFIGHT_PUNCH);
			m_state.ChangeState(EState::SWORD_SWITCH);
			m_isIdleAttackReady = true;
		}
	}


	void CPlayer::CNormalState::UpdateStateDash(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const int INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_DASH, true, INTERPOLE_FRAME, 1);
		}

		// ダッシュアニメーションの速度を移動速度に比例させる
		//!	@todo	外部データにするほうがよい
		const int DASH_ANM_BASE_SPEED = 2;	// アニメーション速度補正値
		float anmSpeed = DASH_ANM_BASE_SPEED * m_bindObject.GetMoveSpeedXZRatio();
		m_bindObject.m_model.SetAnimationSpeed(anmSpeed);

		// ---- 操作 ----
		m_bindObject.UpdateUtilMove();

		// ---- 遷移 ----
		// 歩き状態に遷移
		if (m_bindObject.GetIsStop()) {
			m_state.ChangeState(EState::IDLE);
		}

		// ジャンプ状態に遷移
		if (NInput::GetJump()) {
			m_state.ChangeState(EState::JUMP_START);
		}

		// 攻撃状態に遷移
		if (NInput::GetAttack1()) {
			//m_bindObject.m_attackState.Entry(CAttackState::EState::INFIGHT_PUNCH);
			m_state.ChangeState(EState::SWORD_SWITCH);
			m_isIdleAttackReady = true;
		}
	}

	void CPlayer::CNormalState::UpdateStateJumpStart(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			//!	@brief	外部データにするほうが良い
			const float JUMP_WAIT_FRAME = 5;
			const float anmSpeed = m_bindObject.m_model.GetAnimationFrameLength(ANM_NORMAL_JUMP_START) / JUMP_WAIT_FRAME;
			const int	INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_JUMP_START, false, INTERPOLE_FRAME, anmSpeed);

			// 一度停止
			m_moveSpeedJumpStart = m_bindObject.m_actorMove.GetMoveSpeedXZ();	// 停止前の速度をほっじ
			m_bindObject.m_actorMove.SetMoveSpeedXZ(0);
		}

		// アニメーションが終了したら、ジャンプを実行
		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::JUMP_UP);
		}
	}

	void CPlayer::CNormalState::UpdateStateJumpUp(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const int INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_JUMP_UP, false, INTERPOLE_FRAME, 1);

			// ジャンプ前の移動速度を適用
			m_bindObject.m_actorMove.SetMoveSpeedXZ(m_moveSpeedJumpStart);

			// ジャンプ処理
			const float JUMP_POW = 10 * NSystem::DELTA_TIME;
			m_bindObject.m_actorMove.Jump(JUMP_POW);

			// ジャンプエフェクト(テスト)
			float3 pos;
			m_bindObject.GetTransform().GetPos(pos);
			m_bindObject.m_effect.Play(pos);
		}
		else {
			if (m_bindObject.GetIsGrounded()) {
				m_state.ChangeState(EState::GROUNDED);
			}
		}
		m_bindObject.m_actorMove.SetMoveSpeedXZ(m_moveSpeedJumpStart);

		// 攻撃状態に遷移
		if (NInput::GetAttack1()) {
			m_bindObject.m_isSwordGrab = true;	// 剣をすぐに持つ
			m_bindObject.m_attackState.Entry(CAttackState::EState::SWORD_AIR1);
		}
	}

	void CPlayer::CNormalState::UpdateStateJumpDown(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const int INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_JUMP_DOWN, false, INTERPOLE_FRAME, 1);
		}
		else {
			if (m_bindObject.GetIsGrounded()) {
				m_state.ChangeState(EState::GROUNDED);
			}
		}

		// 攻撃状態に遷移
		if (NInput::GetAttack1()) {
			m_bindObject.m_isSwordGrab = true;
			m_bindObject.m_attackState.Entry(CAttackState::EState::SWORD_AIR1);
		}
	}

	void CPlayer::CNormalState::UpdateStateGrounded(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const float LAND_WAIT_FRAME = 5;
			const float anmSpeed = m_bindObject.m_model.GetAnimationFrameLength(ANM_NORMAL_GROUNDED) / LAND_WAIT_FRAME;
			const int	INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_GROUNDED, false, INTERPOLE_FRAME, anmSpeed);

			// 停止させる
			m_bindObject.m_actorMove.SetMoveSpeedXZ(0);			
		}

		// 遷移
		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			if (m_bindObject.GetIsStop()) {
				if (m_bindObject.m_isSwordGrab) {
					m_state.ChangeState(EState::SWORD_SET);
				}
				else {
					m_state.ChangeState(EState::IDLE);
				}
			}
		}
	}

	void CPlayer::CNormalState::UpdateStateSwordSwitch(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			// 攻撃柄移る場合とで分ける
			if (m_isIdleAttackReady) {
				const int INTERPOLE_FRAME = 1;
				m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_GRAB_SWORD_START, false, INTERPOLE_FRAME, 3);
			}
			else {
				const int INTERPOLE_FRAME = 3;
				m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_GRAB_SWORD_SET, false, INTERPOLE_FRAME, -0.6);
			}
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			// フラグ切り替え
			m_bindObject.m_isSwordGrab = !m_bindObject.m_isSwordGrab;

			// 遷移
			if (m_bindObject.m_isSwordGrab) {
				if (m_isIdleAttackReady) {
					m_state.ChangeState(EState::SWORD_GRAB_TO_ATTACK);
				}
				else {
					m_state.ChangeState(EState::SWORD_GRAB_TO_SET);
				}
			}
			else {
				m_state.ChangeState(EState::IDLE);
			}

			// リセット
			m_isIdleAttackReady = false;
		}
	}

	void CPlayer::CNormalState::UpdateStateSwordGrabToSet(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const int INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_GRAB_SWORD_SET, false, INTERPOLE_FRAME, 1);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::SWORD_SET);
		}
	}

	void CPlayer::CNormalState::UpdateStateSwordGrabToAttack(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			// 地上
			if (m_bindObject.GetIsGrounded()) {
				m_bindObject.m_attackState.Entry(CAttackState::EState::SWORD_ENTRY1);
			}
			// 空中
			else {
				m_bindObject.m_attackState.Entry(CAttackState::EState::SWORD_AIR1);
			}
		}
	}


	void CPlayer::CNormalState::UpdateStateSwordSet(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			const int INTERPOLE_FRAME = 5;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_SWORD_SET, true, INTERPOLE_FRAME, 1);

			// カウンタリセット
			m_cntSwordSet = 0;
		}

		// ---- 操作 ----
		m_bindObject.UpdateUtilMove();

		// ---- 遷移判断 ----

		// 一定時間経過で、剣をしまう
		const int SWORD_CLOSE_CNT = StaticCast<int>(3 * NSystem::FRAME_PER_SECOND);
		if (m_cntSwordSet > SWORD_CLOSE_CNT) {
			m_state.ChangeState(EState::SWORD_SWITCH);
		}

		// 移動状態へ遷移
		if (!m_bindObject.GetIsStop()) {
			m_state.ChangeState(EState::SWORD_DASH);
		}

		// ジャンプ状態に遷移
		if (NInput::GetJump()) {
			m_state.ChangeState(EState::JUMP_START);
		}

		// 攻撃状態に遷移
		if (NInput::GetAttack1()) {
			m_bindObject.m_attackState.Entry(CAttackState::EState::SWORD_ENTRY1);
		}

		// ---- 制御 ----
		m_cntSwordSet++;
	}

	void CPlayer::CNormalState::UpdateStateSwordDash(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_SWORD_DASH, true, 8, 1);
		}
		float anmSpeed = 2 * m_bindObject.GetMoveSpeedXZRatio();
		m_bindObject.m_model.SetAnimationSpeed(anmSpeed);

		// 移動
		m_bindObject.UpdateUtilMove();

		// 待機状態へ遷移
		if (m_bindObject.GetIsStop()) {
			m_state.ChangeState(EState::SWORD_SET);
		}

		// ジャンプ状態に遷移
		if (NInput::GetJump()) {
			m_state.ChangeState(EState::JUMP_START);
		}

		// 攻撃状態に遷移
		if (NInput::GetAttack1()) {
			m_bindObject.m_attackState.Entry(CAttackState::EState::SWORD_ENTRY1);
		}
	}

}