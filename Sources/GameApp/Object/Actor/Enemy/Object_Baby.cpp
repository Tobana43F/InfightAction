#include	"Object_Baby.h"

#include	"Asset\Factory\Asset_ShaderFactory.h"
#include	"Camera\Camera_GlobalManager.h"
#include	"Game\Manager\Game_GlobalAccessManager.h"
#include	"Math\Math_Common.h"
#include	"Math\Math_Util.h"
#include	"System\System_Def.h"

#include	"Collision\CollisionProc\Collision_HitTest.h"
#include	"Sound\Sound_Common.h"

namespace {
	NAsset::CShaderFactory::SPAsset	g_shader;
}

namespace NObject {

	CBaby::CBaby() :
		m_state(this, EState::IDLE)
	{
		m_state.Register(EState::IDLE, &CBaby::UpdateStateIdle);
		m_state.Register(EState::FOLLOW, &CBaby::UpdateStateFollow);
		m_state.Register(EState::ATTACK, &CBaby::UpdateStateAttack);
		m_state.Register(EState::DAMAGE_LOW, &CBaby::UpdateStateDamageLow);
		m_state.Register(EState::DAMAGE_HIGH, &CBaby::UpdateStateDamageHigh);
		m_state.Register(EState::DAMAGE_RECOVER, &CBaby::UpdateStateDamageRecover);
		m_state.Register(EState::DAMAGE_DIE, &CBaby::UpdateStateDamageDie);
	}

	CBaby::~CBaby()
	{

	}

	//==========================================================
	// 基本的な関数群
	//==========================================================

	void CBaby::LoadProc()
	{
		Base::LoadProc();

		// モデルの読み込み
		m_model.Load("Enemy/Baby/Model.fbx");

		// シェーダーの読み込み
		std::string	key = "AlphaTexture.hlsl";
		NAsset::CShaderFactory::GetInst().SetLoadInfo(
			key,
			"VS",
			"PS");
		g_shader = NAsset::CShaderFactory::GetInst().Load(key);

		//----------------------------------------------------------
		// あたり判定
		//----------------------------------------------------------

		// 当たり判定読み込み
		const char* pDamagePaths[StaticCast<int>(EState::MAX)] = {
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Common/None.json",
		};

		m_damageHolder.Load(
			this,
			StaticCast<int>(EState::MAX),
			pDamagePaths,
			&m_model);

		// 当たり判定読み込み
		const char* pAttackPaths[StaticCast<int>(EState::MAX)] = {
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Enemy/Baby/Attack/Attack1.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Common/None.json",
		};

		m_attackHolder.Load(
			this,
			StaticCast<int>(EState::MAX),
			pAttackPaths,
			&m_model);

		// 登録
		auto pPAED = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_ATK_ENEMY_DEF);
		pPAED->Register(m_damageHolder, NCollision::CProcHitTest::EType::DEFENDER);

		auto pPDEA = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_DEF_ENEMY_ATK);
		pPDEA->Register(m_attackHolder, NCollision::CProcHitTest::EType::ATTACKER);

		//----------------------------------------------------------
		// 追従するオブジェクトを設定
		//----------------------------------------------------------

		m_pFollowObject = NGame::GetPlayer();
	}

	void CBaby::ReleaseProc()
	{
		Base::ReleaseProc();
	}

	void CBaby::InitProc()
	{
		Base::InitProc();

		m_state.ChangeState(EState::IDLE);
		m_idleFrame = 0;
	}

	void CBaby::DestroyProc()
	{
		Base::DestroyProc();
	}

	void CBaby::UpdateProc()
	{
		Base::UpdateProc();

		// 状態更新
		m_state.Update();

		// モデルのアニメーションを更新
		m_model.Update();
	}

	void CBaby::UpdateLateProc()
	{
		m_damageHolder.Update(m_state.GetStateNumber());
		m_attackHolder.Update(m_state.GetStateNumber());
	}

	void CBaby::RenderProc()
	{
		Base::RenderProc();
		NMath::SMatrix44 matWorld;
		GetTransform().CalcWorldMatrix(matWorld);

		// シェーダーをパイプラインにセット
		g_shader->SetShaderToPipeline();
		g_shader->SetCullMode(NShader::ECullMode::FRONT);
		g_shader->SetVertexFormat(m_model.GetFVF());

		// デバイスへアクセス
		const auto& dev = NDirectX::GetD3DDevice();

		// 汎用行列変数
		NMath::SMatrix44	mat;
		D3DXMATRIX			matDX;

		// カメラ行列を設定
		NCamera::GetViewMatrix(mat);
		NDirectX::ToD3DMatrix(matDX, mat);
		g_shader->GetVSCT()->SetMatrix(dev, "g_view", &matDX);
		NCamera::GetProjMatrix(mat);
		NDirectX::ToD3DMatrix(matDX, mat);
		g_shader->GetVSCT()->SetMatrix(dev, "g_projection", &matDX);

		// 全メッシュを描画
		for (uint i = 0; i < m_model.GetMeshNodeCount(); ++i) {
			// 描画用行列を取得
			m_model.GetMeshNodeMatrix(mat, i, matWorld);

			// メッシュノードを取得
			const NModel::SMeshNode& meshNode = m_model.GetMeshNode(i);

			// 描画用行列を設定
			NDirectX::ToD3DMatrix(matDX, mat);
			g_shader->GetVSCT()->SetMatrix(dev, "g_world", &matDX);

			// テクスチャをセット
			uint samplerIndex = g_shader->GetPSCT()->GetSamplerIndex("g_sampler");
			g_shader->SetTexture(samplerIndex, meshNode.material.texture);

			// 描画
			m_model.RenderNode(i);

			// テクスチャをリセット
			g_shader->SetTexture(samplerIndex, nullptr);
		}

		g_shader->UnsetShaderFromPipeline();

		// 当たり判定のデバッグ描画
		m_actorMove.RenderDebug();
		m_damageHolder.RenderDebug();
		m_attackHolder.RenderDebug();
	}

	void CBaby::ProcDamage(const NGame::SDamageParam& _rDamage)
	{
		Base::ProcDamage(_rDamage);

		// 攻撃を耐える処理
		bool isEndure = false;
		if (m_state.GetState() == EState::ATTACK && 
			m_actorParam.GetIsAlive() && 
			_rDamage.receiveAttackStrengthType == NCollision::CAttackTypeData::EStrengthType::LOW) {

			isEndure = true;
		}
		
		if (!isEndure) {
			ApplyKnockBack(_rDamage);
			LookAtAttacker(_rDamage);
		}

		// 攻撃の強さタイプによる状態遷移と、サウンド再生
		switch (_rDamage.receiveAttackStrengthType) {
			case NCollision::CAttackTypeData::EStrengthType::LOW:
				if (!isEndure) {
					m_state.ChangeState(EState::DAMAGE_LOW);
				}
				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_LOW);
				break;
			case NCollision::CAttackTypeData::EStrengthType::MIDDLE:
				if (!isEndure) {
					m_state.ChangeState(EState::DAMAGE_LOW);
				}

				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_MIDDLE);

				break;
			case NCollision::CAttackTypeData::EStrengthType::HIGH:
				if (!isEndure) {
					m_state.ChangeState(EState::DAMAGE_HIGH);
				}
				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_HIGH);

				break;
			case NCollision::CAttackTypeData::EStrengthType::BLOW:
				break;
		}

		// 死亡時の状態遷移
		if (!m_actorParam.GetIsAlive()) {
			m_state.ChangeState(EState::DAMAGE_DIE);
		}
	}

	//----------------------------------------------------------

	void CBaby::Operate()
	{
		m_state.Update();
	}

	void CBaby::UpdateStateIdle(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(EAnmIndex::NORMAL_IDLE, true, 5, 1);
			m_idleFrame = 0;
		}

		// 一定時間経過で移動開始
		const int WAIT_FRAME = StaticCast<int>(2.5 * NSystem::FRAME_PER_SECOND);

		if (m_idleFrame > WAIT_FRAME) {
			m_state.ChangeState(EState::FOLLOW);
		}

		m_idleFrame++;
	}

	void CBaby::UpdateStateFollow(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(EAnmIndex::DAMAGE_L_BODY, true, 5, 1);
		}

		UpdateUtilFollowPlayer();

		// 攻撃範囲内に入ったら攻撃を開始
		if (CheckIsAttackRange()) {
			m_state.ChangeState(EState::ATTACK);
		}
	}

	void CBaby::UpdateStateAttack(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(EAnmIndex::ATTACK_HEAD_BAT, false, 3, 1);

			float3 myPos;
			GetTransform().GetPos(myPos);
			
			float3 otherPos;
			NGame::GetPlayer()->GetTransform().GetPos(otherPos);

			const float3 mask(1, 0, 1);
			float3 myPosXZ = myPos * mask;
			float3 otherPosXZ = otherPos * mask;
			float3 vecMyXZToOtherXZ = otherPosXZ - myPosXZ;

			if (vecMyXZToOtherXZ.GetLengthSquare() > FLT_EPSILON) {
				NMath::SMatrix44 posture;
				NMath::MakeMatrixLookAt(
					posture,
					myPosXZ,
					otherPosXZ,
					float3(0, 1, 0));

				// 姿勢部分のみ使用する(座標は元の値を設定)
				posture.vec[3].vec3 = myPos;
				GetTransform().SetLocalMatrix(posture);
			}
		}
		
		if (m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::IDLE);
		}
	}

	void CBaby::UpdateStateDamageLow(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(EAnmIndex::DAMAGE_L_HEAD, false, 0, 2);
		}

		if (m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::DAMAGE_RECOVER);
		}
	}

	void CBaby::UpdateStateDamageHigh(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(EAnmIndex::DAMAGE_L_BODY, false, 0, 1.5);
		}

		if (m_model.GetIsAnimationEnd()) {
			m_state.ChangeState(EState::DAMAGE_RECOVER);
		}
	}

	void CBaby::UpdateStateDamageRecover(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			// ダメージを食らった際に設定していた値を戻す
			SetNormalActorMoveParam();

			// ダメージ後の繊維
			m_state.ChangeState(EState::IDLE);
		}
	}

	void CBaby::UpdateStateDamageDie(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(EAnmIndex::DAMAGE_H_BLOW_BACK, false, 0, 2);
		}

		// 完全に死亡状態
		if (m_model.GetIsAnimationEnd()) {
			SetIsEliminated();
		}
	}

	//----------------------------------------------------------
	// 汎用操作
	//----------------------------------------------------------

	void CBaby::UpdateUtilFollowPlayer()
	{
		// 追従するオブジェクトへのベクトルを取得する
		float3	targetPos;
		m_pFollowObject->GetTransform().GetPos(targetPos);
		
		float3	myPos;
		GetTransform().GetPos(myPos);

		NMath::SVector3	vecToFollow = targetPos - myPos;

		// 方向へ移動
		vecToFollow.Normalize();
		m_actorMove.SetMoveDirXZ(vecToFollow);
		m_actorMove.AccelXZ();
	}

	//----------------------------------------------------------
	// 状態取得
	//----------------------------------------------------------
	bool CBaby::CheckIsAttackRange()
	{
		// 追従するオブジェクトへのベクトルを取得する
		float3	targetPos;
		m_pFollowObject->GetTransform().GetPos(targetPos);

		float3	myPos;
		GetTransform().GetPos(myPos);

		NMath::SVector3	vecToFollow = targetPos - myPos;

		// XZ平面での距離を取得
		float3	maskXZ(1, 0, 1);
		float3	vecToFollowXZ = vecToFollow * maskXZ;
		float	xzLengthSq = vecToFollowXZ.GetLengthSquare();	// 計算省略

		// Y軸方向の差を取得
		float3	maskY(0, 1, 0);
		float3	vecToFollowY = vecToFollow * maskY;
		float	yLength = abs(vecToFollow.y);
		
		// ---- 距離から攻撃範囲内に入ったかを判断する ----
		const float ATTACK_RANGE_XZ = 0.75f;
		const float ATTACK_RANGE_XZ_SQ = ATTACK_RANGE_XZ * ATTACK_RANGE_XZ;
		const float ATTACK_RANGE_Y = 0.1f;

		if (xzLengthSq < ATTACK_RANGE_XZ_SQ &&
			yLength < ATTACK_RANGE_Y) {
			return true;
		}

		return false;		
	}
}
