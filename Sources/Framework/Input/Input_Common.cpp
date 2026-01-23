#include	"Input\Input_Common.h"

#include	"DirectX\DirectX_Input.h"
#include	"Input\Input_Game.h"
#include	"Input\Input_GameBuffer.h"
#include	"System\System_Error.h"

namespace NInput {

	bool Setup(HINSTANCE _hInstance, HWND _hWnd)
	{
		HRESULT	hr = E_FAIL;
		hr = NDirectX::InitInput(_hInstance, _hWnd);
		if (FAILED(hr)) {
			OutErrorMessageEx("ì¸óÕÇÃèâä˙âªèàóùÇ…é∏îsÇµÇ‹ÇµÇΩÅB");
			return false;
		}

		return true;
	}

	void TearDown()
	{
		NDirectX::UninitInput();
	}

	void Update()
	{
		NDirectX::UpdateInput();
		NInput::UpdateVirtualAnalog();
		NInput::UpdateGameBuffer();
	}



	bool GetKeyPress(int _key)
	{
		return NDirectX::GetKeyboardPress(_key);
	}

	bool GetKeyDown(int _key)
	{
		return NDirectX::GetKeyboardTrigger(_key);
	}

	bool GetKeyUp(int _key)
	{
		return NDirectX::GetKeyboardRelease(_key);
	}

	bool GetKeyRepeat(int _key)
	{
		return NDirectX::GetKeyboardRepeat(_key);
	}

	bool GetJoystickUpHold()
	{
		return NDirectX::GetJoystickUpHold();
	}

	bool GetJoystickDownHold()
	{
		return NDirectX::GetJoystickDownHold();
	}

	bool GetJoystickRightHold()
	{
		return NDirectX::GetJoystickRightHold();
	}

	bool GetJoystickLeftHold()
	{
		return NDirectX::GetJoystickLeftHold();
	}

	bool GetJoystickUpTrigger()
	{
		return NDirectX::GetJoystickUpTrigger();

	}
	bool GetJoystickDownTrigger()
	{
		return NDirectX::GetJoystickDownTrigger();
	}

	bool GetJoystickRightTrigger()
	{
		return NDirectX::GetJoystickRightTrigger();
	}

	bool GetJoystickLeftTrigger()
	{
		return NDirectX::GetJoystickLeftTrigger();
	}

	bool GetJoystickPress(int _buttonNo)
	{
		return NDirectX::GetJoystickButtonHold(_buttonNo);
	}

	bool GetJoystickDown(int _buttonNo)
	{
		return NDirectX::GetJoystickButtonTrigger(_buttonNo);
	}

	bool GetJoystickUp(int _buttonNo)
	{
		return NDirectX::GetJoystickButtonRelease(_buttonNo);
	}


}