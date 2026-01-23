#include	"Polygon_Object3D.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"DirectX\DirectX_Util.h"
#include	"Math\Math_Common.h"


#include	"Debug\Debug_Util.h"


namespace NPolygon {

	CObject3D::CObject3D()
	{
	}

	CObject3D::~CObject3D(){

	}

	//----------------------------------------------------------

	void CObject3D::RenderPolygon(const SData& _data)
	{
		//----------------------------------------------------------
		// 頂点データを作成
		//----------------------------------------------------------

		int2	texNo(0, 0);		// テクスチャ番号のテクスチャ座標
		int2	texReverseOffset(0, 0);	// 反転時のオフセット
		float2	texDivLength(1, 1);		// 分割後の1辺の長さ

		GetTextureNo(texNo, _data);
		GetReverseOffset(texReverseOffset, _data);
		GetDivLength(texDivLength, _data);

		float x[EVertexID::MAX] = {		// 座標は平行移動行列を適用して行う
			0 + _data.size.x * 0.5f,
			0 - _data.size.x * 0.5f,
			0 + _data.size.x * 0.5f,
			0 - _data.size.x * 0.5f };
		float y[EVertexID::MAX] = {
			0 + _data.size.y * 0.5f,
			0 + _data.size.y * 0.5f,
			0 - _data.size.y * 0.5f,
			0 - _data.size.y * 0.5f };
		float z[EVertexID::MAX] = {
			0,
			0,
			0,
			0 };
		float tu[EVertexID::MAX] = {
			texDivLength.x * (texNo.x + (texReverseOffset.x)),
			texDivLength.x * (texNo.x + (texReverseOffset.x ^ 0x1)),
			texDivLength.x * (texNo.x + (texReverseOffset.x)),
			texDivLength.x * (texNo.x + (texReverseOffset.x ^ 0x1)) };
		float tv[EVertexID::MAX] = {
			texDivLength.y * (texNo.y + (texReverseOffset.y)),
			texDivLength.y * (texNo.y + (texReverseOffset.y)),
			texDivLength.y * (texNo.y + (texReverseOffset.y ^ 0x1)),
			texDivLength.y * (texNo.y + (texReverseOffset.y ^ 0x1)) };

		// アライメントを適用
		int vertexCount = StaticCast<int>(EVertexID::MAX);
		for (int i = 0; i < vertexCount; ++i) {
			ApplyAlign3D(x[i], y[i], _data);
		}

		//----------------------------------------------------------
		// パイプラインに流す頂点を作成
		//----------------------------------------------------------

		SVertex3D	vertex[EVertexID::MAX];

		D3DXMATRIX	matDX;
		NDirectX::ToD3DMatrix(matDX, _data.matWorld);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_WORLD, &matDX);	// ワールド変換行列を適用

		// 固定パイプライン引用の型を作成専用の型に代入
		for (int i = 0; i < EVertexID::MAX; ++i) {
			vertex[i].pos.x = x[i];
			vertex[i].pos.y = y[i];
			vertex[i].pos.z = z[i];

			EVertexID	idxVertex = (EVertexID)i;
			vertex[i].diffuse = NDirectX::ToD3DColor(
				_data.colors[idxVertex].x,
				_data.colors[idxVertex].y,
				_data.colors[idxVertex].z,
				_data.colors[idxVertex].w);	// 色の型をDirectX用に変換する;

											// 仮
			vertex[i].texCoord.x = tu[i];
			vertex[i].texCoord.y = tv[i];
		}

		// 描画
		NDirectX::GetD3DDevice()->DrawPrimitiveUP(
			D3DPT_TRIANGLESTRIP,
			2,
			vertex,
			sizeof(SVertex3D));		// ポリゴンを描画

	}
}
