#include	"Mesh_Line.h"

#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"

namespace {

	float4		g_lineColor(1, 1, 1, 1);

}

namespace NMesh {

	float4	GetLineColor()
	{
		return g_lineColor;
	}

	void SetLineColor(const float4& _col)
	{
		g_lineColor = _col;
	}

	void RenderLine(
		const float3& _posStart, 
		const float3& _posEnd)
	{
		struct VERTEXDATA {
			float		x, y, z;
			D3DCOLOR	color;
		};

		// ３軸の頂点データ
		D3DCOLOR col = NDirectX::ToD3DColor(g_lineColor.x, g_lineColor.y, g_lineColor.z, g_lineColor.w);

		struct	VERTEXDATA	linevertices[] = {
			_posStart.x, _posStart.y, _posStart.z, col,
			_posEnd.x, _posEnd.y, _posEnd.z, col,		
		};

		NMath::SMatrix44 matIdentity;
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&matIdentity);
		NDirectX::GetD3DDevice()->SetTexture(0, nullptr);

		NDirectX::GetD3DDevice()->LightEnable(0, false);
		//NDirectX::GetD3DDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
		NDirectX::GetD3DDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
		NDirectX::GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, false);	//   光源計算オフ（光源計算を切るとDIFFUSE色がそのままでる）

		NDirectX::GetD3DDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);	// 頂点フォーマットをセット
		NDirectX::GetD3DDevice()->DrawPrimitiveUP(D3DPT_LINELIST, 1, &linevertices[0], sizeof(VERTEXDATA));	// ３軸を描画

		NDirectX::GetD3DDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
		NDirectX::GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, true);	//   光源計算ON
		NDirectX::GetD3DDevice()->LightEnable(0, true);
	}

}
