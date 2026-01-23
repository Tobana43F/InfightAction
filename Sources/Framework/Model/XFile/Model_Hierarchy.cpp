#include	"Model_Hierarchy.h"

#include	"DirectX\DirectX_Util.h"

namespace NModel {

	CHierarchy::CHierarchy(void)
	{}

	CHierarchy::~CHierarchy(void)
	{}

	//=====================================================================
	// マルチバイト文字をワイドキャラに変換する
	//====================================================================
	void MtoW(WCHAR* lpuni, LPSTR lpsjis, int buffersize)
	{
		int len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsjis, -1, NULL, 0);

		if (buffersize) {
			//必要な文字数がバッファサイズより大きい場合はバッファサイズ分を返還
			if (len > buffersize - 1)
				len = buffersize - 1;
		}

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsjis, -1, lpuni, len);
	}

	//=====================================================================
	//割り当て名:フレームかメッシュ
	//====================================================================
	HRESULT AllocateName(LPCSTR Name, LPSTR* pNewName)
	{
		UINT cbLength;

		if (Name != NULL) {
			cbLength = (UINT)strlen(Name) + 1;
			*pNewName = new CHAR[cbLength];
			if (*pNewName == NULL)
				return E_OUTOFMEMORY;
			memcpy(*pNewName, Name, sizeof(CHAR) * cbLength);
		}
		else {
			*pNewName = NULL;
		}
		return  S_OK;
	}

	//====================================================================
	//フレーム作成
	//====================================================================
	HRESULT CHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
	{
		HRESULT hr = S_OK;
		D3DXFRAME_DERIVED* pFrame;//フレーム

		*ppNewFrame = NULL;

		pFrame = New D3DXFRAME_DERIVED;
		if (pFrame == NULL) {
			return E_OUTOFMEMORY;
		}

		// フレーム名格納用のメモリを確保しフレーム名をセットする
		hr = AllocateName(Name, &pFrame->Name);
		if (FAILED(hr))
			return E_FAIL;

		//フレーム部品を初期化
		D3DXMatrixIdentity(&pFrame->TransformationMatrix);
		D3DXMatrixIdentity(&pFrame->CombinedTransformMatrix);

		pFrame->pMeshContainer = NULL;			// メッシュコンテナなしをセット
		pFrame->pFrameSibling = NULL;			// 兄弟フレームなしをセット
		pFrame->pFrameFirstChild = NULL;		// 子供フレームなしをセット

		*ppNewFrame = pFrame;
		pFrame = NULL;

		return S_OK;
	}

	//====================================================================
	//メッシュコンテナの作成
	//====================================================================
	HRESULT CHierarchy::CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA *pMeshData, const D3DXMATERIAL *pMaterials, const D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer)
	{
		HRESULT hr;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = NULL;
		UINT NumFaces;//面の数
		UINT iMaterial;//マテリアル数
		UINT iBone, cBone;//ボーン数
		LPDIRECT3DDEVICE9 pd3ddevice = NULL;

		LPD3DXMESH pMesh = NULL;

		*ppNewMeshContainer = NULL;

		//扱えるメッシュかどうか判定
		if (pMeshData->Type != D3DXMESHTYPE_MESH)
			return E_FAIL;

		//メッシュポインタ取得
		pMesh = pMeshData->pMesh;

		//FVFコンパチブルメッシュではないか判定
		if (pMesh->GetFVF() == 0)
			return E_FAIL;

		//オーバーロード状態の構造体を設定
		pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
		if (pMeshContainer == NULL)
			return E_OUTOFMEMORY;
		memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

		//名前を設定
		hr = AllocateName(Name, &pMeshContainer->Name);
		if (FAILED(hr))
			return E_FAIL;

		pMesh->GetDevice(&pd3ddevice);			// デバイスを取得する
		NumFaces = pMesh->GetNumFaces();		// 面数を取得する

		//法線情報を設定
		if (!(pMesh->GetFVF() & D3DFVF_NORMAL)) {
			pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

			//メッシュのクローンを作って法線の場所を空ける
			hr = pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL, pd3ddevice, &pMeshContainer->MeshData.pMesh);
			if (FAILED(hr))
				return E_FAIL;

			//使用するメッシュコンテナからメッシュポインタを取り出す
			pMesh = pMeshContainer->MeshData.pMesh;

			//メッシュポインタに法線を作る
			D3DXComputeNormals(pMesh, NULL);
		}
		else {
			//メッシュコンテナの参照をセット
			pMeshContainer->MeshData.pMesh = pMesh;
			pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

			pMesh->AddRef();
		}

		//メモリ割り当てマテリアル情報をセット
		pMeshContainer->NumMaterials = max(1, NumMaterials);
		pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
		pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
		pMeshContainer->pAdjacency = new DWORD[NumFaces * 3];
		if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL) || (pMeshContainer->ppTextures == NULL))
			return E_OUTOFMEMORY;

		memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces * 3);
		memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

		//マテリアルが設定されていたらコピーする
		if (NumMaterials > 0) {
			memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

			for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) {
				if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL) {
					WCHAR str[MAX_PATH];

					std::string	path = "";
					path += m_fileDir.data();
					path += pMeshContainer->pMaterials[iMaterial].pTextureFilename;

					MtoW(str, const_cast<char*>(path.c_str()), MAX_PATH);
					if (FAILED(D3DXCreateTextureFromFileW(pd3ddevice, str, &pMeshContainer->ppTextures[iMaterial])))
						pMeshContainer->ppTextures[iMaterial] = NULL;

					//ポインタをダイナミックメモリに確保しないようし、NULLを設定
					pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
				}
			}
		}
		else	//マテリアルが無かった時、デフォルトの素材を設定
		{
			pMeshContainer->pMaterials[0].pTextureFilename = NULL;
			memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
			pMeshContainer->pMaterials[0].MatD3D.Diffuse.r =
				pMeshContainer->pMaterials[0].MatD3D.Diffuse.g =
				pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
			pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
		}

		//スキン情報があれば、それを使う
		if (pSkinInfo != NULL) {
			pMeshContainer->pSkinInfo = pSkinInfo;
			pSkinInfo->AddRef();

			pMeshContainer->pOrigMesh = pMesh;
			pMesh->AddRef();

			//オフセットマトリックス配列を生成
			cBone = pSkinInfo->GetNumBones();
			pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBone];
			if (pMeshContainer->pBoneOffsetMatrices == NULL)
				return E_OUTOFMEMORY;

			//ボーンマトリクス取得
			for (iBone = 0; iBone < cBone; iBone++) {
				pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
			}
			//スキンメッシュ生成
			hr = GenerateSkinnedMesh(pd3ddevice, pMeshContainer);
			if (FAILED(hr))
				return E_FAIL;
		}

		*ppNewMeshContainer = pMeshContainer;
		pMeshContainer = NULL;

		ReleaseSafe(pd3ddevice);

		//適当に割り当てられたデータをきれいにする
		if (pMeshContainer != NULL) {
			DestroyMeshContainer(pMeshContainer);
		}

		return S_OK;
	}

	//==================================================================
	//フレーム削除
	//==================================================================
	HRESULT CHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
	{
		DeleteArraySafe(pFrameToFree->Name);
		DeleteSafe(pFrameToFree);
		return S_OK;
	}

	//================================================================
	//メッシュコンテナ削除
	//===============================================================
	HRESULT CHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
	{
		UINT iMaterial;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

		DeleteArraySafe(pMeshContainer->Name);
		DeleteArraySafe(pMeshContainer->pAdjacency);
		DeleteArraySafe(pMeshContainer->pMaterials);
		DeleteArraySafe(pMeshContainer->pBoneOffsetMatrices);

		//割り当てられたテクスチャの解放
		if (pMeshContainer->ppTextures != NULL) {
			for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++) {
				ReleaseSafe(pMeshContainer->ppTextures[iMaterial]);
			}
		}

		DeleteArraySafe(pMeshContainer->ppTextures);
		DeleteArraySafe(pMeshContainer->ppBoneMatrixPtrs);
		ReleaseSafe(pMeshContainer->pBoneCombinationBuf);
		ReleaseSafe(pMeshContainer->MeshData.pMesh);
		ReleaseSafe(pMeshContainer->pSkinInfo);
		ReleaseSafe(pMeshContainer->pOrigMesh);
		DeleteSafe(pMeshContainer);

		return S_OK;
	}


	//=========================================================================
	//スキンメッシュ生成
	//========================================================================

	HRESULT GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer)
	{
		HRESULT hr = S_OK;
		D3DCAPS9 d3dCaps;
		pd3dDevice->GetDeviceCaps(&d3dCaps);

		if (pMeshContainer->pSkinInfo == NULL)
			return hr;

		ReleaseSafe(pMeshContainer->MeshData.pMesh);
		ReleaseSafe(pMeshContainer->pBoneCombinationBuf);

		hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh(
			pMeshContainer->pOrigMesh,
			D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh);

		if (FAILED(hr))
			return E_FAIL;

		//	DWORD* buf = (DWORD*)pbf->GetBufferPointer();
		//	pMeshContainer->pSkinInfo->Remap(pMeshContainer->MeshData.pMesh->GetNumVertices(),buf);
		//	pbf->Release();

		LPD3DXBONECOMBINATION rgBoneCombinations = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; pMeshContainer->iAttributeSW++) {
			DWORD cInfl = 0;

			for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++) {
				if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
					++cInfl;
			}

			if (cInfl > d3dCaps.MaxVertexBlendMatrices)
				break;
		}

		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups) {
			LPD3DXMESH pMeshTemp;

			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(
				D3DXMESH_SOFTWAREPROCESSING | pMeshContainer->MeshData.pMesh->GetOptions(),
				pMeshContainer->MeshData.pMesh->GetFVF(),
				pd3dDevice, &pMeshTemp);
			if (FAILED(hr))
				return E_FAIL;

			pMeshContainer->MeshData.pMesh->Release();
			pMeshContainer->MeshData.pMesh = pMeshTemp;
			pMeshTemp = NULL;
		}

		return S_OK;
	}
}
