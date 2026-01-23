#include	"Camera_OperatorManager.h"

#include	<array>
#include	<memory>
#include	"Debug\Debug_Assert.h"
#include	"System\System_Error.h"

namespace {
	//	登録用配列のサイズ
	constexpr	size_t	ARRAY_SiZE = StaticCast<size_t>(NCamera::EManagerIndex::MAX);

	// オペレータの登録先
	std::array<NCamera::COperatorBase*, ARRAY_SiZE>	g_pOperatorHolder;
}

namespace NCamera {

	void SetupOperatorManager()
	{
		//	nullptrで初期化
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			g_pOperatorHolder[i] = nullptr;
		}
	}
	
	void UpdateOperatorManager()
	{
		// 全てのカメラに対するオペレータを更新
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			if (g_pOperatorHolder[i] != nullptr) {
				g_pOperatorHolder[i]->Update();
			}
		}
	}

	COperatorBase* RegisterOperator(
		COperatorBase*			_pOperator,
		EManagerIndex		_index)
	{
		Assert(_index < EManagerIndex::MAX && "引数が無効です");
		int index = StaticCast<int>(_index);

		// エラーチェック
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			if (i == index) {
				continue;
			}

			// 他のカメラに登録されている場合はエラーとして処理する
			if (g_pOperatorHolder[i] == _pOperator) {
				ErrorEx("同じカメラオペレータが複数のカメラに対応付けれらました。");
			}
		}

		// 登録済みの値を取得
		COperatorBase* prev = g_pOperatorHolder[index];
		// 登録
		g_pOperatorHolder[index] = _pOperator;
		return prev;
	}

	void UnregisterOperator(COperatorBase*	_pOperator)
	{
		AssertNotNull(_pOperator);

		// 登録されているカメラを探索
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			if (g_pOperatorHolder[i] == _pOperator) {
				g_pOperatorHolder[i] = nullptr;
			}
		}
	}
}