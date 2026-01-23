#include	"System_Common.h"

#include	<string>
#include	"System_Def.h"
#include	"Windows\Windows_Include.h"
#include	"Windows\Windows_Global.h"

namespace NSystem {

	void SetWindowTitle(const char* _str)
	{
		std::string str = NSystem::WINDOW_TITLE;
		str += " ";
		str += _str;
		SetWindowText(NWindows::gHWnd, str.data());
	}

}