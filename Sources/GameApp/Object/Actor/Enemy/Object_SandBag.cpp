#include	"Object_SandBag.h"

#include	"Asset\Factory\Asset_ShaderFactory.h"
#include	"Camera\Camera_GlobalManager.h"
#include	"Math\Math_Common.h"
#include	"Math\Math_Util.h"

#include	"Collision\CollisionProc\Collision_HitTest.h"
#include	"Sound\Sound_Common.h"

namespace {
	NAsset::CShaderFactory::SPAsset	g_shader;
}

namespace NObject {

	CSandBag::CSandBag() :
		m_state(this, ST_NORMAL_IDLE)
	{
		m_state.Register(ST_NORMAL_IDLE, &CSandBag::UpdateIdleState);
		m_state.Register(ST_NORMAL_DIE, &CSandBag::UpdateDie);
		m_state.Register(ST_DAMAGE_L_HEAD, &CSandBag::UpdateDamageLight);
		m_state.Register(ST_DAMAGE_L_BODY, &CSandBag::UpdateDamageLight);
		m_state.Register(ST_DAMAGE_L_FOOT, &CSandBag::UpdateDamageLight);
		m_state.Register(ST_DAMAGE_H_BLOW_BACK, &CSandBag::UpdateDamageHeavy);

		m_actorMove.SetIsApplyRotateMoveDir(false);
		m_actorMove.SetApplyType(NGame::CActorMove::EApplyType::DIRECT);
	}

	CSandBag::~CSandBag()
	{

	}


	//==========================================================
	// 基本的な関数群
	//==========================================================

	void CSandBag::LoadProc()
	{
		Base::LoadProc();

		// モデルの読み込み
		m_model.Load("Enemy/SandBag/Model.fbx");

		// シェーダーの読み込み
		std::string	key = "AlphaTexture.hlsl";
		NAsset::CShaderFactory::GetInst().SetLoadInfo(
			key,
			"VS",
			"PS");
		g_shader = NAsset::CShaderFactory::GetInst().Load(key);

		// 当たり判定読み込み
		const char* pDamagePaths[EState::ST_MAX] = {
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Common/None.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
			"Asset/Data/Collision/Enemy/SandBag/Damage/Idle.json",
		};

		m_damageHolder.Load(
			this,
			EState::ST_MAX,
			pDamagePaths,
			&m_model);

		// 登録
		auto pPAED = NCollision::GetHitTestProc(NCollision::EHitTestProcManagerIndex::PLAYER_ATK_ENEMY_DEF);
		pPAED->Register(m_damageHolder, NCollision::CProcHitTest::EType::DEFENDER);

		// ランダムでおおきさ
		const float SCALE_MIN = 1;
		const float SCALE_MAX = 3;
		const float SCALE_DIFF = SCALE_MAX - SCALE_MIN;
		int			rand100 = (rand() % 101);
		float		ratio = StaticCast<float>(rand100) / 100.0f;

		float scale = SCALE_MIN + (SCALE_DIFF * ratio);
		GetTransform().SetScale(scale);
	}

	void CSandBag::ReleaseProc()
	{
		Base::ReleaseProc();
	}

	void CSandBag::InitProc()
	{
		Base::InitProc();

		m_state.ChangeState(ST_NORMAL_IDLE);
	}

	void CSandBag::DestroyProc()
	{
		Base::DestroyProc();
	}

	void CSandBag::UpdateProc()
	{
		Base::UpdateProc();

		// モデルのアニメーションを更新
		m_model.Update();
	}

	void CSandBag::UpdateLateProc()
	{
		m_damageHolder.Update(m_state.GetStateNumber());
	}

	void CSandBag::RenderProc()
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
	}

	void CSandBag::ProcDamage(const NGame::SDamageParam& _rDamage)
	{
		Base::ProcDamage(_rDamage);

		ApplyKnockBack(_rDamage);
		LookAtAttacker(_rDamage);

		// 攻撃の強さタイプによる状態遷移と、サウンド再生
		switch (_rDamage.receiveAttackStrengthType) {
			case NCollision::CAttackTypeData::EStrengthType::LOW:
				m_state.ChangeState(ST_DAMAGE_L_BODY);
				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_LOW);
				break;
			case NCollision::CAttackTypeData::EStrengthType::MIDDLE:
				m_state.ChangeState(ST_DAMAGE_L_FOOT);

				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_MIDDLE);

				break;
			case NCollision::CAttackTypeData::EStrengthType::HIGH:
				m_state.ChangeState(ST_DAMAGE_L_HEAD);
				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_HIGH);

				break;
			case NCollision::CAttackTypeData::EStrengthType::BLOW:
				break;
		}

		// 死亡時の状態遷移
		if (!m_actorParam.GetIsAlive()) {
			m_state.ChangeState(ST_NORMAL_DIE);
		}
	}


	//----------------------------------------------------------

	void CSandBag::Operate()
	{
		m_state.Update();
	}

	void CSandBag::UpdateIdleState(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(ANM_NORMAL_IDLE, true, 5, 1);
		}
	}

	void CSandBag::UpdateDamageLight(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(ANM_DAMAGE_L_HEAD, false, 0, 3);
		}

		if (m_model.GetIsAnimationEnd()) {
			_param.ChangeState(ST_NORMAL_IDLE);
		}
	}

	void CSandBag::UpdateDamageHeavy(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(ANM_DAMAGE_H_BLOW_BACK, false, 0, 2);
		}

		if (m_model.GetIsAnimationEnd()) {
			_param.ChangeState(ST_NORMAL_IDLE);
		}
	}

	void CSandBag::UpdateDie(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_model.ChangeAnimation(ANM_DAMAGE_H_BLOW_BACK, false, 0, 2);
		}

		// 完全に死亡状態に消滅
		if (m_model.GetIsAnimationEnd()) {
			SetIsEliminated();
		}
	}
}