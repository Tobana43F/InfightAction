#pragma once

#include	<array>
#include	<vector>
#include	"Math\Math_Type.h"
#include	"DirectX\DirectX_Def.h"
#include	"DirectX\DirectX_Util.h"

namespace NModel {

	//!	@brief	モデルの頂点情報
	struct SVertex {
		NMath::SFloat3Data		pos;
		NMath::SFloat3Data		normal;
		NMath::SFloat2Data		texCoord;
	};

	//!	@brief	マテリアル情報
	struct SConstMaterial {
		float4		ambient;
		float4		diffuse;
		float4		specular;	
		float4		emmisive;
	};

	//!	@brief	マテリアル管理構造体
	struct SMaterial {
		float4			ambient;
		float4			diffuse;
		float4			specular;
		float4			emmisive;
		float			specularPower;
		float			transparencyFactor;

		SConstMaterial	constantMaterial;

		LPDIRECT3DTEXTURE9	texture;	// 参照するだけ


		//!	@brief	テクスチャを開放する
		void Release()
		{
			// 参照するだけなので解放処理を行わない
			texture = nullptr;
		}

		SMaterial() :
			texture(nullptr)
		{ }
	};

	//!	@brief	メッシュノード
	struct SMeshNode {
		// バッファに使用する型
		using VertexType	= NModel::SVertex;
		using IndexType		= uint;

		// 初期状態のメッシュを保持するバッファ
		std::vector<VertexType>	initVertices;

		// 現在のメッシュを保持するバッファ
		IDirect3DIndexBuffer9*	pIB;
		IDirect3DVertexBuffer9*	pVB;

		// メッシュ情報
		uint				indexCount;
		uint				vertexCount;

		// ウェイト情報
		std::vector<std::vector<float>>	weights;	// [頂点][クラスター] = weight

		// マテリアル
		SMaterial			material;

		// ノードの変換行列
		NMath::SMatrix44	matrix;

		// デバッグ情報
		std::string			nodeName;

		//!	@brief	バッファを解放する
		void Release()
		{
			material.Release();
			ReleaseSafe(pIB);
			ReleaseSafe(pVB);
		}

		SMeshNode() :
			pIB(nullptr),
			pVB(nullptr),
			indexCount(0),
			vertexCount(0)
		{ }
	};
}
