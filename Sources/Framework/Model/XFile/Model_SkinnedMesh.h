//========================================================
//CSkinnedMeshクラス
//========================================================
#pragma once
#include	<d3d9.h>
#include	<d3dx9.h>
#include	<vector>
#include	<string>

#include	"Model_Hierarchy.h"
#include	"DirectX\DirectX_Util.h"
#include	"OBB\OBB.h"
#include	"Math\Math_Type.h"
#include	"System\System_Def.h"
#include	"DirectX\DirectX_Manager.h"

namespace NModel {

	//!	@brief	ワンスキンモデルの管理・描画を行うためのクラス

	class CSkinnedMesh {
	protected:
		std::string					m_fileDir;

		LPD3DXFRAME					m_pFrameRoot;			//フレームルート
		LPD3DXANIMATIONCONTROLLER	m_pAnimController;		//アニメーションコントローラー
		D3DXVECTOR3					m_vObjectCenter;		//オブジェクトのバウンディングスフィアの中心
		float						m_fObjectRadius;		//オブジェクトのバウンディングスフィアの半径
		LPD3DXANIMATIONSET			m_AnimationSet;			//アニメーションセット
		CHierarchy					m_cHierarchy;
		int							m_iAnimSetCount;
		unsigned int				m_uiAnimID;				//アニメーションID
		LPD3DXANIMATIONSET*			m_ppAnimSet;
		double						m_TimeDelta;			//アニメーション再生タイマー
		int							m_BoneNum;				//ボーンの数
		std::vector<LPD3DXFRAME>	m_AllFrameContainer;	//すべてのフレームを格納したベクター
		double						m_endtime;

		COBB*						m_OBBSkin;				// ワンスキン用OBBオブジェクト					
		NMath::SMatrix44*			m_matBone;				// ボーン行列

		int							m_BBOXnum;				//BBOXの個数

		// 外部から、アニメーションの再生フレーム情報を格納する先
		bool						m_hasExternalAnmFrame;
		std::vector<float>			m_anmFrame;

	public:
		CSkinnedMesh(void);
		virtual ~CSkinnedMesh(void)
		{
			if (m_pFrameRoot) {
				ReleaseAttributeTable(m_pFrameRoot);
				ExitBBox(m_pFrameRoot);
				D3DXFrameDestroy(m_pFrameRoot, &m_cHierarchy);
			}
			ReleaseSafe(m_pAnimController);
			DeleteArraySafe(m_ppAnimSet);
		}

		void SetExternalAnmFrame(std::vector<float>& _frameInfo)
		{
			if (_frameInfo.size() != m_iAnimSetCount) {
				assert(false && "アニメーション数が等しくない");
			}
			m_anmFrame.swap(_frameInfo);
			m_hasExternalAnmFrame = true;
		}

		void Update(NMath::SMatrix44& _rMatWorld)
		{
			if (isMotionEnd() && false) {
				m_pAnimController->ResetTime();
				m_pAnimController->SetTrackPosition(0, 0.0);
				m_pAnimController->AdvanceTime(0.0f, NULL);
			}
			UpdateFrameMatrices(GetFrameRoot(), (LPD3DXMATRIX)&_rMatWorld);
			GetAnimationController()->AdvanceTime(NSystem::MILL_SECOND_PER_FRAME * 0.001, nullptr);
		}

		void Render(NMath::SMatrix44& _rMatWorld)
		{
			NDirectX::GetD3DDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&_rMatWorld);
			DrawFrame(NDirectX::GetD3DDevice(), GetFrameRoot());
		}

		bool LoadXFile(const char* xfilename, LPDIRECT3DDEVICE9 lpd3ddevice);															//ファイル読み込み
		void DrawMeshContainer(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);	//メッシュコンテナ読み込み
		void DrawFrame(LPDIRECT3DDEVICE9 pd3ddevice, LPD3DXFRAME pFrame);														//フレーム描画
		void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);											//フレームマトリクスの更新
		void ReleaseAttributeTable(LPD3DXFRAME pFrameBase);																		//アトリビュートテーブルの解放
		bool ChangeAnimation(int AnimID);																						//アニメーションを変更																				//アニメーション更新
		HRESULT SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase);											//メッシュコンテナのボーンマトリックスポインター設定
		HRESULT SetupBoneMatrixPointers(LPD3DXFRAME pFrame);

		HRESULT CreateSkinnedBBox(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXMESHCONTAINER pMeshContainerParameter);

		void InitBBox(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXFRAME pFrame);
		void UpdateBBox(LPD3DXFRAME pFrameBase);
		void DrawBBox(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXFRAME pFrame);

		void ExitBBox(LPD3DXFRAME pFrameBase);

		void GetBoneMatrix(const int _idx, NMath::SMatrix44& _rDest) const
		{
			assert(_idx < m_BoneNum);
			assert(_idx >= 0);
			_rDest = m_matBone[_idx];
		}

		// 全てのあたり判定;を無効にする
		void SetIsCollisionActive(bool _state)
		{
			for (int i = 0; i < m_BBOXnum; ++i) {
				m_OBBSkin[i].SetIsActive(_state);
			}
		}
		void SetIsCollisionActive(int _idx, bool _state)
		{
			// obbを指定して設定する
			m_OBBSkin[_idx].SetIsActive(_state);
		}

		uint GetAnmID()
		{
			return m_uiAnimID;
		}

		float GetTime()
		{
			return (float)m_TimeDelta;
		}

		//フレームのボーンマトリックスポインターの設定
		// ボーン数を取得する
		int	 GetBoneNum() const
		{
			return m_BoneNum;
		}

		//全フレームの先頭アドレスをベクターコンテナに格納する
		void GetAllFrame(LPD3DXFRAME pFrame);

		//フレームルート取得
		LPD3DXFRAME GetFrameRoot()
		{
			return m_pFrameRoot;
		}

		//アニメーションコントローラー取得
		LPD3DXANIMATIONCONTROLLER GetAnimationController()
		{
			return m_pAnimController;
		}

		//アニメーションセットを取得
		LPD3DXANIMATIONSET GetAnimationSet(int idx)
		{
			return m_ppAnimSet[idx];
		}

		//アニメーションセット数を取得
		int GetAnimationSetCount()
		{
			return m_iAnimSetCount;
		}

		//アニメーション再生タイマーをセット
		void SetTimeDelta(double TimeDelta)
		{
			m_TimeDelta = TimeDelta;
		}

		// フレームコンテナを返す
		std::vector<LPD3DXFRAME>* GetFrameContainer()
		{
			return &m_AllFrameContainer;
		}

		bool isMotionEnd()
		{
			double NowAnimTime = m_pAnimController->GetTime();
			if (NowAnimTime > m_endtime) {
				return true;
			}
			else {
				return false;
			}
		}

		double GetEndTime()
		{
			return m_endtime;
		}

		int		GetOBBNum() { return m_BBOXnum; }
		COBB*	GetOBB(int _idx) { return &m_OBBSkin[_idx]; }

	};
}
