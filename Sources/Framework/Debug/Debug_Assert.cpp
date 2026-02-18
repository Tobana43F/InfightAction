#include	"Debug_Assert.h"

#include	<string>
#include	"Windows\Windows_Include.h"
#include	"Windows\Windows_Global.h"

namespace NDebug {
	void OutAssertMessage(
		const char*		_pMessage,
		const char*		_pFile,
		unsigned int	_line)
	{
		// アサート文字列を作成
		std::string	msg = "";
		msg += "My Assertion Failed!\n\n";
		msg += "Program : \n";
		msg += _pFile;
		msg += "\n\n";
		msg += "Line : ";
		msg += std::to_string(_line);
		msg += "\n\n";
		msg += "Expression : \n";
		msg += _pMessage;
		msg += "\n";

		// メッセージ表示
		int button = IDABORT;
		button = MessageBox(
			NWindows::gHWnd,
			msg.data(),
			"My Assertion Failed!",
			MB_ABORTRETRYIGNORE | MB_ICONERROR);
		
		if (button == IDABORT) {
			abort();	// 異常終了させる
		}
		else {
			// 何もしない
		}
	}

}