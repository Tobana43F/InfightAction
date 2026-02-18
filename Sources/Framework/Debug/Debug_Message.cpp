#include	"Debug_Message.h"
#include	<stdio.h>
#include	<cstdarg>
#include	<string>
#include	"Windows\Windows_Include.h"
#include	"Debug\Debug_Def.h"

namespace {
	constexpr int BUFFER_SIZE = 512;
}

namespace NDebug {

	void PrintDebug(const char* _pFmt, ...)
	{
		if (!IS_DEBUG_MODE)
			return;

		va_list	arg;
		char	buf[BUFFER_SIZE];

		va_start(arg, _pFmt);
		vsprintf_s<sizeof(buf)>(buf, _pFmt, arg);
		va_end(arg);

		printf("%s", buf);
	}

	void Print(bool _isEnable, cchar* _pFmt, ...)
	{
		if (!_isEnable)
			return;

		va_list	arg;
		char	buf[BUFFER_SIZE];

		va_start(arg, _pFmt);
		vsprintf_s<sizeof(buf)>(buf, _pFmt, arg);
		va_end(arg);

		printf("%s", buf);
	}

	void PrintSwitch(bool _isEnable, bool _state, cchar* _msg1, cchar* _msg2)
	{
		if (_state) {
			Print(_isEnable, "%s", _msg1);
		}
		else {
			Print(_isEnable, "%s", _msg2);
		}
	}


	void OutLogMessage(cchar* _pMessage)
	{
		OutputDebugString(_pMessage);
	}

	void OutLogMessage(bool _isEnable, cchar* _pMessage)
	{
		if (!_isEnable)
			return;

		OutLogMessage(_pMessage);
	}
}