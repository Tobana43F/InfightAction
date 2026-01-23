#pragma once

#include	<algorithm>
#include	<fbxsdk.h>
#include	<string>
#include	<unordered_map>
#include	<vector>
#include	"Math\Math_Type.h"

// DirectX依存
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"

namespace NModel {

	//!	@brief	座標系
	enum class EFbxAxisSystem {
		DIRECTX,	// 左手
		OPENGL,		// 右手
		MAYA_Y_UP	// Blenderの軸に合わせられる？
	};

	//!	@brief	マテリアル要素構造体
	//!	@note	テクスチャの情報も保持する
	struct SFbxMaterialElement {
		//!	@brief	頂点内のUVセット順序
		using UVSetID		= std::unordered_map<std::string, int>;

		//!	@brief	UVセット名, テクスチャパス名の対応(UVセット(1) テクスチャパス名(多))
		using TextureSet	= std::unordered_map<std::string, std::vector<std::string>>;

		//!	@brief	管理するテクスチャの型(今後の変更に対応するためにエイリアスを作成)
		using TextureObj = LPDIRECT3DTEXTURE9;
		//!	@brief	TextureSetに対応するテクスチャオブジェクトを管理する型
		using TextureSetObj = std::unordered_map<std::string, std::vector<TextureObj>>;

		//!	@brief	保持する情報のタイプ
		enum EType {
			NONE,		// 無し
			COLOR,		// 色情報のみ
			TEXTURE,	// テクスチャ情報のみ
			BOTH,		// 色とテクスチャ
			MAX
		};

		//	保持するデータ
		EType			type;
		float			r, g, b, a;
		TextureSet		textureSets;
		TextureSetObj	textureSetObjs;

		//!	@brief	テクスチャセットをクリアするコンストラクタ
		SFbxMaterialElement();

		//!	@brief	読み込んだテクスチャセットを開放するデストラクタ
		~SFbxMaterialElement();

		//!	@brief	テクスチャの読み込み処理
		void Load();

		//!	@brief	解放処理
		void Release();
	};

	//!	@brief	マテリアルノード構造体
	struct SFbxMaterialNode {
		//!	@brief	マテリアルのタイプ
		enum EType {
			LAMBERT,
			PHONG
		};

		EType				type;
		SFbxMaterialElement	ambient;
		SFbxMaterialElement	diffuse;
		SFbxMaterialElement	emmisive;
		SFbxMaterialElement	specular;

		float				shininess;
		float				transparencyFactor;	// 透明度

		//!	@brief	それぞれのマテリアルのテクスチャの読み込み処理を行う
		void Load();
		
		//!	@brief	それぞれのマテリアルのテクスチャの解放処理
		void Release();
	};

	//!	@brief	メッシュノード内の構成要素の数
	struct SFbxMeshElementInfo {
		unsigned int	posCount;
		unsigned int	normalCount;
		unsigned int	uvSetCount;

		//!	@brief	初期化用コンストラクタ
		SFbxMeshElementInfo() :
			posCount(0),
			normalCount(0),
			uvSetCount(0)
		{ }
	};

	//!	@brief	メッシュで使用する1頂点構造体
	struct SFbxVertex {
		FbxVector4			pos;
		FbxVector4			normal;
		FbxVector2			texCoord;
		std::vector<float>	weights;	// size = クラスターの数となる
		int					rawIndex;	// FBX内のインデックスの値(アニメーション時に使用する)

		//!	@brief	等号演算子
		//!	@note	メッシュ最適化の際に、同じ頂点を探すために使用する
		bool operator == (const SFbxVertex& _rhs) const
		{
			bool sts = true;
			sts &= (pos == _rhs.pos);
			sts &= (normal == _rhs.normal);
			sts &= (texCoord == _rhs.texCoord);
			sts &= (rawIndex == _rhs.rawIndex);
			sts &= (weights.size() == _rhs.weights.size());

			if (sts) {
				for (uint i = 0; i < weights.size(); ++i) {
					sts &= (weights[i] == _rhs.weights[i]);
				}
			}
			return sts;
		}
	};

	//!	@brief	メッシュノード構造体
	struct SFbxMeshNode {
		std::string						nodeName;		// 自ノード名	
		std::string						parentNodeName;	// 親ノード名
			
		SFbxMeshElementInfo				elementInfo;	// メッシュが保持するデータ情報
		std::vector<SFbxMaterialNode>	materials;		// マテリアル
		SFbxMaterialElement::UVSetID	uvSetID;

		// 頂点情報(値は最適化前のもの)
		// ※ 使用しないでください
		unsigned int					indexCount;		
		unsigned int					vertexCount;
		unsigned int					polygonCount;
		unsigned int					clusterCount;	// クラスター(ボーン)の数 
														// ※ 同一モデル内のすべてのMeshNodeで同じ値になる

		// 最適化しない状態
		std::vector<unsigned int>		indexes;		// インデックス配列
		std::vector<int>				rawIndexes;		// FBX内の頂点インデックス
		std::vector<FbxVector4>			positions;		// ポジション配列
		std::vector<FbxVector4>			normals;		// 法線配列	
		std::vector<FbxVector2>			texcoords;		// テクスチャ座標配列
		std::vector<std::vector<float>>	weights;		// ウェイト値（[頂点インデックス][クラスター] = weight）

		// テスト　最適化した状態
		std::vector<unsigned int>		optimizedIndex;
		std::vector<SFbxVertex>			optimizedVertex;

		// ノードの変換行列
		NMath::SMatrix44				matrix;

		FbxMesh*						pFbxMesh;		// メッシュへの参照を保持する
		FbxSkin*						pFbxSkin;		// スキンへの参照を保持する

		//!	@brief	初期化用コンストラクタ
		SFbxMeshNode() :
			indexCount(0),
			vertexCount(0),
			polygonCount(0),
			clusterCount(0),
			pFbxMesh(nullptr),
			pFbxSkin(nullptr)
		{

		}

		//!	@brief	読み込んだ情報を開放するデストラクタ
		~SFbxMeshNode() 
		{

		}

		//!	@brief	コンテナのクリア
		void Release()
		{
			uvSetID.clear();
			texcoords.clear();
			for (unsigned int i = 0; i < materials.size(); ++i) {
				materials[i].Release();
			}

			indexes.clear();
			positions.clear();
			normals.clear();

			weights.clear();

			pFbxMesh = nullptr;
			pFbxSkin = nullptr;
		}
	};

	//!	@brief	アニメーションスタック情報を保持する構造体
	struct SFbxAnimationStack {
		std::string		anmName;	// アニメーション名

		unsigned int	startFrame;	// アニメーションの開始フレーム
		unsigned int	stopFrame;	// アニメーションの終了フレーム

		FbxLongLong		startTime;	// アニメーションの開始時間(Fbxの時間単位)
		FbxLongLong		stopTime;	// アニメーションの終了時間(Fbxの時間単位)

		SFbxAnimationStack() :
			startFrame(0),
			stopFrame(0),
			startTime(0),
			stopTime(0)
		{ }
	};
	
}
