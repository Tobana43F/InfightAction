#include	"DirectX_Input.h"

namespace NDirectX {

	enum EDirection {
		DIR_UP,
		DIR_RIGHT,
		DIR_DOWN,
		DIR_LEFT,
		DIR_MAX
	};

	struct DI_ENUM_CONTEXT {
		DIJOYCONFIG*	pPreferredJoyCfg;
		bool			bPreferredJoyCfgValid;
	};

	static const int NUM_KEY_MAX = 256;
	static const int REPEAT_WAIT = 20;
	static const int REPEAT_SPEED = 5;

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

	HRESULT InitKeyboard(HINSTANCE hInst, HWND gHWnd);
	void UninitKeyboard(void);
	HRESULT UpdateKeyboard(void);
	HRESULT InitMouse(HINSTANCE hInst, HWND gHWnd);
	void UninitMouse(void);
	HRESULT UpdateMouse(void);

	static LPDIRECTINPUT8		g_pDInput = NULL;					// IDirectInput8インターフェースへのポインタ

	static LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = NULL;			// IDirectInputDevice8インターフェースへのポインタ(キーボード)
	static LPDIRECTINPUTDEVICE8	g_pDIDevMouse = NULL;				// IDirectInputDevice8インターフェースへのポインタ(マウス)
	static BYTE					g_keyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
	static BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	static BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	static BYTE					g_keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	static int					g_keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ
	static DIMOUSESTATE2		g_mouseState;						// マウスの状態を受け取るワーク
	static DIMOUSESTATE2		g_mouseStateTrigger;				// マウスの状態を受け取るワーク
	static POINT				g_MousePoint;						// マウスの現在座標
	static HWND					g_hwnd;

	// ジョイスティック関係
	static LPDIRECTINPUTDEVICE8	g_pDIDevJoystick = NULL;

	static const int NUM_JOYSTICK_BUF = 2;
	DIJOYSTATE2 g_JoystickState[NUM_JOYSTICK_BUF];		// DInput joystick state
	DIJOYSTATE2 g_JoystickStateSub[NUM_JOYSTICK_BUF];	// サブバッファ(スティックをデジタル入力の値に変換して使っている)

	static const int ANALOG_MAX = 1000;					// アナログ入力の最大値
	static const int DEAD_THLESHOLD = 100;				// アナログ入力の閾値
	static const int DEAD_THLESHOLD_LOW = 10;			// アナログ入力の他の軸の入力があるときの閾値
	static const int DEAD_DEGITAL_THRESHOLD = 500;		// アナログをデジタル入力のように使うときの閾値

	static bool g_IsJoystickEnable = true;

	//-----------------------------------------------------------------------------
	// ジョイスティックの更新
	//-----------------------------------------------------------------------------
	//!	@brief	値の調整などを行う関数
	//!	@note	UpdateJoystick()関数内でのみ呼び出す
	void UpdateJoystickAdjust()
	{

		// 判定しない領域を作る(アナログスティックが中央にある時だけ)
		if (abs(g_JoystickStateSub[0].lY) < DEAD_THLESHOLD) {
			if (abs(g_JoystickState[0].lX) < DEAD_THLESHOLD)
				g_JoystickState[0].lX = 0;
		}
		else {
			if (abs(g_JoystickState[0].lX) < DEAD_THLESHOLD_LOW)
				g_JoystickState[0].lX = 0;
		}

		if (abs(g_JoystickStateSub[0].lX) < DEAD_THLESHOLD) {
			if (abs(g_JoystickState[0].lY) < DEAD_THLESHOLD)
				g_JoystickState[0].lY = 0;
		}
		else {
			if (abs(g_JoystickState[0].lY) < DEAD_THLESHOLD_LOW)
				g_JoystickState[0].lY = 0;
		}

		if (abs(g_JoystickStateSub[0].lRy) < DEAD_THLESHOLD) {
			if (abs(g_JoystickState[0].lRx) < DEAD_THLESHOLD)
				g_JoystickState[0].lRx = 0;
		}
		else {
			if (abs(g_JoystickState[0].lRx) < DEAD_THLESHOLD_LOW)
				g_JoystickState[0].lRx = 0;
		}

		if (abs(g_JoystickStateSub[0].lRx) < DEAD_THLESHOLD) {
			if (abs(g_JoystickState[0].lRy) < DEAD_THLESHOLD)
				g_JoystickState[0].lRy = 0;
		}
		else {
			if (abs(g_JoystickState[0].lRy) < DEAD_THLESHOLD_LOW)
				g_JoystickState[0].lRy = 0;
		}

		// 判定しない領域を作る
		if (abs(g_JoystickStateSub[0].lX) < DEAD_DEGITAL_THRESHOLD)
			g_JoystickStateSub[0].lX = 0;

		if (abs(g_JoystickStateSub[0].lY) < DEAD_DEGITAL_THRESHOLD)
			g_JoystickStateSub[0].lY = 0;

		if (abs(g_JoystickStateSub[0].lRx) < DEAD_DEGITAL_THRESHOLD)
			g_JoystickStateSub[0].lRx = 0;

		if (abs(g_JoystickStateSub[0].lRy) < DEAD_DEGITAL_THRESHOLD)
			g_JoystickStateSub[0].lRy = 0;

		// Y軸の座標系を変更
		g_JoystickState[0].lY *= -1;
		g_JoystickState[0].lRy *= -1;
		g_JoystickStateSub[0].lY *= -1;
		g_JoystickStateSub[0].lRy *= -1;


	}

	//!	@brief	更新
	HRESULT UpdateJoystick()
	{
		HRESULT hr;

		// ジョイスティックがない場合は無効
		if (!g_IsJoystickEnable)
			return S_OK;


		// Update old buffer
		g_JoystickState[1] = g_JoystickState[0];
		g_JoystickStateSub[1] = g_JoystickStateSub[0];

		// Poll the device to read the current state
		hr = g_pDIDevJoystick->Poll();
		if (FAILED(hr)) {
			// DInput is telling us that the input stream has been
			// interrupted. We aren't tracking any state between polls, so
			// we don't have any special reset that needs to be done. We
			// just re-acquire and try again.
			hr = g_pDIDevJoystick->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = g_pDIDevJoystick->Acquire();

			// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
			// may occur when the app is minimized or in the process of 
			// switching, so just try again later 
			return S_OK;
		}

		// Get the input's device state
		if (FAILED(g_pDIDevJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &g_JoystickState[0])))
			return hr; // The device should have been acquired during the Poll()
		if (FAILED(g_pDIDevJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &g_JoystickStateSub[0])))
			return hr; // The device should have been acquired during the Poll()

		UpdateJoystickAdjust();

		return hr;
	}

	//-----------------------------------------------------------------------------
	// Name: EnumJoysticksCallback()
	// Desc: Called once for each enumerated joystick. If we find one, create a
	//       device interface on it so we can play with it.
	//-----------------------------------------------------------------------------
	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
		VOID* pContext)
	{
		DI_ENUM_CONTEXT* pEnumContext = (DI_ENUM_CONTEXT*)pContext;
		HRESULT hr;

		// Skip anything other than the perferred joystick device as defined by the control panel.  
		// Instead you could store all the enumerated joysticks and let the user pick.
		if (pEnumContext->bPreferredJoyCfgValid &&
			!IsEqualGUID(pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance))
			return DIENUM_CONTINUE;

		// Obtain an interface to the enumerated joystick.
		hr = g_pDInput->CreateDevice(pdidInstance->guidInstance, &g_pDIDevJoystick, NULL);

		// If it failed, then we can't use this joystick. (Maybe the user unplugged
		// it while we were in the middle of enumerating it.)
		if (FAILED(hr))
			return DIENUM_CONTINUE;

		// Stop enumeration. Note: we're just taking the first joystick we get. You
		// could store all the enumerated joysticks and let the user pick.
		return DIENUM_STOP;
	}

	BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
		VOID* pContext)
	{
		HWND hDlg = (HWND)pContext;

		static int nSliderCount = 0;  // Number of returned slider controls
		static int nPOVCount = 0;     // Number of returned POV controls

									  // For axes that are returned, set the DIPROP_RANGE property for the
									  // enumerated axis in order to scale min/max values.
		if (pdidoi->dwType & DIDFT_AXIS) {
			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
			diprg.lMin = -ANALOG_MAX;
			diprg.lMax = +ANALOG_MAX;

			// Set the range for the axis
			if (FAILED(g_pDIDevJoystick->SetProperty(DIPROP_RANGE, &diprg.diph)))
				return DIENUM_STOP;
		}
		return DIENUM_CONTINUE;
	}

	//=============================================================================
	// 入力処理の初期化
	//=============================================================================
	HRESULT InitInput(HINSTANCE hInst, HWND gHWnd)
	{
		HRESULT hr = 0;// 適当に初期化

		if (!g_pDInput) {
			// DirectInputオブジェクトの作成
			hr = DirectInput8Create(
				hInst,
				DIRECTINPUT_VERSION,
				IID_IDirectInput8,
				(void**)&g_pDInput,
				NULL);
		}

		// キーボードの初期化
		InitKeyboard(hInst, gHWnd);

		// マウスの初期化
		InitMouse(hInst, gHWnd);

		//----------------------------------------------------------
		// ジョイスティックの初期化
		DIJOYCONFIG PreferredJoyCfg = { 0 };
		DI_ENUM_CONTEXT enumContext;
		enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
		enumContext.bPreferredJoyCfgValid = false;

		IDirectInputJoyConfig8* pJoyConfig = NULL;
		if (FAILED(hr = g_pDInput->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
			return hr;

		PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);
		if (SUCCEEDED(pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE))) // This function is expected to fail if no joystick is attached
			enumContext.bPreferredJoyCfgValid = true;
		SAFE_RELEASE(pJoyConfig);

		// Look for a simple joystick we can use for this sample program.
		if (FAILED(hr = g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
			EnumJoysticksCallback,
			&enumContext, DIEDFL_ATTACHEDONLY)))
			return hr;

		// Make sure we got a joystick
		if (NULL == g_pDIDevJoystick) {
			g_IsJoystickEnable = false;
			return S_OK;
		}

		// Set the data format to "simple joystick" - a predefined data format 
		//
		// A data format specifies which controls on a device we are interested in,
		// and how they should be reported. This tells DInput that we will be
		// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		if (FAILED(hr = g_pDIDevJoystick->SetDataFormat(&c_dfDIJoystick2)))
			return hr;


		// Enumerate the joystick objects. The callback function enabled user
		// interface elements for objects that are found, and sets the min/max
		// values property for discovered axes.
		if (FAILED(hr = g_pDIDevJoystick->EnumObjects(EnumObjectsCallback,
			(VOID*)gHWnd, DIDFT_ALL)))

			g_hwnd = gHWnd;

		return hr;
	}

	//=============================================================================
	// 入力処理の終了処理
	//=============================================================================
	void UninitInput(void)
	{
		// キーボードの終了処理
		UninitKeyboard();

		// マウスの終了処理
		UninitMouse();

		// ジョイスティックの終了処理
		if (g_pDIDevJoystick)
			g_pDIDevJoystick->Unacquire();

		SAFE_RELEASE(g_pDIDevJoystick);

		if (g_pDInput) {
			g_pDInput->Release();
			g_pDInput = NULL;
		}
	}

	//=============================================================================
	// 入力処理の更新処理
	//=============================================================================
	void UpdateInput(void)
	{
		// キーボードの更新
		UpdateKeyboard();

		// マウスの更新
		UpdateMouse();

		// ジョイスティックの更新
		UpdateJoystick();
	}

	//=============================================================================
	// キーボードの初期化
	//=============================================================================
	HRESULT InitKeyboard(HINSTANCE hInst, HWND gHWnd)
	{
		HRESULT hr;

		// デバイスオブジェクトを作成
		hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
		if (FAILED(hr) || g_pDIDevKeyboard == NULL) {
			MessageBox(gHWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
			return hr;
		}

		// データフォーマットを設定
		hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr)) {
			MessageBox(gHWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
			return hr;
		}

		// 協調モードを設定（フォアグラウンド＆非排他モード）
		hr = g_pDIDevKeyboard->SetCooperativeLevel(gHWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
		if (FAILED(hr)) {
			MessageBox(gHWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
			return hr;
		}

		// キーボードへのアクセス権を獲得(入力制御開始)
		g_pDIDevKeyboard->Acquire();

		return S_OK;
	}

	//=============================================================================
	// キーボードの終了処理
	//=============================================================================
	void UninitKeyboard(void)
	{
		if (g_pDIDevKeyboard) {
			g_pDIDevKeyboard->Unacquire();

			g_pDIDevKeyboard->Release();
			g_pDIDevKeyboard = NULL;
		}
	}

	//=============================================================================
	// キーボードの更新処理
	//=============================================================================
	HRESULT UpdateKeyboard(void)
	{
		HRESULT hr;
		BYTE keyStateOld[256];

		// 前回のデータを保存
		memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

		// デバイスからデータを取得
		hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
		if (SUCCEEDED(hr)) {
			for (int cnt = 0; cnt < NUM_KEY_MAX; cnt++) {
				// トリガーとリリースを取得
				g_keyStateTrigger[cnt] = (g_keyState[cnt] ^ keyStateOld[cnt]) & g_keyState[cnt];
				g_keyStateRelease[cnt] = (g_keyState[cnt] ^ keyStateOld[cnt]) & keyStateOld[cnt];

				// キーが押されているならリピートの判定処理
				if (g_keyState[cnt]) {
					if (g_keyStateRepeatCnt[cnt] < 20) {
						g_keyStateRepeatCnt[cnt]++;
						// 「初回入力」もしくは「ボタンを押してから20フレーム経過」
						// した場合、リピート用配列のデータを有効にする
						// 上記条件以外はリピート用配列のデータを無効にする
						if (g_keyStateRepeatCnt[cnt] == 1 || g_keyStateRepeatCnt[cnt] >= 20) {
							g_keyStateRepeat[cnt] = 0x80;
						}
						else {
							g_keyStateRepeat[cnt] = 0;
						}
					}
				}
				else {
					g_keyStateRepeatCnt[cnt] = 0;
					g_keyStateRepeat[cnt] = 0;
				}
			}
		}
		else {
			// キーボードへのアクセス権を取得
			hr = g_pDIDevKeyboard->Acquire();
		}

		return hr;
	}

	//=============================================================================
	// キーボードのプレス状態を取得
	//=============================================================================
	bool GetKeyboardPress(int key)
	{
		return ((g_keyState[key] & 0x80) != 0);
	}

	//=============================================================================
	// キーボードのトリガー状態を取得
	//=============================================================================
	bool GetKeyboardTrigger(int key)
	{
		return ((g_keyStateTrigger[key] & 0x80) != 0);
	}

	//=============================================================================
	// キーボードのリピート状態を取得
	//=============================================================================
	bool GetKeyboardRepeat(int key)
	{
		return ((g_keyStateRepeat[key] & 0x80) != 0);
	}

	//=============================================================================
	// キーボードのリリ－ス状態を取得
	//=============================================================================
	bool GetKeyboardRelease(int key)
	{
		return ((g_keyStateRelease[key] & 0x80) != 0);
	}

	//=============================================================================
	// マウスの初期化
	//=============================================================================
	HRESULT InitMouse(HINSTANCE hInst, HWND gHWnd)
	{
		HRESULT hr;

		// デバイスオブジェクトを作成
		hr = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIDevMouse, NULL);
		if (FAILED(hr) || g_pDIDevMouse == NULL) {
			MessageBox(gHWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
			return hr;
		}

		// データフォーマットを設定
		hr = g_pDIDevMouse->SetDataFormat(&c_dfDIMouse2);
		if (FAILED(hr)) {
			MessageBox(gHWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
			return hr;
		}

		// 協調モードを設定（フォアグラウンド＆非排他モード）
		hr = g_pDIDevMouse->SetCooperativeLevel(gHWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
		if (FAILED(hr)) {
			MessageBox(gHWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
			return hr;
		}

		// デバイスの設定
		{
			DIPROPDWORD dipdw;

			dipdw.diph.dwSize = sizeof(dipdw);
			dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
			dipdw.diph.dwObj = 0;
			dipdw.diph.dwHow = DIPH_DEVICE;
			dipdw.dwData = DIPROPAXISMODE_REL;

			hr = g_pDIDevMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
		}

		// キーボードへのアクセス権を獲得(入力制御開始)
		g_pDIDevMouse->Acquire();

		return S_OK;
	}

	//=============================================================================
	// マウスの終了処理
	//=============================================================================
	void UninitMouse(void)
	{
		if (g_pDIDevMouse) {
			g_pDIDevMouse->Unacquire();

			g_pDIDevMouse->Release();
			g_pDIDevMouse = NULL;
		}
	}

	//==========================================================
	// マウスの更新処理
	//==========================================================
	HRESULT UpdateMouse(void)
	{
		HRESULT hr;
		DIMOUSESTATE2 mouseStateOld;

		// 前回のデータを保存
		mouseStateOld = g_mouseState;

		GetCursorPos(&g_MousePoint);
		ScreenToClient(g_hwnd, &g_MousePoint);


		// デバイスからデータを取得
		hr = g_pDIDevMouse->GetDeviceState(sizeof(g_mouseState), &g_mouseState);
		if (SUCCEEDED(hr)) {
			// トリガーの取得
			//g_mouseStateTrigger.lX = 
			//g_mouseStateTrigger.lY = 
			//g_mouseStateTrigger.lZ = 
			for (int cnt = 0; cnt < 8; cnt++) {
				g_mouseStateTrigger.rgbButtons[cnt] = ((mouseStateOld.rgbButtons[cnt] ^ g_mouseState.rgbButtons[cnt]) & g_mouseState.rgbButtons[cnt]);
			}
		}
		else {
			g_pDIDevMouse->Acquire();
		}

		return S_OK;
	}

	//==========================================================
	// マウスのボタン
	//==========================================================
	bool GetMouseLeftPress(void)
	{
		return ((g_mouseState.rgbButtons[0] & 0x80) != 0);
	}

	bool GetMouseLeftTrigger(void)
	{
		return ((g_mouseStateTrigger.rgbButtons[0] & 0x80) != 0);
	}

	bool GetMouseRightPress(void)
	{
		return ((g_mouseState.rgbButtons[1] & 0x8) != 00);
	}

	bool GetMouseRightTrigger(void)
	{
		return ((g_mouseStateTrigger.rgbButtons[1] & 0x80) != 0);
	}

	bool GetMouseCenterPress(void)
	{
		return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
	}

	bool GetMouseCenterTrigger(void)
	{
		return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
	}

	//==========================================================
	// マウスの移動量
	//==========================================================
	long GetMouseAxisX(void)
	{
		return g_mouseState.lX;
	}

	long GetMouseAxisY(void)
	{
		return g_mouseState.lY;
	}

	//==========================================================
	// ホイールの回転量
	//==========================================================
	long GetMouseAxisZ(void)
	{
		return g_mouseState.lZ;
	}

	//==========================================================
	// マウスの座標
	//==========================================================
	long GetMouseX(void)
	{
		return g_MousePoint.x;
	}

	long GetMouseY(void)
	{
		return g_MousePoint.y;
	}

	//==========================================================
	// ジョイスティックの軸
	//==========================================================
	long	GetLeftJoystickX()
	{
		return g_JoystickState[0].lX;
	}

	long	GetLeftJoystickY()
	{
		return g_JoystickState[0].lY;
	}

	long	GetRightJoystickX()
	{
		return g_JoystickState[0].lRx;
	}

	long	GetRightJoystickY()
	{
		return g_JoystickState[0].lRy;
	}

	float	GetLeftJoystickXNormal()
	{
		return static_cast<float>(g_JoystickState[0].lX) / static_cast<float>(ANALOG_MAX);
	}

	float	GetLeftJoystickYNormal()
	{
		return static_cast<float>(g_JoystickState[0].lY) / static_cast<float>(ANALOG_MAX);
	}

	float	GetRightJoystickXNormal()
	{
		return static_cast<float>(g_JoystickState[0].lRx) / static_cast<float>(ANALOG_MAX);
	}

	float	GetRightJoystickYNormal()
	{
		return static_cast<float>(g_JoystickState[0].lRy) / static_cast<float>(ANALOG_MAX);
	}

	//==========================================================
	// ジョイスティックのデジタルとしての上下左右入力
	//==========================================================
	bool GetJoystickRightHold()
	{
		if (g_JoystickStateSub[0].lX > 0)
			return true;
		return false;
	}

	bool GetJoystickLeftHold()
	{
		if (g_JoystickStateSub[0].lX < 0)
			return true;
		return false;
	}

	bool GetJoystickUpHold()
	{
		if (g_JoystickStateSub[0].lY < 0)
			return true;
		return false;
	}

	bool GetJoystickDownHold()
	{
		if (g_JoystickStateSub[0].lY > 0)
			return true;
		return false;
	}

	bool GetJoystickUpTrigger()
	{
		bool prev = g_JoystickStateSub[1].lY < 0;
		bool cur = g_JoystickStateSub[0].lY < 0;

		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetJoystickDownTrigger()
	{
		bool prev = g_JoystickStateSub[1].lY > 0;
		bool cur = g_JoystickStateSub[0].lY > 0;

		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetJoystickRightTrigger()
	{
		bool prev = g_JoystickStateSub[1].lX > 0;
		bool cur = g_JoystickStateSub[0].lX > 0;

		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetJoystickLeftTrigger()
	{
		bool prev = g_JoystickStateSub[1].lX < 0;
		bool cur = g_JoystickStateSub[0].lX < 0;

		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetJoystickUpRelease()
	{
		bool prev = g_JoystickStateSub[1].lY < 0;
		bool cur = g_JoystickStateSub[0].lY < 0;

		if (cur == false && prev == true)
			return true;
		return false;
	}

	bool GetJoystickDownRelease()
	{
		bool prev = g_JoystickStateSub[1].lY > 0;
		bool cur = g_JoystickStateSub[0].lY > 0;

		if (cur == false && prev == true)
			return true;
		return false;
	}

	bool GetJoystickRightRelease()
	{
		bool prev = g_JoystickStateSub[1].lX > 0;
		bool cur = g_JoystickStateSub[0].lX > 0;

		if (cur == false && prev == true)
			return true;
		return false;
	}

	bool GetJoystickLeftRelease()
	{
		bool prev = g_JoystickStateSub[1].lX < 0;
		bool cur = g_JoystickStateSub[0].lX < 0;

		if (cur == false && prev == true)
			return true;
		return false;
	}

	//==========================================================
	//　POV入力
	//==========================================================
	bool GetPOVUpHold()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool sts = false;
		sts |= g_JoystickState[0].rgdwPOV[0] == 0;
		sts |= g_JoystickState[0].rgdwPOV[0] == 4500;
		sts |= g_JoystickState[0].rgdwPOV[0] == 31500;
		return sts;
	}

	bool GetPOVRightHold()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool sts = false;
		sts |= g_JoystickState[0].rgdwPOV[0] == 9000;
		sts |= g_JoystickState[0].rgdwPOV[0] == 13500;
		sts |= g_JoystickState[0].rgdwPOV[0] == 4500;
		return sts;
	}

	bool GetPOVDownHold()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool sts = false;
		sts |= g_JoystickState[0].rgdwPOV[0] == 18000;
		sts |= g_JoystickState[0].rgdwPOV[0] == 13500;
		sts |= g_JoystickState[0].rgdwPOV[0] == 22500;
		return sts;
	}

	bool GetPOVLeftHold()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool sts = false;
		sts |= g_JoystickState[0].rgdwPOV[0] == 27000;
		sts |= g_JoystickState[0].rgdwPOV[0] == 22500;
		sts |= g_JoystickState[0].rgdwPOV[0] == 31500;
		return sts;
	}
	
	bool GetPOVUpTrigger()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 0;
		cur |= g_JoystickState[0].rgdwPOV[0] == 4500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 31500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 0;
		prev |= g_JoystickState[1].rgdwPOV[0] == 4500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 31500;

		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetPOVRightTrigger()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 9000;
		cur |= g_JoystickState[0].rgdwPOV[0] == 13500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 4500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 9000;
		prev |= g_JoystickState[1].rgdwPOV[0] == 13500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 4500;

		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetPOVDownTrigger()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 18000;
		cur |= g_JoystickState[0].rgdwPOV[0] == 13500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 22500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 18000;
		prev |= g_JoystickState[1].rgdwPOV[0] == 13500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 22500;
		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetPOVLeftTrigger()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 27000;
		cur |= g_JoystickState[0].rgdwPOV[0] == 22500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 31500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 27000;
		prev |= g_JoystickState[1].rgdwPOV[0] == 22500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 31500;
		if (cur == true && prev == false)
			return true;
		return false;
	}

	bool GetPOVUpRelease()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 0;
		cur |= g_JoystickState[0].rgdwPOV[0] == 4500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 31500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 0;
		prev |= g_JoystickState[1].rgdwPOV[0] == 4500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 31500;

		if (cur == false && prev == true)
			return true;
		return false;
	}

	bool GetPOVRightRelease()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 9000;
		cur |= g_JoystickState[0].rgdwPOV[0] == 13500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 4500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 9000;
		prev |= g_JoystickState[1].rgdwPOV[0] == 13500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 4500;

		if (cur == false && prev == true)
			return true;
		return false;
	}

	bool GetPOVDownRelease()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 18000;
		cur |= g_JoystickState[0].rgdwPOV[0] == 13500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 22500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 18000;
		prev |= g_JoystickState[1].rgdwPOV[0] == 13500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 22500;
		if (cur == false && prev == true)
			return true;
		return false;
	}

	bool GetPOVLeftRelease()
	{
		// ジョイスティックが無効な場合はfalse
		if (!g_IsJoystickEnable) { return false; }

		bool cur = false;
		cur |= g_JoystickState[0].rgdwPOV[0] == 27000;
		cur |= g_JoystickState[0].rgdwPOV[0] == 22500;
		cur |= g_JoystickState[0].rgdwPOV[0] == 31500;
		bool prev = false;
		prev |= g_JoystickState[1].rgdwPOV[0] == 27000;
		prev |= g_JoystickState[1].rgdwPOV[0] == 22500;
		prev |= g_JoystickState[1].rgdwPOV[0] == 31500;
		if (cur == false && prev == true)
			return true;
		return false;
	}

	//==========================================================
	// ジョイスティックのボタン
	//==========================================================
	bool GetJoystickButtonHold(int _no)
	{
		// 無効な範囲
		if (_no < 0 || _no >= 128)
			return false;

		return (g_JoystickState[0].rgbButtons[_no] & 0x80) != 0;
	}

	bool GetJoystickButtonTrigger(int _no)
	{
		// 無効な範囲
		if (_no < 0 || _no >= 128)
			return false;

		bool prev = (g_JoystickState[1].rgbButtons[_no] & 0x80) != 0;
		bool cur = (g_JoystickState[0].rgbButtons[_no] & 0x80) != 0;

		if (cur == true && prev == false)
			return true;
		return false;

	}

	bool GetJoystickButtonRelease(int _no)
	{
		// 無効な範囲
		if (_no < 0 || _no >= 128)
			return false;

		bool prev = (g_JoystickState[1].rgbButtons[_no] & 0x80) != 0;
		bool cur = (g_JoystickState[0].rgbButtons[_no] & 0x80) != 0;

		if (cur == false && prev == true)
			return true;
		return false;
	}
}
