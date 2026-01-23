#include	"System_Error.h"

#include	<string>
#include	"System_Exception.h"
#include	"Windows\Windows_Include.h"
#include	"Windows\Windows_Global.h"

namespace {

	//!	@brief	エラーメッセージを作成する
	void MakeErrorMessage(
		std::string&	_rDest,
		const char*		_pMessage,
		const char*		_pFileName,
		int				_line)
	{
		_rDest = "";	// 初期化

						// 詳細情報を付加
		if (_pFileName != nullptr) {
			_rDest += "File : \n";
			_rDest += _pFileName;
			_rDest += "\n\n";
		}
		if (_line != 0) {
			_rDest += "line : (";
			_rDest += std::to_string(_line);
			_rDest += ")\n\n";
		}
		// メッセージを追加
		_rDest += "Message : \n";
		_rDest += _pMessage;
	}
}

namespace NSystem {

	void Error(
		const char*	_pMessage,
		const char*	_pFileName,
		int			_line)
	{
		CException	exeption(_pMessage, _pFileName, _line);
		throw(exeption);
	}

	void OutErrorMessage(
		const char*	_pMessage,
		const char* _pFileName,
		int			_line)
	{
		std::string	msg;
		MakeErrorMessage(msg, _pMessage, _pFileName, _line);
		MessageBox(NWindows::gHWnd, msg.data(), "Error", MB_OK | MB_ICONSTOP);
	}

	void OutWarningMessage(
		const char*	_pMessage,
		const char* _pFileName,
		int			_line)
	{
		std::string	msg;
		MakeErrorMessage(msg, _pMessage, _pFileName, _line);
		MessageBox(NWindows::gHWnd, msg.data(), "Warning", MB_OK | MB_ICONWARNING);
	}
}
