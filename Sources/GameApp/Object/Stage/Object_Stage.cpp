#include	"Object\Stage\Object_Stage.h"

#include	"Collision\Manager\Collision_Manager.h"
#include	"Debug\Debug_Util.h"

namespace NObject {
	CStage::CStage() :
		CGameObject(),
		m_sndBGMLabel(NSound::ELabel::BGM_BATTLE1)
	{

	}

	CStage::~CStage()
	{
		// 登録した当たり判定をマネージャから解除する
		NCollision::UnregisterStageCollision(&m_colGround);
		NCollision::UnregisterStageCollision(&m_colWall);
	}

	void CStage::SetStageCollision()
	{
		// 地形のあたり判定マネージャに登録する
		NCollision::RegisterStageCollision(
			&m_colGround,
			NCollision::EStageCollisionIndex::GROUND_MAIN);
		NCollision::RegisterStageCollision(
			&m_colWall,
			NCollision::EStageCollisionIndex::WALL_MAIN);
	}

	//----------------------------------------------------------

	void CStage::LoadProc()
	{
		CGameObject::LoadProc();

		// 地面の描画用モデルをロード
		m_modelGround.Load("Stage/Ground.fbx");

		// 当たり判定データ読み込み
		m_colGround.Load("Stage/GroundCollision.fbx");
		m_colWall.Load("Stage/WallCollision.fbx");

	}

	void CStage::ReleaseProc()
	{
		CGameObject::ReleaseProc();
		m_modelGround.Release();
		m_colGround.Release();
		m_colWall.Release();

	}

	void CStage::InitProc()
	{
		CGameObject::InitProc();
		m_skyDome.Init();

		// BGM再生
		NSound::Play(m_sndBGMLabel);
	}

	void CStage::DestroyProc()
	{
		CGameObject::DestroyProc();
		m_skyDome.Destroy();

		// BGM終了
		NSound::Stop(m_sndBGMLabel);
	}

	void CStage::UpdateProc()
	{
		CGameObject::UpdateProc();
		m_skyDome.Update();
	}

	void CStage::RenderProc()
	{
		CGameObject::RenderProc();

		// ステージを描画
		NMath::SMatrix44 matWorld;
		GetTransform().CalcWorldMatrix(matWorld);
		m_modelGround.RenderDebug(matWorld);

		// スカイドームを描画
		m_skyDome.Render();

		// ステージのあたり判定を描画する
		m_colGround.RenderNormal();
		m_colWall.RenderNormal();
	}
}
