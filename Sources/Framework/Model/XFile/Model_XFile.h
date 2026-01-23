#pragma once

#include	<d3d9.h>
#include	<d3dx9.h>

#include	"DirectX\DirectX_Manager.h"
#include	"Math\Math_Type.h"

namespace NModel {

	//!	@brief	Xファイルのモデルを管理するクラス
	class CXFile {
	public:

		//!	@brief	Xファイル読み込み
		//!	@param	Xファイル名
		//!	@param	デバイスオブジェクト
		//!	@retval	TRUE　成功終了/FALSE　失敗終了
		bool LoadXFile(char* xfilename, LPDIRECT3DDEVICE9 lpd3ddevice);

		void UnLoadXFile();

		void Render(NMath::SMatrix44& _rMatWorld)
		{
			NDirectX::GetD3DDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&_rMatWorld);
			Draw(NDirectX::GetD3DDevice());
		}

		void Draw(LPDIRECT3DDEVICE9 lpd3ddevice);
		
		void DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice);
		
		void DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice);



		LPD3DXMESH			GetMesh() const;
		
		DWORD				GetNumMaterial() const;
		
		D3DMATERIAL9*		GetPMaterial(int index) const;
		
		LPDIRECT3DTEXTURE9*	GetPTexture(int index) const;

	private:
		LPD3DXMESH					m_lpmesh;				// メッシュインターフェイス
		D3DMATERIAL9				*m_lpmeshmaterials;		// モデルデータのマテリアル
		LPDIRECT3DTEXTURE9			*m_lpmeshtextures;		// モデルデータのテクスチャ
		DWORD						m_nummaterial;			// マテリアル数

	public:
		CXFile();
		virtual ~CXFile();
	};

}