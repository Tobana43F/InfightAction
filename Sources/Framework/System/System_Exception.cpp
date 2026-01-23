#include	"System_Exception.h"
#include	"Windows/Windows_Include.h"
#include	"Windows/Windows_Global.h"

namespace {
	const char* pTitleName = "Error";
}

namespace NSystem {
	//----------------------------------------------------------

	CException::CException(const char* _pMessage, const char*  _pFileName, int _line)
	{
		mMessage = "";	// 初期化

		// 詳細情報を付加
		if (_pFileName != nullptr) {
			mMessage += "File : \n";
			mMessage += _pFileName;
			mMessage += "\n\n";
		}
		if (_line != 0) {
			mMessage += "line : (";
			mMessage += std::to_string(_line);
			mMessage += ")\n\n";
		}
		// メッセージを追加
		mMessage += "Message : \n";
		mMessage += _pMessage;
	}

	void CException::OutputMessage()
	{
		MessageBox(NWindows::gHWnd, mMessage.data(), pTitleName, MB_OK | MB_ICONSTOP);
	}
}
