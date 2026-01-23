#pragma once

#include	<vector>
#include	<fbxsdk.h>

#include	"Model_FbxDef.h"

namespace NModel {

	//!	@brief	FBXの読み込みを行うクラス
	//!	@todo	ボーンノードの情報も必要
	class CFbxLoader {
	public:

		//!	@brief	FBXファイルの読み込み
		bool Load(
			const char*		_filePath,
			EFbxAxisSystem	_axis = EFbxAxisSystem::OPENGL);

		//!	@brief	FBXのノードを取得
		FbxNode&		GetFbxRootNode() { return *m_pFbxScene->GetRootNode(); }

		//!	@brief	メッシュノードの数を取得
		size_t			GetMeshNodeCount() const { return m_meshNodes.size(); }

		//!	@brief	メッシュノードを取得
		SFbxMeshNode&		GetMeshNode(const unsigned int _idx) { return m_meshNodes[_idx]; }
		const SFbxMeshNode&	GetMeshNode(const unsigned int _idx) const { return m_meshNodes[_idx]; }

		//!	@brief	アニメーション数を取得する
		unsigned int	GetAnimationCount() const { return m_anmStack.size(); }

		//!	@brief	アニメーション情報を取得する
		const SFbxAnimationStack&	GetAnmStack(const unsigned int _idx) const { return m_anmStack[_idx]; }

		//!	@brief	1フレームFBX時間を取得する
		FbxLongLong		GetFbxTimePerFrame() const { return m_timePerFrame; }
		
		//!	@brief	使用するアニメーションスタックをセットする
		//!	@note	モデルのアニメーション情報を取得するときは必ず呼び出す
		//!			- クラスターから行列を取得するときは必ず呼び出す。
		void			SetAnmStack(const unsigned int _idxAnmStack);

	private:
		//	Fbxオブジェクト
		FbxManager*		m_pFbxManager;
		FbxScene*		m_pFbxScene;
		FbxImporter*	m_pFbxImporter;

		// モデル情報
		std::vector<SFbxMeshNode>		m_meshNodes;

		// アニメーション情報
		std::vector<SFbxAnimationStack>	m_anmStack;
		FbxLongLong						m_timePerFrame;	// 1フレーム当たりのFbxの時間単位

		// 制御用情報
		std::string						m_filePath;		// 読み込んだファイルのパスを保持
		std::string						m_fileDirPath;	// 読み込んだファイルが存在するディレクトリのパス

		//----------------------------------------------------------
		// それぞれの読み込みを行う関数群
		//----------------------------------------------------------

		//!	@brief	ノードを読み込む
		void SetupNode();

		//!	@brief		ノードを再帰的に読み込む
		//!	@details	rootノードの親にはnullを設定
		void LoadNodeRecursively(
			FbxNode*			_pNode, 
			std::string			_parentNodeName, 
			const std::string&	_rFilePath);

		//!	@brief	メッシュを最適化する
		void OptimizeMesh();

		//!	@brief	アニメーション情報を読み込む
		void SetupAnimation();

		//----------------------------------------------------------
		// ユーティリティ関数群
		//----------------------------------------------------------

		//!	@brief	FBXのノードをダンプする
		void DumpNode(const char*		_fileName);

		//!	@brief	アニメーション情報をダンプする
		void DumpAnimation(const char*	_fileName);

		//!	@brief	頂点情報などのモデル情報をダンプする
		void DumpModelInfo(const char*	_fileName);

		//!	@brief	クラスター名とクラスターインデックスをダンプする
		void DumpClusterInfo(const char*	_fileName);

	public:
		CFbxLoader();
		~CFbxLoader();

	private:
		// コピーを禁止する
		CFbxLoader(CFbxLoader&){ }
		CFbxLoader& operator = (CFbxLoader&){ }
	};
}