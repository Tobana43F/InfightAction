#include	"Collision_Manager.h"

#include	"..\CollisionProc\Collision_Proc.h"
#include	"..\CollisionProc\Collision_HitTest.h"
#include	"Collision\Collision_Mesh.h"
#include	"Debug\Debug_Assert.h"

namespace {
	//	登録用配列のサイズ
	constexpr	size_t	STAGE_COLLISION_SIZE = StaticCast<size_t>(NCollision::EStageCollisionIndex::MAX);
	constexpr	size_t	HIT_TEST_PROC_SIZE = StaticCast<size_t>(NCollision::EHitTestProcManagerIndex::MAX);
	
	// ステージ用の当たり判定マネージャ
	std::array<NCollision::CMesh*, STAGE_COLLISION_SIZE>		g_pStageCollisions;

	// HitTest専用の登録先
	std::array<NCollision::CProcHitTest*, HIT_TEST_PROC_SIZE>	g_pHitTestProcs;



	//!	@brief	ステージ用の当たり判定マネージャの初期化
	void SetupStageCollisionManager();

	//!	@brief	HitTestクラスのマネージャを初期化
	void SetupHitTestProc();
}

namespace NCollision {

	//----------------------------------------------------------
	// 汎用
	//----------------------------------------------------------

	void Setup()
	{
		SetupStageCollisionManager();
		SetupHitTestProc();
	}

	//----------------------------------------------------------
	// ステージ用の当たり判定マネージャ関係
	//----------------------------------------------------------

	void RegisterStageCollision(CMesh* _pStageCollision, EStageCollisionIndex _index)
	{
		Assert(_index < EStageCollisionIndex::MAX && "引数が無効です");
		AssertNotNull(_pStageCollision);
		int index = StaticCast<int>(_index);

		//!	@note	上書きを許容するようにする
		//AssertNull(g_pStageCollisions[index] && "当たり判定情報が上書きされました。");
		g_pStageCollisions[index] = _pStageCollision;
	}

	void UnregisterStageCollision(CMesh* _pMeshCollision)
	{
		AssertNotNull(_pMeshCollision);

		// 登録されているポインタを探索
		for (uint i = 0; i < STAGE_COLLISION_SIZE; ++i) {
			if (g_pStageCollisions[i] == _pMeshCollision) {
				g_pStageCollisions[i] = nullptr;
			}
		}
	}

	CMesh* GetStageCollision(EStageCollisionIndex _index)
	{
		Assert(_index < EStageCollisionIndex::MAX && "引数が無効です");
		int index = StaticCast<int>(_index);

		if (g_pStageCollisions[index] == nullptr) {
			ErrorEx("指定した_indexにステージ用当たり判定が登録されていません。");
		}

		return g_pStageCollisions[index];
	}

	//----------------------------------------------------------
	// HitTestProc関係
	//----------------------------------------------------------
	void RegisterHitTestProc(CProcHitTest* _pProcHitTest, EHitTestProcManagerIndex _index)
	{
		Assert(_index < EHitTestProcManagerIndex::MAX && "引数が無効です");
		AssertNotNull(_pProcHitTest);
		int index = StaticCast<int>(_index);

		// 指定されたインデックスに登録
		AssertNull(g_pHitTestProcs[index]);
		g_pHitTestProcs[index] = _pProcHitTest;
	}

	void UnregisterHitTestProc(CProcHitTest* _pProcHitTest)
	{
		AssertNotNull(_pProcHitTest);

		// 登録されているProcを探索
		for (uint i = 0; i < HIT_TEST_PROC_SIZE; ++i) {
			if (g_pHitTestProcs[i] == _pProcHitTest) {
				g_pHitTestProcs[i] = nullptr;
			}
		}
	}

	CProcHitTest*	GetHitTestProc(EHitTestProcManagerIndex _index)
	{
		Assert(_index < EHitTestProcManagerIndex::MAX && "引数が無効です");
		int index = StaticCast<int>(_index);

		if (g_pHitTestProcs[index] == nullptr) {
			ErrorEx("指定した_indexにHitTestクラスが登録されていません。");
		}

		return g_pHitTestProcs[index];
	}
}

namespace {

	void SetupStageCollisionManager()
	{
		// nulllptrで初期化
		for (uint i = 0; i < STAGE_COLLISION_SIZE; ++i) {
			g_pStageCollisions[i] = nullptr;
		}
	}

	void SetupHitTestProc()
	{
		// nullptrで初期化
		for (uint i = 0; i < HIT_TEST_PROC_SIZE; ++i) {
			g_pHitTestProcs[i] = nullptr;
		}
	}
}