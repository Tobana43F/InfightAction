#pragma once

#include	<d3dx9.h>
#include	"Graphic\Graphic_Def.h"
#include	"Shader\Shader_Def.h"

//!	@brief	安全な解放処理
#define ReleaseSafe(p) { if(p != nullptr) { (p)->Release(); (p)=nullptr; } }

namespace NDirectX {
	
	//!	@brief	正規化された値をRGBAに変換する
	D3DCOLOR	ToD3DColor(float _r, float _g, float _b, float _a);
	
	//!	@brief	DirectXの定義に変換
	void ToD3DMatrix(D3DXMATRIX& _dest, const NMath::SMatrix44 &_src);

	void ToD3DVector2(D3DXVECTOR2& _dest, const float2& _src);
	void ToD3DVector3(D3DXVECTOR3& _dest, const float3& _src);
	void ToD3DVector4(D3DXVECTOR4& _dest, const float4& _src);

	void ToD3DQuaternion(D3DXQUATERNION& _dest, const NMath::SQuaternion& _src);


	//!	@brief	独自定義フォーマットに変換
	void ToSMatrix44(NMath::SMatrix44 &_dest, const D3DXMATRIX& _src);

	void ToSQuaternion(NMath::SQuaternion& _dest, const D3DXQUATERNION& _src);



	//!	@brief	独自定義の値を基にレンダーステートを設定する
	void SetBlendState(NGraphic::EBlendState _blendState);


	//!	@brief	通常の合成を設定(透明無効)
	void SetBlendStateNormal(LPDIRECT3DDEVICE9 _pD3DDevice);

	//!	@brief	半透明合成
	void SetBlendStateAlpha(LPDIRECT3DDEVICE9 _pD3DDevice);

	//!	@brief	加算合成を設定
	void SetBlendStateAdd(LPDIRECT3DDEVICE9 _pD3DDevice);

	//!	@brief	減算合成を設定
	void SetBlendStateSub(LPDIRECT3DDEVICE9 _pD3DDevice);



	//!	@brief	レンダーステートの基本的な初期化
	void SetRenderStateDefault(LPDIRECT3DDEVICE9 _pD3DDevice);


	//!	@brief	独自定義の値を基にカリングを設定
	void SetCullMode(NShader::ECullMode _cullMode);

	//!	@brief	両面のカリングを設定
	void SetCullModeBoth();

	//!	@brief	表向きのカリングを設定
	void SetCullModeFront();

	//!	@brief	裏向きのカリングを設定
	void SetCullModeBack();



	//!	@brief	マテリアルデータを作成
	void MakeMaterialData(D3DMATERIAL9& _rMaterial, const float4& _color);
}
