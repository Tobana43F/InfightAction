#include	"Object_Player.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"Camera\Camera_OperatorManager.h"
#include	"Collision\Manager\Collision_Manager.h"
#include	"Collision\CollisionProc\Collision_HitTest.h"
#include	"Input\Input_Game.h"
#include	"System\System_Def.h"
#include	"Sound\Sound_Common.h"
#include	"Util\Util_GameCommon.h"

namespace NObject {

	CPlayer::CPlayer() : 
		m_stateBig(this, EBigState::NORMAL),
		m_isSwordGrab(false),
		m_normalState(*this),
		m_damageState(*this),
		m_attackState(*this),
		m_isResetLife(true)
	{

		m_stateBig.Register(EBigState::NORMAL, &CPlayer::UpdateStateNormal);
		m_stateBig.Register(EBigState::DAMAGE, &CPlayer::UpdateStateDamage);
		m_stateBig.Register(EBigState::ATTACK, &CPlayer::UpdateStateAttack);
	}

	CPlayer::~CPlayer()
	{
		// 登録したカメラを解除		
		NCamera::UnregisterOperator(&m_cameraOperator);
	}

	void CPlayer::LoadProc()
	{
		CActor::LoadProc();

		// シェーダーの読み込み
		std::string	key = "AlphaTexture.hlsl";
		NAsset::CShaderFactory::GetInst().SetLoadInfo(
			key,
			"VS",
			"PS");
		m_shader = NAsset::CShaderFactory::GetInst().Load(key);

		// モデル読み込み
		m_model.Load("Player/Model.fbx");

		m_swordModel.Load("Sword/Model.fbx");

		// カメラ初期化
		m_cameraOperator.Setup(
			this,
			&m_moveSpeedAlias,
			m_actorMove.GetMoveSpeedXZMax());

		// カメラ登録
		NCamera::RegisterOperator(&m_cameraOperator);

		//----------------------------------------------------------
		// 状態遷移関係
		//----------------------------------------------------------
		m_normalState.Load();
		m_damageState.Load();
		m_attackState.Load();

		//----------------------------------------------------------
		// 内包するオブジェクトの初期化
		//----------------------------------------------------------

		m_swordTransform.GetTransform().SetParent(&GetTransform());
	}	

	void CPlayer::ReleaseProc()
	{
		CActor::ReleaseProc();
	}

	void CPlayer::InitProc()
	{
		// 初期化前のライフを取得
		auto life = m_actorParam.life;

		CActor::InitProc();

		// リセットしない場合は元のライフを再度設定
		if (!m_isResetLife) {
			m_actorParam.life = life;
		}

		m_moveSpeedAlias = 0;
		
		GetTransform().SetRot(NMath::SMatrix44());	// 正面
		GetTransform().SetPos(0, 10, 0);
		m_stateBig.ChangeState(EBigState::NORMAL);
		m_isSwordGrab = false;

		m_cameraOperator.Init();

		m_normalState.Init();
		m_damageState.Init();
		m_attackState.Init();
	}

	void CPlayer::DestroyProc()
	{
		CActor::DestroyProc();
	}

	void CPlayer::UpdateProc()
	{
		CActor::UpdateProc();

		Operate();

		m_model.Update();

		// 剣の位置を表すゲームオブジェクトを作成する
		if (m_isSwordGrab) {
			NMath::SMatrix44	matSwordBone;
			m_model.GetClusterMatrix(matSwordBone, BONE_RIGHT_HAND_WEAPON);
			m_swordTransform.GetTransform().SetLocalMatrix(matSwordBone);
		}
		else {
			NMath::SMatrix44	matSwordBone;
			m_model.GetClusterMatrix(matSwordBone, BONE_SWORD_HOLDER);
			m_swordTransform.GetTransform().SetLocalMatrix(matSwordBone);
		}

		// 移動の更新
		UpdateOperatorParam();

		// あたり判定の更新
		m_effect.Update();
	}

	void CPlayer::UpdateLateProc()
	{
		// 状態の排他制御
		UpdateExcludeState();

		// 当たり判定の更新
		m_normalState.UpdateLate();
		m_attackState.UpdateLate();
		m_damageState.UpdateLate();
	}

	void CPlayer::RenderProc()
	{
		CActor::RenderProc();
		NMath::SMatrix44 matWorld;
		GetTransform().CalcWorldMatrix(matWorld);

		// シェーダーをパイプラインにセット
		m_shader->SetShaderToPipeline();
		m_shader->SetCullMode(NShader::ECullMode::FRONT);
		m_shader->SetVertexFormat(m_model.GetFVF());

		// デバイスへアクセス
		const auto& dev = NDirectX::GetD3DDevice();

		// 汎用行列変数
		NMath::SMatrix44	mat;
		D3DXMATRIX			matDX;

		// カメラ行列を設定
		NCamera::GetViewMatrix(mat);
		NDirectX::ToD3DMatrix(matDX, mat);
		m_shader->GetVSCT()->SetMatrix(dev, "g_view", &matDX);
		NCamera::GetProjMatrix(mat);
		NDirectX::ToD3DMatrix(matDX, mat);
		m_shader->GetVSCT()->SetMatrix(dev, "g_projection", &matDX);

		// 全メッシュを描画
		for (uint i = 0; i < m_model.GetMeshNodeCount(); ++i) {
			// 描画用行列を取得
			m_model.GetMeshNodeMatrix(mat, i, matWorld);

			// メッシュノードを取得
			const NModel::SMeshNode& meshNode = m_model.GetMeshNode(i);

			// 描画用行列を設定
			NDirectX::ToD3DMatrix(matDX, mat);
			m_shader->GetVSCT()->SetMatrix(dev, "g_world", &matDX);

			// テクスチャをセット
			uint samplerIndex = m_shader->GetPSCT()->GetSamplerIndex("g_sampler");
			m_shader->SetTexture(samplerIndex, meshNode.material.texture);

			// 描画
			m_model.RenderNode(i);

			// テクスチャをリセット
			m_shader->SetTexture(samplerIndex, nullptr);
		}

		m_shader->UnsetShaderFromPipeline();


		//----------------------------------------------------------
		// それ以外のオブジェクトの描画

		m_normalState.RenderDebug();
		m_attackState.RenderDebug();
		m_damageState.RenderDebug();

		NMath::SMatrix44	matSwordWorld;
		m_swordTransform.GetTransform().CalcWorldMatrix(matSwordWorld);
		m_swordModel.RenderDebug(matSwordWorld);
		m_effect.Render();
	}

	void CPlayer::Operate()
	{
		// 全ての状態に共通の処理
		UpdateUtilFall();
		m_actorMove.SetMoveSpeedXZRatio(1);	// 他の場所で設定している値を初期化する

		m_stateBig.Update();
	}

	void CPlayer::ProcDamage(const NGame::SDamageParam& _rDamage)
	{
		Base::ProcDamage(_rDamage);

		ApplyKnockBack(_rDamage);
		LookAtAttacker(_rDamage);

		// 地上と空中で処理を分岐
		// 攻撃の強さタイプによる状態遷移と、サウンド再生
		switch (_rDamage.receiveAttackStrengthType) {
			case NCollision::CAttackTypeData::EStrengthType::LOW:
				if (GetIsGrounded()) {
					m_damageState.Entry(CDamageState::EState::GROUND_LOW);
				}
				else {
					m_damageState.Entry(CDamageState::EState::AIR);
				}
				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_LOW);
				break;
			case NCollision::CAttackTypeData::EStrengthType::MIDDLE:
				if (GetIsGrounded()) {
					m_damageState.Entry(CDamageState::EState::GROUND_MIDDLE);
				}
				else {
					m_damageState.Entry(CDamageState::EState::AIR);
				}
				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_MIDDLE);

				break;
			case NCollision::CAttackTypeData::EStrengthType::HIGH:
				if (GetIsGrounded()) {
					m_damageState.Entry(CDamageState::EState::GROUND_HIGH);
				}
				else {
					m_damageState.Entry(CDamageState::EState::AIR);
				}

				// 攻撃音を鳴らす
				NSound::Play(NSound::ELabel::SE_ATTACK_SWORD_HIGH);

				break;
			case NCollision::CAttackTypeData::EStrengthType::BLOW:
				break;
		}

		// 死亡時の状態遷移
		if (!m_actorParam.GetIsAlive()) {
			m_damageState.Entry(CDamageState::EState::DIE);
		}
	}

	//----------------------------------------------------------

	void CPlayer::UpdateOperatorParam()
	{
		m_moveSpeedAlias = m_actorMove.GetMoveSpeedXZ();
	}

	void CPlayer::UpdateExcludeState()
	{
		// 一度全て無効にする
		m_normalState.SetIsEnable(false);
		m_attackState.SetIsEnable(false);
		m_damageState.SetIsEnable(false);

		switch (m_stateBig.GetState()) {
			case EBigState::NORMAL:
				m_normalState.SetIsEnable(true);
				break;

			case EBigState::ATTACK:
				m_attackState.SetIsEnable(true);
				break;

			case EBigState::DAMAGE:
				m_damageState.SetIsEnable(true);
				break;
		}
	}

	//----------------------------------------------------------

	void CPlayer::UpdateUtilMove()
	{
		float2 analog;
		analog.x = NInput::GetLeftAnalogX();
		analog.y = NInput::GetLeftAnalogY();

		// 入力方向のベクトルを取得
		NMath::SVector3	inputDir;
		inputDir.x = analog.x;
		inputDir.y = analog.y;
		inputDir.Normalize();

		// カメラから見たベクトルに変換
		NMath::SVector3	inputDirFromCam;
		NUtil::ToVectorFromCameraXZ(
			inputDirFromCam,
			inputDir);

		// 適用
		m_actorMove.SetMoveDirXZ(inputDirFromCam);

		if (inputDir.GetLengthSquare() > FLT_EPSILON) {
			m_actorMove.SetMoveSpeedXZRatio(analog.GetLength());
			m_actorMove.AccelXZ();
		}
	}

	void CPlayer::UpdateUtilFall()
	{
		// 落下中、空中、ダメージ処理以外、である場合、落下状態に遷移
		if (m_actorMove.GetMoveSpeedY() < 0 && 
			!GetIsGrounded() && 
			m_stateBig.GetState() != EBigState::DAMAGE && 
			m_stateBig.GetState() != EBigState::ATTACK) {

			m_normalState.Entry(CNormalState::EState::JUMP_DOWN);
		}
	}

	//----------------------------------------------------------

	bool CPlayer::GetIsStop() const
	{
		if (m_actorMove.GetMoveSpeedXZ() < FLT_EPSILON) {
			return true;
		}
		else {
			return false;
		}
	}

	bool CPlayer::GetIsGrounded() const
	{
		if (m_actorMove.GetIsGrounded()) {
			return true;
		}
		else {
			return false;
		}
	}

	bool CPlayer::GetIsFalling() const
	{
		if (m_actorMove.GetMoveSpeedY() < 0) {
			return true;
		}
		else {
			return false;
		}
	}

	float	CPlayer::GetMoveSpeedXZRatio() const
	{
		float cur = m_actorMove.GetMoveSpeedXZ();
		float max = m_actorMove.GetMoveSpeedXZMax();
		return cur / max;
	}

}

