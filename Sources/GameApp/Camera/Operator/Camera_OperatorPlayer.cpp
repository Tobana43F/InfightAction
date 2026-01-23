#include	"Camera_OperatorPlayer.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"Debug\Debug_Assert.h"
#include	"Input\Input_Game.h"
#include	"Math\Math_Common.h"
#include	"System\System_Def.h"

#include	"Util\Util_GameCommon.h"

namespace NCamera {

	COperatorPlayer::COperatorPlayer() : 
		m_isSetupOK(false),
		m_pFocusGameObject(nullptr)
	{

	}

	COperatorPlayer::~COperatorPlayer()
	{
		m_pFocusGameObject = nullptr;
	}

	void COperatorPlayer::Setup(
		const NObject::CGameObject*	_pFocusGameObject,
		const float*				_pMoveSpeed,
		const float					_playerMoveSpeedMax)
	{
		Assert(!m_isSetupOK);
		m_isSetupOK = true;
		m_pFocusGameObject		= _pFocusGameObject;
		m_pMoveSpeed			= _pMoveSpeed;
		m_playerMoveSpeedMax	= _playerMoveSpeedMax;
		Init();
	}

	void COperatorPlayer::Init()
	{
		AssertNotNull(m_pFocusGameObject);
		m_targetCameraLocalPos = float3(0, 2.0f, -3);	// オブジェクトの後ろから見る
		m_focusOffset = float3(0, 1.4f, 0);				// 頭のあたりを注目するようなオフセット

		InitVariable();
	}

	//----------------------------------------------------------

	void COperatorPlayer::Operate()
	{
		Assert(m_isSetupOK);
		AssertNotNull(m_pFocusGameObject);

		RotateCameraInput();
		RotateCameraPlayerMove();
	}

	//----------------------------------------------------------

	void COperatorPlayer::RotateCameraPlayerMove()
	{
		// カメラの正面方向のベクトルを取得する
		float3 objPos;
		m_pFocusGameObject->GetTransform().GetPos(objPos);
		float3 camPosLocal = m_targetCameraLocalPos;
		float3 camPosWorld;
		float3 camFoc;
		camPosWorld = camPosLocal + objPos;
		GetFocusPos(camFoc);
		NMath::SMatrix44 matCamInv;
		NCamera::GetViewMatrix(matCamInv);
		matCamInv.Transpose();
		NMath::SVector3	camForward = matCamInv.GetRow(2);
		NMath::SVector3	camRight = matCamInv.GetRow(0);
		NMath::SVector3	camForwardXZ = camForward * float3(1, 0, 1);
		NMath::SVector3	camRightXZ = camRight * float3(1, 0, 1);
		camForwardXZ.Normalize();
		camRightXZ.Normalize();

		// 移動方向を取得
		NMath::SVector2	inputDir;
		inputDir.x = NInput::GetLeftAnalogX();
		inputDir.y = NInput::GetLeftAnalogY();
		inputDir.Normalize();

		// 移動速度の割合
		float speedRatio = 
			(*m_pMoveSpeed) / 
			(m_playerMoveSpeedMax);

		// 減速(X方向への入力が少ない場合は減速) 
		if (abs(inputDir.x) < 0.125f) {
			m_rotateSpeedPlayerMove *= ROTATE_SPEED_DEG_PLAYER_MOVE_DEC_RATIO;
		}

		// X軸の移動速度を取得
		float speedX = inputDir.x;
		if (inputDir.y < -0.5) {
			//speedX *= -1;
		}

		m_rotateSpeedPlayerMove +=
			speedX * 
			(ROTATE_SPEED_DEG_PLAYER_MOVE_ACC * NSystem::DELTA_TIME);

		// 最大値でクランプ
		float rotateSpeedMaxPerSec = 
			ROTATE_SPEED_DEG_PLAYER_MOVE_MAX * 
			NSystem::DELTA_TIME * 
			abs(speedRatio);

		if (m_rotateSpeedPlayerMove > rotateSpeedMaxPerSec) {
			m_rotateSpeedPlayerMove = rotateSpeedMaxPerSec;
		}
		else if (m_rotateSpeedPlayerMove < -rotateSpeedMaxPerSec) {
			m_rotateSpeedPlayerMove = -rotateSpeedMaxPerSec;
		}

		// 入力で操作中は補正しない
		if (!m_isOperatingWithStick) {
			m_rotateSpeedSum += m_rotateSpeedPlayerMove;
		}

		// カメラプレイヤー周りに回転
		NMath::SMatrix44 matRot, matTrans, matTransObj;
		// 回転量の行列を作成
		NMath::MakeMatrixRotationY(matRot, m_rotateSpeedSum);

		// 現在のカメラの移動量行列を作成
		NMath::MakeMatrixTranslation(matTrans, camPosLocal);

		// オブジェクト位置への移動行列
		NMath::MakeMatrixTranslation(matTransObj, objPos);

		// 合成行列
		NMath::SMatrix44 matFinal;
		matFinal = matTrans * matRot * matTransObj;

		float3 posFinal;
		posFinal.SetZero();
		NMath::TransformVector(posFinal, matFinal);

		// 座標を設定
		SetCameraPosData(posFinal);
		SetCameraFocData(camFoc);
		SetCameraUpData(float3(0, 1, 0));
	}

	void COperatorPlayer::RotateCameraInput()
	{
		m_inputRightAnalogRaw.x = NInput::GetRightAnalogX();
		m_inputRightAnalogRaw.y = NInput::GetRightAnalogY();

		// 入力があるときのみカメラを移動
		float absSpeedX = abs(m_inputRightAnalogRaw.x);
		if (absSpeedX > FLT_EPSILON) {
			m_rotateSpeedInput +=
				m_inputRightAnalogRaw.x *
				(ROTATE_SPEED_DEG_INPUT_ACC * NSystem::DELTA_TIME);
			m_isOperatingWithStick = true;
			m_noOperateCnt = 0;
		}
		// 入力がない場合は減速処理
		else {
			m_rotateSpeedInput *= ROTATE_SPEED_DEG_INPUT_DEC_RATIO;
			m_noOperateCnt++;
		}
		if (m_noOperateCnt == INPUT_RELEASE_WAIT_CNT) {
			m_rotateSpeedPlayerMove = 0;
		}
		if (m_noOperateCnt > INPUT_RELEASE_WAIT_CNT) {
			m_isOperatingWithStick = false;
		}

		// 最大値でクランプ
		float rotateSpeedDebMaxPerFrame =
			absSpeedX *
			(ROTATE_SPEED_DEG_INPUT_MAX * NSystem::DELTA_TIME);

		if (m_rotateSpeedInput > rotateSpeedDebMaxPerFrame) {
			m_rotateSpeedInput = rotateSpeedDebMaxPerFrame;
		}
		else if (m_rotateSpeedInput < -rotateSpeedDebMaxPerFrame) {
			m_rotateSpeedInput = -rotateSpeedDebMaxPerFrame;
		}

		m_rotateSpeedSum += m_rotateSpeedInput;
	}

	void COperatorPlayer::GetCameraTargetPos(float3& _rDest)
	{
		float3 objPos;
		m_pFocusGameObject->GetTransform().GetPos(objPos);
		float3 objForward = m_pFocusGameObject->GetTransform().GetForward();
		float3 upDir(0, 1, 0);

		float3 camPosLocal;
		camPosLocal = 
			objForward * m_targetCameraLocalPos.z +
			upDir * m_targetCameraLocalPos.y;		
		_rDest = objPos + camPosLocal;
	}

	void COperatorPlayer::GetFocusPos(float3& _rDest){
		float3 objPos;
		m_pFocusGameObject->GetTransform().GetPos(objPos);
		_rDest = objPos + m_focusOffset;
	}

}