#pragma once

#include	<string>

namespace NSystem {

	//!	@brief		例外を処理するクラス
	class CException {
	public:
		//!	@brief		メッセージを表示
		void OutputMessage();

	private:
		std::string		mMessage;

	public:
		CException(
			const char* _pMessage, 
			const char* _pFileName = nullptr, 
			int			_line = 0);

		~CException() { }
	};
}
