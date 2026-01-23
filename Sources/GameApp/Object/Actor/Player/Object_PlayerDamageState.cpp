#include	"Object_Player.h"

#include	"Collision\Manager\Collision_Manager.h"
#include	"Collision\CollisionProc\Collision_HitTest.h"

namespace NObject {

	CPlayer::CDamageState::CDamageState(CPlayer& _bindObject) :
		m_bindObject(_bindObject),
		m_state(this)
	{
		// èÛë‘Ç∆ä÷êîÇÃê›íË
		m_state.Register(EState::GROUND_LOW, &CDamageState::UpdateStateGroundLow);
		m_state.Register(EState::GROUND_MIDDLE, &CDamageState::UpdateStateGroundMiddle);
		m_state.Register(EState::GROUND_HIGH, &CDamageState::UpdateStateGroundHigh);
		m_state.Register(EState::GROUND_RECOVER, &CDamageState::UpdateStateGroundRecover);
		m_state.Register(EState::AIR, &CDamageState::UpdateStateAir);
		m_state.Register(EState::AIR_UP, &CDamageState::UpdateStateAirUp);
		m_state.Register(EState::AIR_FALL, &CDamageState::UpdateStateAirFall);
		m_state.Register(EState::GROUNDED, &CDamageState::UpdateStateGrounded);
		m_state.Register(EState::LAY_RECOVER, &CDamageState::UpdateStateLayRecover);
		m_state.Register(EState::DIE, &CDamageState::UpdateStateDie);
	}

	CPlayer::CDamageState::~CDamageState()
	{

	}

	void CPlayer::CDamageState::Load()
	{
		// ----- îÌÉ_ÉÅÅ[ÉWîªíËì«Ç›çûÇ› ----
		const char* pDamageCollisionPaths[StaticCast<uint>(EState::MAX)] = {
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Player/Damage_Common.json",
			"Asset/Data/Collision/Common/None.json",
		};

		m_colDamageHolder.Load(
			&m_bindObject,
			StaticCast<uint>(EState::MAX),
			pDamageCollisionPaths,
			&m_bindObject.m_model);

		//---- ìñÇΩÇËîªíËÇÃìoò^ ----
		auto pPDEAProc = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_DEF_ENEMY_ATK);
		pPDEAProc->Register(m_colDamageHolder, NCollision::CProcHitTest::EType::DEFENDER);
	}

	//----------------------------------------------------------


	void CPlayer::CDamageState::UpdateStateGroundLow(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_DAMAGE_LIGHT, false, 1, 1);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::GROUND_RECOVER);
		}
	}

	void CPlayer::CDamageState::UpdateStateGroundMiddle(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_DAMAGE_LIGHT, false, 1, 1);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::GROUND_RECOVER);
		}
	}

	void CPlayer::CDamageState::UpdateStateGroundHigh(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_DAMAGE_HEAVY, false, 1, 1);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::GROUND_RECOVER);
		}
	}

	void CPlayer::CDamageState::UpdateStateGroundRecover(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.SetNormalActorMoveParam();

			// åïÇÃï€éùèÛë‘Ç…ÇÊÇËÅAëJà⁄êÊÇêßå‰
			if (m_bindObject.m_isSwordGrab) {
				m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
			}
			else {
				m_bindObject.m_normalState.Entry(CNormalState::EState::IDLE);
			}
		}
	}

	void CPlayer::CDamageState::UpdateStateAir(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_DAMAGE_HEAVY, false, 1, 1);

			m_state.ChangeState(EState::AIR_UP);
		}
	}

	void CPlayer::CDamageState::UpdateStateAirUp(StateParam _state)
	{
		if (m_bindObject.m_actorMove.GetMoveSpeedY() < 0) {
			m_state.ChangeState(EState::AIR_FALL);
		}
	}

	void CPlayer::CDamageState::UpdateStateAirFall(StateParam _state)
	{
		if (m_bindObject.GetIsGrounded()) {
			m_state.ChangeState(EState::GROUNDED);
		}
	}

	void CPlayer::CDamageState::UpdateStateGrounded(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			const float LAND_WAIT_FRAME = 10;
			const float anmSpeed = m_bindObject.m_model.GetAnimationFrameLength(ANM_NORMAL_GROUNDED) / LAND_WAIT_FRAME;
			const int	INTERPOLE_FRAME = 3;
			m_bindObject.m_model.ChangeAnimation(ANM_NORMAL_GROUNDED, false, INTERPOLE_FRAME, anmSpeed);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::LAY_RECOVER);
		}
	}

	void CPlayer::CDamageState::UpdateStateLayRecover(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.SetNormalActorMoveParam();

			// åïÇÃï€éùèÛë‘Ç…ÇÊÇËÅAëJà⁄êÊÇêßå‰
			if (m_bindObject.m_isSwordGrab) {
				m_bindObject.m_normalState.Entry(CNormalState::EState::SWORD_SET);
			}
			else {
				m_bindObject.m_normalState.Entry(CNormalState::EState::IDLE);
			}
		}
	}

	void CPlayer::CDamageState::UpdateStateDie(StateParam _state)
	{
		if (_state.GetIsEntry()) {
			m_bindObject.m_model.ChangeAnimation(ANM_DAMAGE_DIE, false, 1, 1);

			// í‚é~
			m_bindObject.m_actorMove.SetMoveSpeedXZ(0);
		}

		if (m_bindObject.m_model.GetIsAnimationEnd()) {
			m_bindObject.SetIsEliminated();
		}
	}

}
