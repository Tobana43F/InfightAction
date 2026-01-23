#include	"Shader_Manager.h"

#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"
#include	"Shader_Util.h"

namespace NShader {

	void CManager::SetCullMode(const ECullMode _cullMode)
	{
		D3DCULL	cullMode;
		switch (_cullMode) {
			case ECullMode::FRONT:
				cullMode = D3DCULL_CCW;
				break;
			case ECullMode::BACK:	
				cullMode = D3DCULL_CW;
				break;
			case ECullMode::BOTH:
				cullMode = D3DCULL_NONE;
				break;
		}
		NDirectX::GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	}

	void CManager::SetVertexFormat(const DWORD _fvf)
	{
		NDirectX::GetD3DDevice()->SetFVF(_fvf);
	}

	void CManager::SetTexture(const uint _index, const LPDIRECT3DTEXTURE9 _pTexture)
	{
		NDirectX::GetD3DDevice()->SetTexture(_index, _pTexture);
	}

	//----------------------------------------------------------

	CManager::CManager() : 
		m_pVS(nullptr),
		m_pVSCT(nullptr),
		m_pPS(nullptr),
		m_pPSCT(nullptr)
	{

	}

	CManager::~CManager()
	{
		ReleaseSafe(m_pVS);
		ReleaseSafe(m_pVSCT);
		ReleaseSafe(m_pPS);
		ReleaseSafe(m_pPSCT);
	}

	bool CManager::Load(
		const char* _pVSFilePath,
		const char*	_pVSEntryName,
		const char*	_pPSFilePath,
		const char* _pPSEntryName)
	{
		bool sts = false;

		// 頂点シェーダーコンパイル
		sts = CompileVertexShader(
			NDirectX::GetD3DDevice(),		// デバイスオブジェクト
			_pVSFilePath,					// シェーダーファイル名
			_pVSEntryName,					// エントリー関数名
			"vs_3_0",						// バージョン
			&m_pVSCT,						// 定数テーブル
			&m_pVS);						// 頂点シェーダーオブジェクト

		// ピクセルシェーダーコンパイル
		sts = CompilePixelShader(
			NDirectX::GetD3DDevice(),		// デバイスオブジェクト
			_pPSFilePath,					// シェーダーファイル名
			_pPSEntryName,					// エントリー関数名
			"ps_3_0",						// バージョン
			&m_pPSCT,						// 定数テーブル
			&m_pPS);						// ピクセルシェーダーオブジェクト

		return sts;
	}

	void CManager::SetShaderToPipeline()
	{
		NDirectX::GetD3DDevice()->SetVertexShader(m_pVS);
		NDirectX::GetD3DDevice()->SetPixelShader(m_pPS);
	}

	void CManager::UnsetShaderFromPipeline()
	{
		NDirectX::GetD3DDevice()->SetVertexShader(nullptr);
		NDirectX::GetD3DDevice()->SetPixelShader(nullptr);
	}
}