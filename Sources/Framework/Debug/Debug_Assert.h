#pragma once

// Assert関係関数定義

namespace NDebug {
	//!	@brief	アサートメッセージの表示を行う
	void OutAssertMessage(
		const char*		_pMessage,
		const char*		_pFile,
		unsigned int	_line);
}

//!	@brief	独自定義のアサーションを定義
//!	@note	リリース時にも機能させるため
#ifdef _GAME_DEBUG
#undef	myAssert
#define myAssert(expr)								\
	if(!static_cast<bool>(expr)) {					\
		NDebug::OutAssertMessage(					\
			static_cast<const char*>(#expr),		\
			static_cast<const char*>(__FILE__),		\
			static_cast<unsigned int>(__LINE__));	\
	}											
#else 
#define myAssert(expr)		((void)0)
#endif

//! @brief		基本のAssert関数
#define Assert(expr)			myAssert(expr)

//!	@brief		Nullであることを検証する
#define	AssertNull(var)			Assert(var == nullptr)

//!	@brief		Nullでないことを検証する
#define AssertNotNull(var)		Assert(var != nullptr)

//!	@brief		値を検証する
#define AssertPlus(var)			Assert(var > 0)
#define AssertMinus(var)		Assert(var < 0)
#define AssertZero(var)			Assert(var == 0)
#define AssertNotMinus(var)		Assert(var >= 0)

