#include	"Debug_Debugger.h"

#include	"Camera\Operator\Camera_OperatorBase.h"
#include	"Camera\Operator\Camera_OperatorDebug.h"
#include	"Camera\Camera_OperatorManager.h"
#include	"Debug_Def.h"
#include	"Debug\Debug_Assert.h"
#include	"Input\Input_Common.h"
#include	"Input\Input_Def.h"
#include	"System\System_Def.h"

namespace {

	//!	@brief	再生の繊維状態
	enum EDebugPlayState {
		PLAY_STATE_PLAY,
		PLAY_STATE_PAUSE,
		PLAY_STATE_SLOW,

		PLAY_STATE_MAX,
	};

	const float			SLOW_SPEED_CHANGE_ACC = 0.1f;	// スロー速度の変化量
	const float			SLOW_SPEED_MIN = 0.1f;
	const float			SLOW_SPEED_MAX = 0.9f;

	int					g_counter;
	int					g_slowCounter;
	EDebugPlayState		g_playState;

	NCamera::COperatorBase*			g_prevMainCameraOperator = nullptr;
	NCamera::COperatorDebug			g_cameraOperatorDebug;
	bool							g_isDebugCamera = false;
	bool							g_isDebugCameraLock = false;

	//!	@brief	汎用更新処理
	void UpdateCommon();

	//!	@brief	再生状態を更新
	void UpdatePlayState();

	//!	@brief	デバッグカメラの切り替え
	void ChangeDebugCamera();

	//!	@brief	デバッグカメラの更新を切る(マネージャから外す)
	void LockDebugCamera();
}

namespace NDebug {

	void InitDebugger()
	{
		g_playState = PLAY_STATE_PLAY;
		g_counter = 0;
		g_slowCounter = 0;
	}

	void UpdateDebugger()
	{
		UpdatePlayState();
		ChangeDebugCamera();
		LockDebugCamera();
		UpdateCommon();
	}
}

namespace {

	void UpdateCommon()
	{
		g_counter++;
		g_slowCounter++;
	}

	void UpdatePlayState()
	{
		NDebug::IS_PAUSE_STEP = false;

		switch (g_playState) {
			case PLAY_STATE_PLAY:
				NDebug::IS_PAUSE_STEP = true;

				// ポーズモードの切り替え
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_PAUSE_SWITCH)) {
					NDebug::IS_PAUSE = true;
				}
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_SLOW_SWITCH)) {
					NDebug::IS_SLOW = true;
				}

				// 遷移
				if (NDebug::IS_SLOW) {
					g_playState = PLAY_STATE_SLOW;
				}
				else if (NDebug::IS_PAUSE) {
					g_playState = PLAY_STATE_PAUSE;
				}
				break;

			case PLAY_STATE_PAUSE:
				// ポーズモードの切り替え
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_PAUSE_SWITCH)) {
					NDebug::IS_PAUSE = false;
				}

				// スローモードの切り替え
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_SLOW_SWITCH)) {
					NDebug::IS_SLOW = true;
				}

				// ポーズ中のステップ処理
				NDebug::IS_PAUSE_STEP = false;
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_PAUSE_STEP)) {
					NDebug::IS_PAUSE_STEP = true;
				}

				// 遷移
				if (!NDebug::IS_PAUSE) {
					g_playState = PLAY_STATE_PLAY;
				}
				else if (NDebug::IS_SLOW) {
					g_playState = PLAY_STATE_SLOW;
				}

				break;

			case PLAY_STATE_SLOW:
				// スローモードの切り替え
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_SLOW_SWITCH)) {
					NDebug::IS_SLOW = false;
				}

				// スロー中のステップ処理
				if (g_slowCounter > (int)round(1.0f / NDebug::SLOW_RATIO)) {
					NDebug::IS_PAUSE_STEP = true;
					g_slowCounter = 0;
				}

				// スロー中の速度変更
				if (NInput::GetKeyDown(NInput::SYS_DEBUG_SLOW_SPEED_UP)) {
					NDebug::SLOW_RATIO += SLOW_SPEED_CHANGE_ACC;
					if (NDebug::SLOW_RATIO > SLOW_SPEED_MAX) {
						NDebug::SLOW_RATIO = SLOW_SPEED_MAX;
					}
				}
				else if (NInput::GetKeyDown(NInput::SYS_DEBUG_SLOW_SPEED_DOWN)) {
					NDebug::SLOW_RATIO -= SLOW_SPEED_CHANGE_ACC;
					if (NDebug::SLOW_RATIO < SLOW_SPEED_MIN) {
						NDebug::SLOW_RATIO = SLOW_SPEED_MIN;
					}
				}

				// 遷移
				if (!NDebug::IS_SLOW) {
					if (NDebug::IS_PAUSE) {
						g_playState = PLAY_STATE_PAUSE;
					}
					else {
						g_playState = PLAY_STATE_PLAY;
					}
				}
				break;

			default:
				Assert(false);
				break;
		}
	}

	void ChangeDebugCamera()
	{
		if(NInput::GetKeyDown(NInput::SYS_DEBUG_SWITCH_FREE_CAM))
		{
			// デバッグカメラだった場合は戻す
			if (g_isDebugCamera) {
				g_isDebugCamera = false;
				AssertNotNull(g_prevMainCameraOperator);
				NCamera::RegisterOperator(g_prevMainCameraOperator);
			}
			// デバッグカメラとメインカメラを入れ替える
			else {
				g_isDebugCamera = true;
				g_prevMainCameraOperator = NCamera::RegisterOperator(&g_cameraOperatorDebug);
			}
		}
	}

	void LockDebugCamera()
	{
		// デバッグカメラの時のみ行う
		if (!g_isDebugCamera) {
			return;
		}

		if (NInput::GetKeyDown(NInput::SYS_DEBUG_SWITCH_FREE_CAM_LOCK)) {
			// ロック状態だった場合はもう一度登録
			if (g_isDebugCameraLock) {
				g_prevMainCameraOperator = NCamera::RegisterOperator(&g_cameraOperatorDebug);
			}
			// ロックするときはマネージャから外す
			else {
				NCamera::UnregisterOperator(&g_cameraOperatorDebug);
			}
		}
	}
}
