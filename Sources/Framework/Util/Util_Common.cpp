#include	"Util\Util_Common.h"

#include	<array>
#include	<cstdarg>
#include	<string>
#include	<stdlib.h>

namespace {
	const uint BUF_SIZE = 1024;
}

namespace NUtil {

	void ToString(std::string& _rDest, cwchar* _pWStr)
	{
		std::wstring	tmpWStr(_pWStr);
		std::string		tmpStr(tmpWStr.begin(), tmpWStr.end());
		_rDest = tmpStr;
	}

	void ToWString(std::wstring& _rDest, cchar* _pStr)
	{
		std::string			tmpStr(_pStr);
		std::wstring		tmpWStr(tmpStr.begin(), tmpStr.end());
		_rDest = tmpWStr;
	}

	void MakeString(std::string& _rDest, cchar* _pFmt, ...)
	{
		va_list	arg;
		char	buf[BUF_SIZE];

		va_start(arg, _pFmt);
		vsprintf_s<sizeof(buf)>(buf, _pFmt, arg);
		va_end(arg);

		_rDest = buf;
	}

	void ReplaceString(
		std::string&		_rDest,
		const std::string&	_rTarget,
		const std::string&	_rReplace)
	{
		std::string::size_type pos(_rDest.find(_rTarget));
		while (pos != std::string::npos) {
			_rDest.replace(pos, _rTarget.length(), _rReplace);
			pos = _rDest.find(_rTarget, pos + _rReplace.length());
		}
	}

	bool CompareString(const char* _string1, const char* _string2)
	{
		return (strcmp(_string1, _string2) == 0);
	}

	bool CompareString(const std::string _string1, const char* _string2)
	{
		return (_string1.compare(_string2) == 0);
	}

	bool CompareString(const std::string _string1, const std::string _string2)
	{
		return (_string1.compare(_string2) == 0);
	}

}