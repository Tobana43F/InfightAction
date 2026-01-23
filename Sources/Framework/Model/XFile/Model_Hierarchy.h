//========================================================

//========================================================
#pragma once
#include	<d3d9.h>
#include	<d3dx9.h>

#include	<string>

namespace NModel {

	//フレーム構造体
	struct D3DXFRAME_DERIVED : public D3DXFRAME {
		D3DXMATRIXA16	CombinedTransformMatrix;
	};

	//メッシュコンテナ構造体
	struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER {
		LPDIRECT3DTEXTURE9* ppTextures;	//テクスチャ配列

		//スキンメッシュ情報
		LPD3DXMESH				pOrigMesh;	//メッシュ
		LPD3DXATTRIBUTERANGE	pAttributeTable;	//属性
		DWORD					NumAttributeGroups;	//属性グループ
		DWORD					NumInfl;
		LPD3DXBUFFER			pBoneCombinationBuf;	//ボーンの組み合わせバッファ
		D3DXMATRIX**			ppBoneMatrixPtrs;		//ボーン行列の配列
		D3DXMATRIX*				pBoneOffsetMatrices;	//ボーンオフセット行列
		DWORD					NumPaletteEntries;		//パレットエントリー
		bool					UserSoftwareVP;			//ソフトウェアVP使用フラグ
		DWORD					iAttributeSW;
	};

	//!	@brief	スキンメッシュの階層構造を解析するためのクラス
	class CHierarchy : public ID3DXAllocateHierarchy {
	private:

		// xファイルのディレクトリ
		std::string	m_fileDir;

	public:
		void SetFileDir(const char *_fileDir)
		{
			m_fileDir = _fileDir;
		}

		CHierarchy(void);
		~CHierarchy(void);
		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);	//フレーム作成
		STDMETHOD(CreateMeshContainer)(THIS_			//メッシュコンテナの作成
			LPCSTR Name,								//名前
			CONST D3DXMESHDATA *pMeshData,				//メッシュデータ
			CONST D3DXMATERIAL *pMaterials,				//マテリアル
			CONST D3DXEFFECTINSTANCE *pEffectInstances,	//エフェクトの実態
			DWORD NumMaterials,							//マテリアル数
			CONST DWORD* pAdjacency,					//隣接関係
			LPD3DXSKININFO pSkinInfo,					//スキン情報
			LPD3DXMESHCONTAINER* ppNewMeshContainer);	//メッシュコンテナのポインタ
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);	//フレーム削除
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);	//メッシュコンテナの削除
	};

	HRESULT GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer);

}