#include	"Camera_OperatorDebug.h"

#include	"Input\Input_Game.h"
#include	"Input\Input_Common.h"	
#include	"System\System_Def.h"

namespace NCamera {

	COperatorDebug::COperatorDebug() :
		MOVE_SPEED_HIGH_MAX(20),
		MOVE_SPEED_LOW_MAX(2),
		ROTATE_SPEED_HIGH_MAX(720),
		ROTATE_SPEED_LOW_MAX(45),
		FOCUS_OFFSET(0, 0, 1),
		m_isSlow(false)
	{

	}

	COperatorDebug::~COperatorDebug()
	{

	}

	//----------------------------------------------------------

	void COperatorDebug::Operate()
	{
		if (NInput::GetKeyDown(DIK_X) || NDirectX::GetJoystickButtonHold(NInput::JOY_L3)) {
			m_isSlow = !m_isSlow;
		}

		OperateCamera();
	}

	//----------------------------------------------------------

	void COperatorDebug::OperateCamera()
	{
		// スティックのアナログ入力を取得
		float2 leftStick;
		float2 rightStick;
		leftStick.x = NInput::GetLeftAnalogX();
		leftStick.y = NInput::GetLeftAnalogY();
		rightStick.x = NInput::GetRightAnalogX();
		rightStick.y = NInput::GetRightAnalogY();

		// スティックの入力の加減を反映
		float moveMultiplier = leftStick.GetLength();
		float rotMultiplier = rightStick.GetLength();

		// フラグにより移動スピードを設定
		float moveSpeedMax = MOVE_SPEED_HIGH_MAX * NSystem::DELTA_TIME;
		if (m_isSlow) { moveSpeedMax = MOVE_SPEED_LOW_MAX* NSystem::DELTA_TIME; }
		float rotateSpeedMax = ROTATE_SPEED_HIGH_MAX* NSystem::DELTA_TIME;
		if (m_isSlow) { rotateSpeedMax = ROTATE_SPEED_LOW_MAX* NSystem::DELTA_TIME; }

		// 移動速度を求める
		float2 moveSpeed = leftStick * (moveSpeedMax * moveMultiplier);
		float2 rotSpeed = rightStick * (rotateSpeedMax * rotMultiplier);

		float3 forward = m_posObj.GetTransform().GetForward();
		float3 right = m_posObj.GetTransform().GetRight();
		float3 vel(0, 0, 0);
		vel = forward * moveSpeed.y + right * moveSpeed.x;

		// オブジェクトを移動させる
		m_posObj.GetTransform().Move(vel);

		// 回転させる
		m_posObj.GetTransform().Rotate(rotSpeed.y, rotSpeed.x, 0);
		forward = m_posObj.GetTransform().GetForward();

		// オブジェクトの位置を使ってカメラの位置をきめる
		float3 camPos;
		float3 camUp;
		float3 camFoc;
		m_posObj.GetTransform().GetPos(camPos);
		camUp = m_posObj.GetTransform().GetUp();
		camFoc = camPos + (1 * forward);

		SetCameraPosData(camPos);
		SetCameraUpData(camUp);
		SetCameraFocData(camFoc);
	}

}
