#include	"Model_FbxDef.h"

#include	"System\System_Error.h"

namespace NModel {

	//----------------------------------------------------------
	// SFbxMaterialElement
	//----------------------------------------------------------

	SFbxMaterialElement::SFbxMaterialElement() :
		type(NONE),
		r(0),
		g(0),
		b(0),
		a(1)
	{
		textureSets.clear();
		textureSetObjs.clear();
	}

	SFbxMaterialElement::~SFbxMaterialElement()
	{
		//!	@note	テクスチャの解放を含むので明示的に、Releaseを呼び出すほうが安全
		//Release();
	}

	void SFbxMaterialElement::Load()
	{
		auto itrTexSet = textureSets.begin();
		auto itrTexObj = textureSetObjs.begin();

		//	全てのテクスチャを読み込む
		while (itrTexSet != textureSets.end()) {
			
			// 全てのレイヤードテクスチャを読み込む
			uint layerdTexCount = itrTexSet->second.size();
			for (uint iLayer = 0; iLayer < layerdTexCount; ++iLayer) {
				auto&	path = itrTexSet->second[iLayer];
				auto&	texObj = itrTexObj->second[iLayer];

				HRESULT hr;
				hr = D3DXCreateTextureFromFile(
					NDirectX::GetD3DDevice(),
					path.data(),
					&texObj);

				if (FAILED(hr)) {
					LPVOID string;
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						hr,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPTSTR)&string,
						0,
						NULL);
					if (string != NULL)
						OutputDebugString((LPCSTR)string);

					std::string	str = "";
					str += path;
					str += "\nの読み込みに失敗しました。";
					str += "\n\n";
					str += "HRESULT Message : ";
					str += (LPCSTR)string;
					LocalFree(string);
					ErrorEx(str.data());
				}
			}

			// イテレータを進める
			itrTexSet++;
			itrTexObj++;
		}
	}

	void SFbxMaterialElement::Release()
	{
		// テクスチャセットの解放
		for (auto itr = textureSets.begin(); itr != textureSets.end(); ++itr) {
			itr->second.clear();
		}
		textureSets.clear();

		// 読み込んだテクスチャの解放
		for (auto itr = textureSetObjs.begin(); itr != textureSetObjs.end(); ++itr) {
			for (uint i = 0; i < itr->second.size(); ++i) {
				ReleaseSafe(itr->second[i]);
			}
			itr->second.clear();
		}
		textureSetObjs.clear();
	}

	//----------------------------------------------------------
	// SFbxMaterialNode
	//----------------------------------------------------------
	void SFbxMaterialNode::Load()
	{
		ambient.Load();
		diffuse.Load();
		emmisive.Load();
		specular.Load();
	}

	void SFbxMaterialNode::Release()
	{
		ambient.Release();
		diffuse.Release();
		emmisive.Release();
		specular.Release();
	}

}