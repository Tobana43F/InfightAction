#pragma once

//!	@brief		ファイル名と行の情報を付加するマクロ
//!	@note		エラーを処理する際に利用してください
#define ErrorEx(_pMessage)	NSystem::Error(_pMessage, __FILE__, __LINE__)
#define	OutErrorMessageEx(_pMessage) NSystem::OutErrorMessage(_pMessage, __FILE__, __LINE__)
#define	OutWarningMessageEx(_pMessage) NSystem::OutWarningMessage(_pMessage, __FILE__, __LINE__)


namespace NSystem {
	class CException;

	//!	@brief		エラーを処理する関数
	//!	@details	CExceptionクラスを生成し、例外を投げる
	void Error(
		const char*	_pMessage,
		const char* _pFileName,
		int			_line);

	//!	@brief		エラーメッセージを表示する
	//!	@details	例外は投げない
	//!	@note		エラー報告のみ行い、プログラムを停止したくないときに使用する。
	void OutErrorMessage(
		const char*	_pMessage,
		const char* _pFileName,
		int			_line);

	//!	@brief		警告メッセージを表示する
	//!	@details	エラーではないが注意すべき状態を報告するときに使用する。
	void OutWarningMessage(
		const char*	_pMessage,
		const char* _pFileName,
		int			_line);
}
