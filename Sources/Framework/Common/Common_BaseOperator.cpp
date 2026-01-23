#include	"Common_BaseOperator.h"
#include	"Windows\Windows_Include.h"

// アライメントを指定できるプレースメントnew
void* operator new(
	size_t		_size,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw()
{
	UNREFERENCED_PARAMETER(_blockUse);
	void* p = _aligned_malloc_dbg(_size, _alignment, _fileName, _lineNumber);
	assert(p != nullptr);
	return p;
}

// プレースメントnewに対応したdelete
void operator delete(void*		 _p,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw()
{
	UNREFERENCED_PARAMETER(_alignment);
	UNREFERENCED_PARAMETER(_blockUse);
	UNREFERENCED_PARAMETER(_fileName);
	UNREFERENCED_PARAMETER(_lineNumber);

	_aligned_free_dbg(_p);
}

// アライメントを指定できるプレースメントnew[]
void* operator new[](size_t		_size,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw()
{
	UNREFERENCED_PARAMETER(_blockUse);
	void* p = _aligned_malloc_dbg(_size, _alignment, _fileName, _lineNumber);
	assert(p != nullptr);
	return p;
}

// プレースメントnewに対応したdelete[]
void operator delete[](void*		_p,
	size_t		_alignment,
	int			_blockUse,
	const char* _fileName,
	int			_lineNumber) throw()
{
	UNREFERENCED_PARAMETER(_alignment);
	UNREFERENCED_PARAMETER(_blockUse);
	UNREFERENCED_PARAMETER(_fileName);
	UNREFERENCED_PARAMETER(_lineNumber);

	_aligned_free_dbg(_p);
}



void* operator new(size_t _size) throw()
{
	void* p = _aligned_malloc_dbg(_size, 16, "Global operator new", 0);
	return p;
}

void operator delete(void* _p) throw()
{
	_aligned_free_dbg(_p);
}

void* operator new[](size_t _size) throw() {
	void* p = _aligned_malloc_dbg(_size, 16, "Global operator new", 0);
	return p;
}

void operator delete[](void* _p) throw() {
	_aligned_free_dbg(_p);
}
