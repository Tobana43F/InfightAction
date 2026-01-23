#include	"Game_ActorMove.h"

#include	<string>
#include	"Collision\Collision_Common.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"
#include	"Object\Object_Transform.h"
#include	"Math\Math_Common.h"
#include	"Physics\Physics_Def.h"
#include	"System\System_Def.h"
#include	"System\System_Error.h"

#include	"DirectX\DirectX_DebugFont.h"

namespace {

	// 移動速度の最小値(これ以下は0とする)
	const float MOVE_SPEED_MIN = 0.5f * NSystem::DELTA_TIME;

	// Y軸方向の最大速度(デフォルト値)(バグ防止が主な目的)
	const float FALL_SPEED_MAX = -40.0f;
	const float JUMP_SPEED_MAX = 40.0f;

	// 地面の判定をするレイを飛ばすオフセット値(Y軸のみ使用する)
	const float3	GROUND_RAY_OFFSET(0, 1, 0);
	
}

namespace NGame {

	//----------------------------------------------------------
	// CActorMove::CActorBody
	//----------------------------------------------------------

	CActorMove::CActorBody::CActorBody() : 
		m_mass(1),
		m_sphereMesh(nullptr)
	{
		memset(&m_colBodys, 0, sizeof(m_colBodys));
		NDirectX::MakeMaterialData(m_materialEnable, float4(1, 1, 1, 0.125f));
	}

	CActorMove::CActorBody::~CActorBody()
	{
		ReleaseSafe(m_sphereMesh);
	}

	void CActorMove::CActorBody::Setup(json11::Json& _json)
	{
		// 重さを設定
		m_mass	= StaticCast<float>(_json["BodyMass"].number_value());

		// あたり判定情報を設定
		for (int i = 0; i < 3; ++i) {
			m_colBodys.centerPos.array[i] = StaticCast<float>(_json["BodyCollisionCenterPos"].array_items()[i].number_value());
		}
		m_colBodys.r = StaticCast<float>(_json["BodyCollisionRadius"].number_value());

		// デバッグ描画用メッシュ作成
		D3DXCreateSphere(
			NDirectX::GetD3DDevice(),
			m_colBodys.r,
			8,
			4,
			&m_sphereMesh,
			nullptr);
	}

	void CActorMove::CActorBody::RenderDebug(const float3& _worldPos)
	{
	#ifdef _GAME_DEBUG
		NCollision::SSphere colWorld;
		GetWorldCol(colWorld, _worldPos);

		NMath::SMatrix44	matWorld;
		matWorld.SetRow(3, colWorld.centerPos);

		D3DXMATRIX	d3dMatWorld;
		NDirectX::ToD3DMatrix(d3dMatWorld, matWorld);

		NDirectX::GetD3DDevice()->SetMaterial(&m_materialEnable);
		NDirectX::GetD3DDevice()->SetTexture(0, nullptr);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_WORLD, &d3dMatWorld);
		m_sphereMesh->DrawSubset(0);
	#endif
	}


	void CActorMove::CActorBody::GetWorldCol(
		NCollision::SSphere&	_dest,
		const float3&			_worldPos)
	{
		_dest.r = m_colBodys.r;
		_dest.centerPos = m_colBodys.centerPos + _worldPos;
	}

	//----------------------------------------------------------
	// CActorMove
	//----------------------------------------------------------

	CActorMove::CActorMove(NObject::CGameObject& _rGameObject) : 
		m_pGameObject(&_rGameObject),
		m_curAccelXZ(0),
		m_moveSpeedXZ(0),
		m_curAccelY(0),
		m_moveSpeedY(0),
		m_moveSpeedXZMaxRatio(1),
		m_isGrounded(false),
		m_isCollideWall(false),
		m_isJumpControl(false),
		m_isJumpFrame(false),
		m_isReleaseGround(true),	// 初期状態で離れている
		m_isApplyDeccel(true),
		m_isApplyRotateToMoveDir(true),
		m_isApplySpeedY(true),
		m_applyType(EApplyType::FORWARDING),
		m_colGroundIndex(NCollision::EStageCollisionIndex::GROUND_MAIN),
		m_colWallIndex(NCollision::EStageCollisionIndex::WALL_MAIN),
		m_pColMeshGround(nullptr),
		m_pColMeshWall(nullptr),
		m_constMoveSpeedXZMax(0),
		m_constMoveSpeedXZAcc(0),
		m_constMoveSpeedXZDecRatio(0),
		m_constMoveSpeedXZDecRatioAir(0),
		m_constFallSpeedMax(0),
		m_constJumpSpeedMax(0),
		m_constGravityScale(0),
		m_constDeltaGravity(0)
	{


	}

	CActorMove::~CActorMove()
	{

	}

	void CActorMove::Setup(
		NAsset::CJsonFactory::Key	_key,
		NCollision::EStageCollisionIndex	_colGroundIndex,
		NCollision::EStageCollisionIndex	_colWallIndex)
	{
		// あたり判定用コリジョン設定
		m_colGroundIndex = _colGroundIndex;
		m_colWallIndex = _colWallIndex;

		// データ読み込み
		try {
			NAsset::CJsonFactory::SPAsset pJson;
			pJson = NAsset::CJsonFactory::GetInst().Load(_key);
			LoadConstant(*pJson);
			m_body.Setup(*pJson);
		}
		catch (const char* _err) {
			std::string	err = "";
			err += "ActorMoveのSetupでエラーが起きました。\n";
			err += "FilePath : ";
			err += _key.data();
			err += "\n\n [Message]\n";
			err += _err;
		}
	}

	void CActorMove::Init()
	{
		// パラメータ初期化
		m_moveSpeedXZ = 0;
		m_moveSpeedY = 0;

		// 座標関係の変数をすべてオブジェクトの位置で初期化する
		m_pGameObject->GetTransform().GetPos(m_prevPos);
		m_posAfterApplyMoveXZ = m_prevPos;
		m_posAfterApplyMoveY = m_prevPos;
		m_posAfterUpdate = m_prevPos;

		// ステージのあたり判定を再設定
		m_pColMeshGround = NCollision::GetStageCollision(m_colGroundIndex);
		m_pColMeshWall = NCollision::GetStageCollision(m_colWallIndex);

		Assert(m_pColMeshGround != nullptr && "地面のあたり判定がnullptrでした。");
		Assert(m_pColMeshWall != nullptr && "壁のあたり判定がnullptrでした。");

	}

	void CActorMove::UpdateFast()
	{
		m_pGameObject->GetTransform().GetPos(m_prevPos);
	}

	void CActorMove::Update()
	{
		// ジャンプの制御を開始
		UpdateJump();

		// 重力加速
		AccelGravity();

		// 移動を適用とステージとのあたり判定
		ApplyMoveXZ();
		CorrectWallXZ();
		ApplyMoveY();
		CorrectGroundY();
		RotateMoveDirWithSpeed();
		ApplyMovement();
	}

	void CActorMove::RenderDebug()
	{
	#ifdef _GAME_DEBUG
		m_body.RenderDebug(m_posAfterUpdate);
	#endif
	}

	void CActorMove::AccelXZ(float _ratio)
	{
		// 最大値でクランプ
		if (_ratio > 1.0f) {
			_ratio = 1.0f;
		}
		// 最小値でクランプ
		else if (_ratio < -1.0f) {
			_ratio = -1.0f;
		}

		// 加速度を設定
		m_curAccelXZ += m_constMoveSpeedXZAcc * _ratio;
	}
	
	void CActorMove::AccelXZDirect(const float _accelDelta)
	{
		m_curAccelXZ += _accelDelta;
	}

	void CActorMove::AccelYDirect(const float _accelDelta)
	{
		m_curAccelY += _accelDelta;
	}

	void CActorMove::Jump(const float _jumpPow)
	{
		m_moveSpeedY = 0;
		m_curAccelY = _jumpPow;
		m_isJumpFrame = true;
		m_isReleaseGround = false;
	}

	void CActorMove::SetMove(const float3& _dir, const float _speed)
	{
		// XZ平面に分解
		const float3 maskXZ(1, 0, 1);
		float3	dirXZ = _dir * maskXZ;
		SetMoveDirXZ(dirXZ);
		SetMoveSpeedXZ(dirXZ.GetLength() * _speed);

		// Y軸方向に分解
		SetMoveSpeedY(_dir.y * _speed);

	}

	void CActorMove::SetMoveDirXZ(const float3& _moveDir)
	{
		// XZ平面にする
		const float3 mask(1, 0, 1);
		float3		moveDirXZ = _moveDir * mask;

		if (moveDirXZ.GetLengthSquare() < FLT_EPSILON) {
			return;
		}

		// 正規化
		moveDirXZ.Normalize();

		// 値を格納
		m_moveDirXZ = moveDirXZ;
	}

	void CActorMove::SetMoveSpeedXZRatio(const float _ratio)
	{
		float ratio = _ratio;;
		if (ratio < 0) {
			ratio = 0;
		}
		else if (ratio > 1) {
			ratio = 1;
		}

		m_moveSpeedXZMaxRatio = ratio;
	}

	float	CActorMove::GetMoveSpeed() const
	{
		float3 moveSpeed;
		moveSpeed = 
			m_moveDirXZ * m_moveSpeedXZ +
			float3(0, 1, 0) * m_moveSpeedY;
		return moveSpeed.GetLength();
	}

	//----------------------------------------------------------

	void CActorMove::LoadConstant(json11::Json& _json) 
	{
		m_constMoveSpeedXZMax = StaticCast<float>(_json["MoveSpeedXZMax"].number_value() * NSystem::DELTA_TIME);
		m_constMoveSpeedXZAcc = StaticCast<float>(_json["MoveSpeedXZAcc"].number_value() * NSystem::DELTA_TIME);
		m_constMoveSpeedXZDecRatio = StaticCast<float>(_json["MoveSpeedXZDecRatio"].number_value());
		m_constMoveSpeedXZDecRatioAir = StaticCast<float>(_json["MoveSpeedXZDecRatioAir"].number_value());
		m_constGravityScale = StaticCast<float>(_json["GravityScale"].number_value());
		m_constDeltaGravity = (NPhysics::GRAVITY_ACC.y * NSystem::DELTA_TIME) * m_constGravityScale;

		m_constFallSpeedMax = FALL_SPEED_MAX * NSystem::DELTA_TIME;
		m_constJumpSpeedMax = JUMP_SPEED_MAX * NSystem::DELTA_TIME;

		m_constRotSpeedMaxDeg = StaticCast<float>(_json["RotSpeedMaxDeg"].number_value() * NSystem::DELTA_TIME);
	}

	void CActorMove::UpdateJump()
	{
		// ジャンプ開始時の初期化
		if (m_isJumpFrame) {
			m_isJumpControl = true;
			m_isReleaseGround = false;

			// トリガー初期化
			m_isJumpFrame = false;
		}

		// ジャンプ制御中フラグ
		if (m_isJumpControl) {

			// ジャンプ後に地面から離れていない間
			if (!m_isReleaseGround) {

				// 地面から離れたかをチェック
				if (!m_isGrounded) {
					m_isReleaseGround = true;
					m_isJumpControl = false;	// ジャンプの制御を終了
				}
			}
		}
		// ジャンプ制御中でない場合はフラグを初期化する
		else {
			//m_isReleaseGround = true;
		}
	}


	void CActorMove::AccelGravity()
	{
		m_curAccelY += m_constDeltaGravity;
	}

	void CActorMove::ApplyMoveXZ()
	{
		// 加速量がない場合は減速する
		if (abs(m_curAccelXZ) < FLT_EPSILON) {

			float decRatio = 1.0f;

			// 着地状態により減速率を変える
			if (m_isGrounded) {
				decRatio = m_constMoveSpeedXZDecRatio;
			}
			else {
				decRatio = m_constMoveSpeedXZDecRatioAir;
			}

			// 減速を適用するかを判断する
			//!	@note	インデントを増やさないためここで行った
			if (!m_isApplyDeccel) {
				decRatio = 1.0f;
			}

			// 原則を適用
			m_moveSpeedXZ *= decRatio;

			// 停止を判断
			if (abs(m_moveSpeedXZ) < MOVE_SPEED_MIN) {
				m_moveSpeedXZ = 0;
			}
		}
		else {
			// 加速度を移動量に適用
			m_moveSpeedXZ += m_curAccelXZ;

			// 最大側でクランプ
			float moveSpeedMax = m_constMoveSpeedXZMax * m_moveSpeedXZMaxRatio;
			if (m_moveSpeedXZ > moveSpeedMax) {
				m_moveSpeedXZ = moveSpeedMax;
			}
			else if (m_moveSpeedXZ < -moveSpeedMax) {
				m_moveSpeedXZ = -moveSpeedMax;
			}
		}

		// 前座標からXZの移動量を適用した値を計算
		m_posAfterApplyMoveXZ = m_prevPos + (m_moveDirXZ * m_moveSpeedXZ);

		// 現在の加速量を初期化
		m_curAccelXZ = 0;
	}

	void CActorMove::CorrectWallXZ()
	{
		// 壁用の当たり判定が設定されていない場合は無視
		if (m_pColMeshWall == nullptr) { return; }

		bool sts = false;
		float3 pushback;
		NCollision::SSphere sphere;
		m_body.GetWorldCol(sphere, m_posAfterApplyMoveXZ);
		sts = NCollision::CheckSphereAndMeshWall(
			sphere,
			*m_pColMeshWall,
			pushback);
		if (sts) {
			m_posAfterApplyMoveXZ += pushback;
			m_isCollideWall = true;
		}
		else {
			m_isCollideWall = false;
		}
	}

	void CActorMove::ApplyMoveY()
	{
		// 加速度を移動速度に適用
		m_moveSpeedY += m_curAccelY;
		
		// スピード最大速度でクランプ
		// 落下(-方向)をクランプ
		if (m_moveSpeedY < m_constFallSpeedMax) {
			m_moveSpeedY = m_constFallSpeedMax;
		}
		else if (m_moveSpeedY > m_constJumpSpeedMax) {
			m_moveSpeedY = m_constJumpSpeedMax;
		}

		//!	@brief	スピードを適用しない場合はスピードを0にする
		if (!m_isApplySpeedY) {
			m_moveSpeedY = 0;
		}

		// スピードを適用した後の座標を取得
		m_posAfterApplyMoveY = m_posAfterApplyMoveXZ + (float3(0, 1, 0) * m_moveSpeedY);

		// 最終的な座標として保持
		m_posAfterUpdate = m_posAfterApplyMoveY;

		// 現在の加速量を初期化
		m_curAccelY = 0;
	}

	void CActorMove::CorrectGroundY()
	{
		// 地面のメッシュが設定されていない場合は無視
		if (m_pColMeshGround == nullptr) { return; }

		// 地面方向に向けて飛ばすレイの情報を作成
		const float3			rayStartPos = m_posAfterApplyMoveXZ + GROUND_RAY_OFFSET;
		const NMath::SVector3	vecRayStartToPos = m_posAfterApplyMoveY - rayStartPos;

		// 地面との当たり判定を実行
		bool	sts = false;
		float3	crossPos;
		sts = m_pColMeshGround->SerchGroundOrigToDirection(
			crossPos,
			rayStartPos,
			vecRayStartToPos.GetLengthSquare());

		// 地面との当たり判定
		if (sts) {
			m_isGrounded = true;

			// 地面の高さに補正された値を適用
			m_posAfterUpdate = crossPos;
		}
		else {
			m_isGrounded = false;
		}

		// 値を適用
		//m_pGameObject->GetTransform().SetPos(m_posAfterUpdate);
	}

	void CActorMove::RotateMoveDirWithSpeed()
	{
		if (!m_isApplyRotateToMoveDir) { 
			m_curMatRot.SetIdentity();
			return; 
		}

		const float3	maskXZ(1, 0, 1);
		NMath::SVector3	objDirXZ = m_pGameObject->GetTransform().GetForward();
		objDirXZ.Normalize();
		objDirXZ *= maskXZ;

		//----
		// 回転量行列を作成
		float diffRotCos = NMath::Dot(objDirXZ, m_moveDirXZ);
		float diffRotRad = acos(diffRotCos);

		// 移動量のクオータニオンを作成
		D3DXQUATERNION	qt;	// 回転の移動量

		float dot = 0;
		NMath::SQuaternion	qtMoveRot;
		NMath::MakeQuaternionRotationArc(
			qtMoveRot,
			objDirXZ,
			m_moveDirXZ,
			&dot);

		float speedRatio = m_moveSpeedXZ / m_constMoveSpeedXZMax;
		float rad = NMath::ArcCos(dot) * speedRatio;
		if (rad > NMath::ToRad(m_constRotSpeedMaxDeg)) {
			rad = NMath::ToRad(m_constRotSpeedMaxDeg);
		}
		if (m_moveSpeedXZ <= FLT_EPSILON) {
			rad = 0;
		}

		NMath::SVector3	dirVec;
		float dir = 0;
		NMath::Cross(dirVec, objDirXZ, m_moveDirXZ);
		dir = dirVec.y;
		if (dir < 0) {
			rad = -rad;
		}

		const D3DXVECTOR3 ROT_AXIS(0, 1, 0);
		D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)&ROT_AXIS, rad);

		// 現在の姿勢のクオータニオンを作成
		NMath::SMatrix44 matWorld;
		matWorld = m_pGameObject->GetTransform().GetLocalMatrix();
		D3DXMATRIX	matWorldDX;
		NDirectX::ToD3DMatrix(matWorldDX, matWorld);
		D3DXQUATERNION	qtCur;
		D3DXQuaternionIdentity(&qtCur);
		D3DXQuaternionRotationMatrix(
			&qtCur,
			&matWorldDX);

		D3DXMATRIX	mat;
		D3DXMatrixRotationQuaternion(&mat, &qt);

		NMath::SMatrix44 matRot;
		NDirectX::ToSMatrix44(matRot, mat);

		//----------------------------------------------------------
		// 変化量を取得
		m_curMatRot = matRot;

		//matWorld = matCur * matWorld;
		//m_pGameObject->GetTransform().SetLocalMatrix(matWorld);
	}

	void CActorMove::ApplyMovement()
	{
		if (m_applyType == EApplyType::FORWARDING) {
			// Y軸移動量はそのまま適用する
			float3	finalY = m_prevPos;
			finalY.y = m_posAfterUpdate.y;
			m_pGameObject->GetTransform().SetPos(finalY);

			// 移動量
			float3	mask(1, 0, 1);
			float3	curMoveXZ = (m_posAfterUpdate - m_prevPos) * mask;

			float3	forward(0, 0, 1);
			float	finalSpeedXZ = curMoveXZ.GetLength();

			// 移動量を行列で取得
			NMath::MakeMatrixTranslation(m_curMatTrans, forward * finalSpeedXZ);

			//
			NMath::SMatrix44	matCur = m_curMatRot * m_curMatTrans;
			NMath::SMatrix44	matLocal = m_pGameObject->GetTransform().GetLocalMatrix();
			matLocal = matCur * matLocal;
			m_pGameObject->GetTransform().SetLocalMatrix(matLocal);

			// 補正後の値を設定
			if (m_isCollideWall) {
				m_pGameObject->GetTransform().SetPos(m_posAfterUpdate);
			}
		}
		else if (m_applyType == EApplyType::DIRECT) {
			m_pGameObject->GetTransform().SetPos(m_posAfterUpdate);

			NMath::SMatrix44	matLocal = m_pGameObject->GetTransform().GetLocalMatrix();
			NMath::SMatrix44	matNew = m_curMatRot * matLocal;
			m_pGameObject->GetTransform().SetLocalMatrix(matNew);
		}
	}
}
