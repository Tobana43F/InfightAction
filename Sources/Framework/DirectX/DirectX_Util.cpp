#include	"DirectX_Util.h"

#include	"DirectX\DirectX_Manager.h"
#include	"Graphic\Graphic_Util.h"

namespace NDirectX {

	D3DCOLOR	ToD3DColor(float _r, float _g, float _b, float _a)
	{
		uint r = NGraphic::To8Color(_r);
		uint g = NGraphic::To8Color(_g);
		uint b = NGraphic::To8Color(_b);
		uint a = NGraphic::To8Color(_a);

		return D3DCOLOR_RGBA(r, g, b, a);
	}

	void ToD3DMatrix(D3DXMATRIX& _dest, const NMath::SMatrix44 &_src)
	{
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				_dest.m[y][x] = _src.flt[y][x];
			}
		}
	}

	void ToD3DVector2(D3DXVECTOR2& _dest, const float2& _src)
	{
		_dest.x = _src.x;
		_dest.y = _src.y;
	}

	void ToD3DVector3(D3DXVECTOR3& _dest, const float3& _src)
	{
		_dest.x = _src.x;
		_dest.y = _src.y;
		_dest.z = _src.z;
	}

	void ToD3DVector4(D3DXVECTOR4& _dest, const float4& _src)
	{
		_dest.x = _src.x;
		_dest.y = _src.y;
		_dest.z = _src.z;
		_dest.w = _src.w;
	}

	void ToD3DQuaternion(D3DXQUATERNION& _dest, const NMath::SQuaternion& _src)
	{
		_dest.x = _src.x;
		_dest.y = _src.y;
		_dest.z = _src.z;
		_dest.w = _src.w;
	}



	void ToSMatrix44(NMath::SMatrix44 &_dest, const D3DXMATRIX& _src)
	{
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				_dest.flt[y][x] = _src.m[y][x];
			}
		}
	}

	void ToSQuaternion(NMath::SQuaternion& _dest, const D3DXQUATERNION& _src)
	{
		_dest.x = _src.x;
		_dest.y = _src.y;
		_dest.z = _src.z;
		_dest.w = _src.w;
	}


	void SetBlendState(NGraphic::EBlendState _blendState)
	{
		switch (_blendState) {
			case NGraphic::EBlendState::BLEND_STATE_REPLACE_NOALPHA:
				SetBlendStateNormal(NDirectX::GetD3DDevice());
				break;
			case NGraphic::EBlendState::BLEND_STATE_REPLACE:
				SetBlendStateAlpha(NDirectX::GetD3DDevice());
				break;
			case NGraphic::EBlendState::BLEND_STATE_ADD:
				SetBlendStateAdd(NDirectX::GetD3DDevice());
				break;
			case NGraphic::EBlendState::BLEND_STATE_SUB:
				SetBlendStateSub(NDirectX::GetD3DDevice());
				break;
		}
	}

	void SetBlendStateNormal(LPDIRECT3DDEVICE9 _pD3DDevice)
	{
		_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}

	void SetBlendStateAlpha(LPDIRECT3DDEVICE9 _pD3DDevice)
	{
		_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	}

	void SetBlendStateAdd(LPDIRECT3DDEVICE9 _pD3DDevice)
	{
		_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	}

	void SetBlendStateSub(LPDIRECT3DDEVICE9 _pD3DDevice)
	{
		_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
		_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

		_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	}

	void SetRenderStateDefault(LPDIRECT3DDEVICE9 _pD3DDevice)
	{
		// Ｚバッファ有効
		_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		// ライト有効
		_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		// カリング無効化
		_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		// 環境光セット
		_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

		// アルファブレンド設定
		NDirectX::SetBlendStateAlpha(_pD3DDevice);

		// テクスチャステージステート
		_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	}

	void SetCullMode(NShader::ECullMode _cullMode)
	{
		switch (_cullMode) {
			case NShader::ECullMode::BOTH:
				SetCullModeBoth();
				break;
			case NShader::ECullMode::FRONT:
				SetCullModeFront();
				break;
			case NShader::ECullMode::BACK:
				SetCullModeBack();
				break;
		}
	}

	void SetCullModeBoth()
	{
		NDirectX::GetD3DDevice()->SetRenderState(
			D3DRS_CULLMODE, 
			D3DCULL::D3DCULL_NONE);
	}

	void SetCullModeFront()
	{
		NDirectX::GetD3DDevice()->SetRenderState(
			D3DRS_CULLMODE,
			D3DCULL::D3DCULL_CCW);
	}

	void SetCullModeBack()
	{
		NDirectX::GetD3DDevice()->SetRenderState(
			D3DRS_CULLMODE, 
			D3DCULL::D3DCULL_CW);
	}

	void MakeMaterialData(D3DMATERIAL9& _rMaterial, const float4& _color)
	{
		_rMaterial.Diffuse.r = _color.x;
		_rMaterial.Diffuse.g = _color.y;
		_rMaterial.Diffuse.b = _color.z;
		_rMaterial.Diffuse.a = _color.w;

		_rMaterial.Ambient.r = _color.x;
		_rMaterial.Ambient.g = _color.y;
		_rMaterial.Ambient.b = _color.z;
		_rMaterial.Ambient.a = 1.0f;

		_rMaterial.Specular.r = _color.x;
		_rMaterial.Specular.g = _color.y;
		_rMaterial.Specular.b = _color.z;
		_rMaterial.Specular.a = 1.0f;

		_rMaterial.Emissive.r = 0.0f;
		_rMaterial.Emissive.g = 0.0f;
		_rMaterial.Emissive.b = 0.0f;
		_rMaterial.Emissive.a = 0.0f;
	}
}
