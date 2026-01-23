#include	"Game\Game_EnemyCreator.h"

#include	<string>
#include	<unordered_map>
#include	"Debug\Debug_Assert.h"

// 全ての敵ヘッダのインクルード
#include	"Object\Actor\Enemy\Object_SandBag.h"
#include	"Object\Actor\Enemy\Object_Baby.h"

namespace {

	//!	@brief	敵を生成する関数ポインタ
	using FPCreateEnemy = NObject::CEnemy*(*)();

	//!	@brief	敵クラスを継承したオブジェクトを生成する関数
	template<class TEnemyInherit>
	NObject::CEnemy*	CreateEnemy()
	{
		return DynamicCast<NObject::CEnemy*>(New TEnemyInherit);
	}

	//!	@brief	敵のIDと生成関数を結び付けた変数
	//!	@note	敵を追加して使用する場合は、ここに追加すること。
	std::unordered_map<std::string, FPCreateEnemy>	m_enemyCreateFuncPtrs =
	{
		{ "CSandBag", ::CreateEnemy<NObject::CSandBag> },
		{ "CBaby", ::CreateEnemy<NObject::CBaby> }
	};
}

namespace NGame {

	//!	@brief	クラス名をもとに敵クラスを生成する
	NObject::CEnemy*	CreateEnemy(const char* _pEnemyClassName)
	{
		if (m_enemyCreateFuncPtrs.count(_pEnemyClassName) <= 0) {
			std::string	err = "敵生成時に不正な名前が指定されました。または生成関数が登録されていません。\n";
			err += "指定した名前 : ";
			err += _pEnemyClassName;
			throw(err.data());
		}

		return m_enemyCreateFuncPtrs[_pEnemyClassName]();
	}
}
