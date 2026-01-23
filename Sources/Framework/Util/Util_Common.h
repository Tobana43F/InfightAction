#pragma once

#include	<string>

#undef		CompareString

//!	汎用的な関数をまとめたヘッダ

namespace NUtil {

	//!	@brief	wcharをcharに変換数
	void ToString(std::string& _rDest, cwchar* _pWStr);

	//!	@brief	charをwcharに変換
	void ToWString(std::wstring& _rDest, cchar* _pStr);

	//!	@brief	フォーマットを指定して文字列を作成する
	void MakeString(std::string& _rDest, cchar* _pFmt, ...);

	//!	@brief			文字列内の特定文字列を指定した文字列で置き換える
	//!	@param[in/out]	_rDest		文字列。結果の代入先。
	//!	@param[in]		_rTarget	置き換え対象の文字列
	//!	@param[in]		_rReplace	置き換える文字列
	void ReplaceString(
		std::string&		_rDest,
		const std::string&	_rTarget,
		const std::string&	_rReplace);

	//!	@brief		文字列の比較をboola型で返す関数
	//!	@return		一致していたらtrueを返す
	bool CompareString(const char* _string1, const char* _string2);
	bool CompareString(const std::string _string1, const char* _string2);
	bool CompareString(const std::string _string1, const std::string _string2);
}