#pragma once

#include	"Camera_OperatorBase.h"

#include	"Math\Math_Type.h"
#include	"Object\Object_GameObject.h"

namespace NCamera {

	//!	@brief		プレイヤーが行うカメラ操作を行うクラス
	//!	@todo		仮実装
	//!				- 滑らかな追従(ばねのような追従)
	//!				- 注視点を滑らかにずらす
	//!	@warning	必ずプレイヤーが存在すること(プレイヤーが内包すればOK)
	//!	@bug		プレイヤーがカメラを参照した動きをして、
	//!				その後にカメラ操作が入るので、挙動がおかしくなる時がある。
	class COperatorPlayer : public COperatorBase {
	public:
		//!	@brief		このクラスを使用できる状態にする
		//!	@param[in]	_pFocusGameObject	注視するオブジェクト(プレイヤー)のポインタ
		//!	@param[in]	_pPlayerMoveDir		プレイヤーの移動方向の参照
		void Setup(
			const NObject::CGameObject*	_pFocusGameObject,
			const float*				_pMoveSpeed,
			const float					_playerMoveSpeedMax);

		//!	@brief	値を初期化する
		void Init();

	protected:
		virtual void Operate() override;

	private:

		// 定数
		float	ROTATE_SPEED_DEG_PLAYER_MOVE_MAX;	// プレイヤーの移動によるカメラの回転の速度の最大
													// ※ 1秒間の回転角度
		float	ROTATE_SPEED_DEG_PLAYER_MOVE_ACC;	// プレイヤーの移動によるカメラの回転の加速度
													// ※ 1描画の回転角度
		float	ROTATE_SPEED_DEG_PLAYER_MOVE_DEC_RATIO;	// プレイヤーの移動によるカメラの回転の減速率

		float	ROTATE_SPEED_DEG_INPUT_MAX;
		float	ROTATE_SPEED_DEG_INPUT_ACC;
		float	ROTATE_SPEED_DEG_INPUT_DEC_RATIO;
		int		INPUT_RELEASE_WAIT_CNT;

		// 制御用
		bool	m_isSetupOK;

		// カメラ制御に使用する変数
		const NObject::CGameObject*		m_pFocusGameObject;	// 注目、追尾するゲームオブジェクト
		const NMath::SVector2*			m_pPlayerMoveDirXZ;	// プレイヤーのカメラに対する移動方向の参照
		const float*					m_pMoveSpeed;		// 移動量の参照
		float							m_playerMoveSpeedMax;
		float2							m_inputRightAnalogRaw;

		float3	m_focusOffset;				// 注目するオブジェクトの位置からのオフセット
		float3	m_targetCameraLocalPos;		// 注視するオブジェクトの位置からのカメラの相対座標の目的地
		float	m_rotateSpeedPlayerMove;	// プレイヤーの移動によるカメラの回転スピード
		float	m_rotateSpeedInput;			// 入力による回転速度
		float	m_rotateSpeedSum;	
		bool	m_isOperatingWithStick;
		int		m_noOperateCnt;

		//!	@brief	定数値を初期化
		void InitVariable()
		{
			ROTATE_SPEED_DEG_PLAYER_MOVE_MAX = 90;
			ROTATE_SPEED_DEG_PLAYER_MOVE_ACC = 10;
			ROTATE_SPEED_DEG_PLAYER_MOVE_DEC_RATIO = 0.925f;

			ROTATE_SPEED_DEG_INPUT_MAX = 180;
			ROTATE_SPEED_DEG_INPUT_ACC = 18;
			ROTATE_SPEED_DEG_INPUT_DEC_RATIO = 0.8f;
			INPUT_RELEASE_WAIT_CNT = 30;

			m_rotateSpeedPlayerMove = 0;
			m_rotateSpeedSum = 0;
			m_isOperatingWithStick = false;
			m_noOperateCnt = 0;
		}

		//!	@brief	プレイヤーの移動によるカメラの移動(Y軸回転)
		void RotateCameraPlayerMove();

		//!	@brief	入力によるカメラの移動(Y軸回転)
		void RotateCameraInput();

		//!	@brief	カメラの目標位置を取得する
		void GetCameraTargetPos(float3& _rDest);

		//!	@brief	オフセットを考慮した注視する位置を取得
		void GetFocusPos(float3& _rDest);

	public:
		COperatorPlayer();
		virtual ~COperatorPlayer();
	};

}