#include "Model_SkinnedMesh.h"
#include <string>
#include <stdio.h>

#include "System\System_Error.h"

namespace NModel {

	//==================================================================
	//　コンストラクタ
	//===================================================================
	CSkinnedMesh::CSkinnedMesh(void) :m_ppAnimSet(NULL)
	{}

	//==================================================================
	//　階層構造を持つＸファイルの読み込み
	//===================================================================
	bool CSkinnedMesh::LoadXFile(const char *xfilename, LPDIRECT3DDEVICE9 lpd3ddevice)
	{
		HRESULT hr;

		//　階層構造を持つＸファイルを読み込む
		std::string modelFilePath = xfilename;
		int dirNum;
		dirNum = modelFilePath.find_last_of('/') + 1;	// 最後の区切り文字までの文字数を取得する
														// 区切り文字を含める

		std::string modelFileDir;
		modelFileDir = modelFilePath.substr(0, dirNum);

		m_fileDir = modelFileDir.data();

		m_cHierarchy.SetFileDir(modelFileDir.data());	// ファイルのディレクトリを教える


		//　階層構造を持つＸファイルを読み込む
		hr = D3DXLoadMeshHierarchyFromX(xfilename,					// ファイル名
			D3DXMESH_MANAGED,
			lpd3ddevice,				// デバイスオブジェクト
			&m_cHierarchy,				// 階層構造を読み込むためのクラス
			NULL,
			&m_pFrameRoot,				// 最上位のフレーム
			&m_pAnimController);		// アニメ―ションコントローラ
		if (FAILED(hr)) {
			ErrorEx("モデル読み込み失敗");
			return false;
		}

		// ボーン数分のボーン行列を格納するエリアを確保し、合成行列のポインタをセットする
		SetupBoneMatrixPointers(m_pFrameRoot);

		//　境界球を求める
		D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius);

		// すべてのフレームをＶＥＣＴＯＲコンテナにセットする
		GetAllFrame(m_pFrameRoot);

		//アニメーショントラックを取得
		if (m_pAnimController != NULL) {
			m_iAnimSetCount = m_pAnimController->GetNumAnimationSets();		// アニメーションセット数を取得

			if (m_iAnimSetCount > 0) {
				//アニメーションセット保存エリアを確保
				m_ppAnimSet = new LPD3DXANIMATIONSET[m_iAnimSetCount];
				for (int i = 0; i < m_iAnimSetCount; i++) {
					m_pAnimController->GetAnimationSet(i, &m_ppAnimSet[i]);			// アニメーションセットを取得
				}	
			}
		}

		InitBBox(lpd3ddevice, m_pFrameRoot);

		// 初期化
		m_hasExternalAnmFrame = false;
		m_anmFrame.clear();
		return true;
	}

	//==============================================================
	//メッシュコンテナ描画
	//=============================================================
	void CSkinnedMesh::DrawMeshContainer(LPDIRECT3DDEVICE9 pd3ddevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
	{
		HRESULT hr;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
		D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
		UINT iMaterial;
		UINT NumBlend;
		UINT iAttrib;
		DWORD AttribIdPrev;
		LPD3DXBONECOMBINATION pBoneComb;

		UINT iMatrixIndex;
		D3DXMATRIXA16 matTemp;
		D3DCAPS9 d3dCaps;
		pd3ddevice->GetDeviceCaps(&d3dCaps);			// グラフィックカードの性能を取得する

														//ワンスキンの情報があるか？
		if (pMeshContainer->pSkinInfo != NULL) {
			AttribIdPrev = UNUSED32;
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

			//HW処理で描画
			// このメッシュコンテナのアトリビュートグループ数分ループ（ボーン数）
			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
				NumBlend = 0;
				// 影響を受けるボーン数
				for (DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
					if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX) {
						NumBlend = i;
					}
				}

				// グラフィックカードの頂点ブレンディングのＭＡＸ数を超えていないか
				if (d3dCaps.MaxVertexBlendMatrices >= NumBlend) {
					//ブレンドウェイト用ワールドマトリクスを計算
					for (DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
						iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
						if (iMatrixIndex != UINT_MAX) {
							D3DXMatrixMultiply(&matTemp,
								&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],		// ボーンオフセット行列 
								pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);		// ボーン行列

							hr = pd3ddevice->SetTransform(D3DTS_WORLDMATRIX(i), &matTemp);	// 頂点ブレンド用行列をセットする
							if (FAILED(hr)) {
								MessageBox(NULL, "error", "SetTransform error", MB_OK);
							}
						}
					}

					hr = pd3ddevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
					if (FAILED(hr)) {
						MessageBox(NULL, "error", "pd3ddevice->SetRenderState error", MB_OK);
					}

					//マテリアル面がサブセットに使用したルックアップ
					if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32)) {
						hr = pd3ddevice->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
						if (FAILED(hr)) {
							MessageBox(NULL, "error", "pd3ddevice->SetMaterial error", MB_OK);
						}
						hr = pd3ddevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
						if (FAILED(hr)) {
							MessageBox(NULL, "error", "pd3ddevice->SetTextre error", MB_OK);
						}
						AttribIdPrev = pBoneComb[iAttrib].AttribId;
					}

					//メッシュ描画
					hr = pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
					if (FAILED(hr)) {
						MessageBox(NULL, "error", "DrawSubset error", MB_OK);
					}
				}
			}

			//HWが扱うことが出来なかったサブセットの描画
			if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups) {
				AttribIdPrev = UNUSED32;
				hr = pd3ddevice->SetSoftwareVertexProcessing(TRUE);
				if (FAILED(hr)) {
					MessageBox(NULL, "error", "SetSoftwareVertexProcessing error", MB_OK);
				}

				for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
					NumBlend = 0;
					for (DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
						if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
							NumBlend = i;
					}

					if (d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1) {
						//ブレンドウェイト用ワールドマトリクスを計算
						for (DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
							iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
							if (iMatrixIndex != UINT_MAX) {
								D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
								hr = pd3ddevice->SetTransform(D3DTS_WORLDMATRIX(i), &matTemp);
								if (FAILED(hr)) {
									MessageBox(NULL, "error", "SetTransform error", MB_OK);
								}
							}
						}

						hr = pd3ddevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
						if (FAILED(hr)) {
							MessageBox(NULL, "error", "SetRenderSrarte error", MB_OK);
						}


						//マテリアル面がサブセットに使用したルックアップ
						if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32)) {
							hr = pd3ddevice->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
							if (FAILED(hr)) {
								MessageBox(NULL, "error", "SetMaterial error", MB_OK);
							}
							hr = pd3ddevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
							if (FAILED(hr)) {
								MessageBox(NULL, "error", "SetTexture error", MB_OK);
							}
							AttribIdPrev = pBoneComb[iAttrib].AttribId;
						}

						//メッシュ描画
						hr = pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
					}
				}
				//頂点計算の設定を元に戻す
				hr = pd3ddevice->SetSoftwareVertexProcessing(FALSE);
				if (FAILED(hr)) {
					MessageBox(NULL, "error", "SetSoftwareVertexProcessing", MB_OK);
				}
			}

			hr = pd3ddevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
			if (FAILED(hr)) {
				MessageBox(NULL, "error", "SetRenderState error", MB_OK);
			}
		}
		else { //マルチメッシュ描画
			hr = pd3ddevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformMatrix);
			if (FAILED(hr)) {
				MessageBox(NULL, "error", "SetTransform error", MB_OK);
			}

			for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++) {
				hr = pd3ddevice->SetMaterial(&pMeshContainer->pMaterials[iMaterial].MatD3D);
				if (FAILED(hr)) {
					MessageBox(NULL, "error", "SetMaterial error", MB_OK);
				}
				hr = pd3ddevice->SetTexture(0, pMeshContainer->ppTextures[iMaterial]);
				if (FAILED(hr)) {
					MessageBox(NULL, "error", "SetTexture error", MB_OK);
				}
				hr = pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
				if (FAILED(hr)) {
					MessageBox(NULL, "error", "DrawSubset error", MB_OK);
				}
			}
		}
	}

	//============================================================================
	//フレーム描画
	//===========================================================================
	void CSkinnedMesh::DrawFrame(LPDIRECT3DDEVICE9 pd3ddevice, LPD3DXFRAME pFrame)
	{
		LPD3DXMESHCONTAINER pMeshContainer;

		pMeshContainer = pFrame->pMeshContainer;
		while (pMeshContainer != NULL) {
			DrawMeshContainer(pd3ddevice, pMeshContainer, pFrame);

			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		//兄弟がいるなら再帰的にフレーム描画
		if (pFrame->pFrameSibling != NULL) {
			DrawFrame(pd3ddevice, pFrame->pFrameSibling);
		}

		//子がいるなら再帰的にフレーム描画
		if (pFrame->pFrameFirstChild != NULL) {
			DrawFrame(pd3ddevice, pFrame->pFrameFirstChild);
		}

	}

	//====================================================================
	//アニメーション変更
	//====================================================================
	bool CSkinnedMesh::ChangeAnimation(int AnimID)
	{
		LPD3DXANIMATIONSET AnimSet;
		D3DXTRACK_DESC	TrackDesc;

		// トラック情報の初期化
		ZeroMemory(&TrackDesc, sizeof(TrackDesc));
		TrackDesc.Weight = 1;
		TrackDesc.Speed = 1;
		TrackDesc.Enable = 1;

		AnimSet = GetAnimationSet(AnimID);
		m_pAnimController->SetTrackDesc(0, &TrackDesc);
		m_pAnimController->SetTrackAnimationSet(0, AnimSet);

		if (m_hasExternalAnmFrame) {
			m_endtime = m_anmFrame[AnimID];
		}
		else {
			m_endtime = AnimSet->GetPeriod();
		}

		m_pAnimController->SetTrackEnable(0, true);
		m_pAnimController->SetTrackEnable(1, false);
		m_pAnimController->ResetTime();
		m_pAnimController->SetTrackPosition(0, 0.0);
		m_pAnimController->AdvanceTime(0.0f, NULL);

		return true;
	}

	//=====================================================================
	// すべてのフレームをｖｅｃｔｏｒにセットする
	//=====================================================================
	void CSkinnedMesh::GetAllFrame(LPD3DXFRAME pFrame)
	{

		// フレームをコンテナにセット
		m_AllFrameContainer.push_back(pFrame);

		//兄弟がいるなら再帰的にフレーム描画
		if (pFrame->pFrameSibling != NULL) {
			GetAllFrame(pFrame->pFrameSibling);
		}

		//子がいるなら再帰的にフレーム描画
		if (pFrame->pFrameFirstChild != NULL) {
			GetAllFrame(pFrame->pFrameFirstChild);
		}
	}

	//==================================================================
	//メッシュ上のボーンマトリクスポインターをセット
	//==================================================================
	HRESULT CSkinnedMesh::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
	{
		UINT iBone, cBones;
		D3DXFRAME_DERIVED* pFrame;

		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	//#ifdef _DEBUG
	//	FILE* fp;
	//	fopen_s(&fp, "bonename.txt", "a");
	//#endif

		//スキンメッシュがあればボーン行列をセット
		if (pMeshContainer->pSkinInfo != NULL) {
			// ボーン数取得
			m_BoneNum = cBones = pMeshContainer->pSkinInfo->GetNumBones();

			// ボーン数分　行列エリアを確保
			pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
			if (pMeshContainer->ppBoneMatrixPtrs == NULL) {					// メモリの取得エラー
				return E_OUTOFMEMORY;
			}

			// ボーン数分ループ
			for (iBone = 0; iBone < cBones; iBone++) {
			//#ifdef _DEBUG
			//	fprintf(fp, " (%d) %s \n", iBone, pMeshContainer->pSkinInfo->GetBoneName(iBone));
			//#endif
				// 指定番号のボーンを持つフレームを探す
				pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
				if (pFrame == NULL) {		// 見つからなかった場合
					return E_FAIL;
				}
				// 親子関係を考慮した行列（ボーン行列をすべてかけたもの）のポインタをメッシュコンテナにセット
				pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformMatrix;
			}
		}
	//#ifdef _DEBUG
	//	fclose(fp);
	//#endif
		return S_OK;
	}

	//================================================================
	//ボーンマトリクスポインタをセット
	//================================================================
	HRESULT CSkinnedMesh::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
	{
		HRESULT hr;

		// メッシュコンテナが存在するか？
		if (pFrame->pMeshContainer != NULL) {
			hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
			if (FAILED(hr)) {
				return hr;
			}
		}

		// 兄弟がいるか？
		if (pFrame->pFrameSibling != NULL) {
			hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
			if (FAILED(hr)) {
				return hr;
			}
		}

		// 子供がいるか
		if (pFrame->pFrameFirstChild != NULL) {
			hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
			if (FAILED(hr)) {
				return hr;
			}
		}

		return S_OK;
	}

	//==================================================================
	//フレームマトリクス更新
	//==================================================================
	void CSkinnedMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
	{
		D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

		if (pParentMatrix != NULL) {
			D3DXMatrixMultiply(&pFrame->CombinedTransformMatrix, &pFrame->TransformationMatrix, pParentMatrix);
		}
		else
			pFrame->CombinedTransformMatrix = pFrame->TransformationMatrix;

		if (pFrame->pFrameSibling != NULL)
			UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);

		if (pFrame->pFrameFirstChild != NULL)
			UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformMatrix);

		for (int i = 0; i < m_BoneNum; i++) {
			m_matBone[i].SetIdentity();
		}
		UpdateBBox(m_pFrameRoot);
	}


	//===================================================================
	//アトリビュートテーブルの解放
	//==================================================================
	void CSkinnedMesh::ReleaseAttributeTable(LPD3DXFRAME pFrameBase)
	{
		D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer;

		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		while (pMeshContainer != NULL) {
			delete[] pMeshContainer->pAttributeTable;

			pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer;
		}

		if (pFrame->pFrameSibling != NULL) {
			ReleaseAttributeTable(pFrame->pFrameSibling);
		}

		if (pFrame->pFrameFirstChild != NULL) {
			ReleaseAttributeTable(pFrame->pFrameFirstChild);
		}
	}

	//==================================================================================
	//ワンスキン用のBBOXを作成する
	//==================================================================================
	HRESULT CSkinnedMesh::CreateSkinnedBBox(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXMESHCONTAINER pMeshContainerParameter)
	{
		LPD3DXSKININFO pSkinInfo = pMeshContainerParameter->pSkinInfo;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer;
		DWORD	BoneNum;
		DWORD	numVertices;
		D3DXVECTOR3* vertices;
		DWORD*	vindex;
		float*   weigths;
		D3DXMATRIX mat, eigenmat;
		D3DXVECTOR3* work;
		CBBoxGenerater Generater;

		//スキン情報があればワンスキン用のBBOXを作成する。
		LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
		D3DXVECTOR3* pdata = NULL;
		WORD* pindex = NULL;
		LPD3DXMESH pcloneMesh;

		//頂点座標のみのクローンメッシュを生成
		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerParameter;
		pMeshContainer->pOrigMesh->CloneMeshFVF(
			pMeshContainer->pOrigMesh->GetOptions(),		//元の頂点フォーマット
			D3DFVF_XYZ,										// 指定頂点フォーマット
			lpd3ddevice,
			&pcloneMesh);
		//頂点情報を取得
		if (FAILED(pcloneMesh->GetVertexBuffer(&pVB))) {
			return E_FAIL;
		}
		if (FAILED(pVB->Lock(0, sizeof(D3DXVECTOR3), (VOID**)&pdata, 0))) {
			ReleaseSafe(pVB);
			return E_FAIL;
		}
		vertices = (D3DXVECTOR3*)pdata;

		//ワークに頂点情報を格納
		numVertices = pcloneMesh->GetNumVertices();		// 頂点数を取得
		work = new D3DXVECTOR3[numVertices];			// 頂点数分メモリを確保

														//	memcpy(work,vertices,sizeof(D3DXVECTOR3)*numVertices);

		for (unsigned int jj = 0; jj < numVertices; jj++) {
			work[jj].x = vertices[jj].x;
			work[jj].y = vertices[jj].y;
			work[jj].z = vertices[jj].z;
		}

		pVB->Unlock();
		ReleaseSafe(pVB);

		//ボーン数取得
		BoneNum = m_BBOXnum = pSkinInfo->GetNumBones();

		m_OBBSkin = new COBB[BoneNum];
		m_matBone = New NMath::SMatrix44[BoneNum];

		for (uint i = 0; i < pSkinInfo->GetNumBones(); i++) {
			m_matBone[i].SetZero();
		}

	//#ifdef _DEBUG
	//	FILE* fp;
	//	fopen_s(&fp, "bone.txt", "w");
	//	fprintf(fp, "ボーン数 %d \n", BoneNum);
	//	fprintf(fp, "\n\n");
	//	fclose(fp);
	//#endif

		//ボーン毎にBBOXを生成(ボーン数分ループ)
		for (unsigned int i = 0; i < BoneNum; i++) {
			numVertices = pSkinInfo->GetNumBoneInfluences(i);
			vindex = new DWORD[numVertices];
			weigths = new float[numVertices];
			// ボーンの影響を受ける頂点インデックスを取得する
			pSkinInfo->GetBoneInfluence(
				i,					// ボーン番号
				vindex,				// 指定のボーンに影響をうける頂点番号
				weigths);			// 指定のボーンに影響を受ける重みデータ

		//#ifdef _DEBUG
		//							// デバッグ用にボーン情報を出力
		//	FILE* fp;
		//	fopen_s(&fp, "bone.txt", "a");
		//	for (unsigned int jj = 0; jj < numVertices; jj++) {
		//		fprintf(fp, "(%d) 座標(%f %f %f) ウエイト(%f) \n", vindex[jj], work[vindex[jj]].x, work[vindex[jj]].y, work[vindex[jj]].z, weigths[jj]);
		//	}
		//	fprintf(fp, "\n\n");
		//	fclose(fp);
		//#endif
			m_OBBSkin[i].PCA(work, vindex, numVertices);
			m_OBBSkin[i].CreateBoxMesh(lpd3ddevice);

			delete[] vindex;
			delete[] weigths;
		}

		//確保したメモリ解放
		delete[] work;

		return S_OK;
	}

	//============================================================================
	// BBOXを初期化する
	//===========================================================================
	void CSkinnedMesh::InitBBox(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXFRAME pFrame)
	{
		LPD3DXMESHCONTAINER pMeshContainer;

		pMeshContainer = pFrame->pMeshContainer;
		HRESULT	hr;

		// メッシュコンテナがある限りループする
		while (pMeshContainer != NULL) {
			if (pMeshContainer->pSkinInfo != NULL) {		// ワンスキンフレームか？
				hr = CreateSkinnedBBox(lpd3ddevice, pMeshContainer);
			}
			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		//兄弟がいるなら再帰的にＢＢＯＸを作成
		if (pFrame->pFrameSibling != NULL) {
			InitBBox(lpd3ddevice, pFrame->pFrameSibling);
		}

		//子がいるなら再帰的にＢＢＯＸを作成
		if (pFrame->pFrameFirstChild != NULL) {
			InitBBox(lpd3ddevice, pFrame->pFrameFirstChild);
		}
	}

	//====================================================================
	// BBOXの更新
	//===================================================================
	void CSkinnedMesh::UpdateBBox(LPD3DXFRAME pFrameBase)
	{
		D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer;

		int		bonenum;
		D3DXMATRIX matwork;

		if (pFrame->pMeshContainer != NULL) {
			if (pFrame->pMeshContainer->pSkinInfo != NULL) {
				// メッシュコンテナ取得
				pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;
				// ボーン数取得
				bonenum = pMeshContainer->pSkinInfo->GetNumBones();
				for (int i = 0; i < bonenum; i++) {
					// ボーンオフセット行列×ボーン行列（親子関係を考慮したものに仕上がってる）
					matwork = pMeshContainer->pBoneOffsetMatrices[i] * *pMeshContainer->ppBoneMatrixPtrs[i];
					m_OBBSkin[i].Update(matwork);

					NMath::SMatrix44	matWork44;
					for (int y = 0; y < 4; ++y) {
						for (int x = 0; x < 4; ++x) {
							//matWork44.flt[y][x] = matwork.m[y][x];
							matWork44.flt[y][x] = pMeshContainer->ppBoneMatrixPtrs[i]->m[y][x];
						}
					}

					m_matBone[i] = matWork44;
				}
			}
		}

		//兄弟がいるなら再帰的にフレーム描画
		if (pFrame->pFrameSibling != nullptr) {
			UpdateBBox(pFrame->pFrameSibling);
		}

		//子がいるなら再帰的にフレーム描画
		if (pFrame->pFrameFirstChild != nullptr) {
			UpdateBBox(pFrame->pFrameFirstChild);
		}
	}

	//==================================================================================
	//BBOXを描画する
	//==================================================================================
	void CSkinnedMesh::DrawBBox(LPDIRECT3DDEVICE9 lpd3ddevice, LPD3DXFRAME pFrame)
	{
		LPD3DXMESHCONTAINER pMeshContainer;

		pMeshContainer = pFrame->pMeshContainer;

		D3DXFRAME_DERIVED* pf = (D3DXFRAME_DERIVED*)pFrame;

		// メッシュコンテナがある限りループする
		while (pMeshContainer != NULL) {
			if (pMeshContainer->pSkinInfo != NULL) {							// ワンスキンフレームか？

				int loop = pMeshContainer->pSkinInfo->GetNumBones();		// ボーン数取得

				for (int i = 0; i < loop; i++) {
					m_OBBSkin[i].Draw(lpd3ddevice);
				}
			}
			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		//兄弟がいるなら再帰的にフレーム描画
		if (pFrame->pFrameSibling != NULL) {
			DrawBBox(lpd3ddevice, pFrame->pFrameSibling);
		}

		//子がいるなら再帰的にフレーム描画
		if (pFrame->pFrameFirstChild != NULL) {
			DrawBBox(lpd3ddevice, pFrame->pFrameFirstChild);
		}
	}

	//==================================================================================
	// BBOXを削除する
	//==================================================================================
	void CSkinnedMesh::ExitBBox(LPD3DXFRAME pFrameBase)
	{
		D3DXFRAME_DERIVED* pf = (D3DXFRAME_DERIVED*)pFrameBase;
		LPD3DXMESHCONTAINER pMeshContainer;

		pMeshContainer = pf->pMeshContainer;

		// メッシュコンテナがある限りループする
		while (pMeshContainer != NULL) {
			if (pMeshContainer->pSkinInfo != NULL) {							// ワンスキンフレームか？
				int loop = pMeshContainer->pSkinInfo->GetNumBones();		// ボーン数取得

				delete[] m_OBBSkin;
				delete[] m_matBone;

			}
			else {
				//if (pf->BBoxMesh != NULL) {
				//	ReleaseSafe(pf->BBoxMesh);								// メッシュを解放
				//}
			}
			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		//兄弟がいるなら再帰的にBBOX削除
		if (pf->pFrameSibling != NULL) {
			ExitBBox(pf->pFrameSibling);
		}

		//子がいるなら再帰的にBBOX削除
		if (pf->pFrameFirstChild != NULL) {
			ExitBBox(pf->pFrameFirstChild);
		}
	}

}
