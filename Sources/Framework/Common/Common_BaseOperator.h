#pragma once

//!	@brief	基本的な演算子を定義する

#include	<stdlib.h>
#include	<crtdbg.h>
#include	<malloc.h>

//==========================================================
// アロケータ関係
//==========================================================

//!	@brief	配列のサイズを求める
#define		ArraySize(_array)		_countof(_array)

#define New	new(16, _NORMAL_BLOCK, __FILE__, __LINE__)


//!	@brief	アライメントを指定できるプレースメントnew
void* operator new(
	size_t		_size,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw();

//!	@brief	プレースメントnewに対応したdelete
void operator delete(
	void*		 _p,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw();

//!	@brief	アライメントを指定できるプレースメントnew[]
void* operator new[](
	size_t		_size,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw();

//!	@brief	プレースメントnewに対応したdelete[]
void operator delete[](
	void*		_p,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw();


//!	@brief	グローバルのnewのオーバーロード(他のライブラリから使用できるようにする)
void* operator new(size_t _size) throw();

//!	@brief	グローバルのdeleteのオーバーロード
void operator delete(void* _p) throw();

//!	@brief	グローバルのnew[]のオーバーロード(他のライブラリから使用できるようにする)
void* operator new[](size_t _size) throw();

//!	@brief	グローバルのdelete[]のオーバーロード
void operator delete[](void* _p) throw();



//!	@brief	ポインタを開放
#define DeleteSafe(_pTarget) {	\
	if(_pTarget != nullptr) {	\
		delete _pTarget;		\
		_pTarget = nullptr;		\
	}							\
}

//!	@brief	配列を開放
#define DeleteArraySafe(_pTarget) {	\
	if(_pTarget != nullptr) {		\
		delete[] _pTarget;			\
		_pTarget = nullptr;			\
	}								\
}



//==========================================================
// キャスト変換
//==========================================================

#define		StaticCast			static_cast
#define		ConstCast			const_cast
#define		ReinterpretCast		reinterpret_cast

// dynamic_castは遅いので、Release時はstatic_castとして扱う
#ifdef _GAME_DEBUG
#define		DynamicCast			dynamic_cast
#else 
#define		DynamicCast			static_cast
#endif


namespace NCommon {

}