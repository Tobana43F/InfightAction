#pragma once

#include	<array>
#include	<vector>
#include	"Asset\Factory\Asset_ModelFactory.h"
#include	"Model_Def.h"
#include	"Math\Math_Type.h"
#include	"Model_Manager.h"

namespace NModel {

	//!	@brief		描画する数だけ生成するモデルを扱うオブジェクト
	//!	@details	ファイルから読み込んだデータを、アニメーションさせるなどの制御を行う。
	//!	@note		アニメーションは254個まで対応
	class CObject {
	public:

		//!	@brief	使用するモデルを設定する
		//!	@note	_key の値はAsset/Asset_AssetFactoryを参照
		void Load(NAsset::CModelFactory::Key _key);

		//!	@brief	読み込んだ情報を開放する
		void Release();

		//!	@brief	アニメーションを更新
		//!	@note	描画の前に必ず行う。
		void Update();

		//!	@brief	デバッグ用シェーダーで描画
		//!	@note	透明度を無視した描画(モデルの表示テスト用)
		void RenderDebug(NMath::SMatrix44& _matWorld);

		//!	@brief	デバッグ用シェーダ―で描画
		//!	@note	透明度を考慮した描画(描画順のテスト用)
		void RenderDebugAlpha(NMath::SMatrix44& _matWorld);

		//!	@brief	ノードを描画
		void RenderNode(const uint _nodeIndex);

		//!	@brief		アニメーションの変更完全版
		//!	@param[in]	_anmStackIdx			設定するアニメーション
		//!	@param[in]	_isLoop					アニメーションをループするフラグ
		//!										- true : ループ, false : 1度のみ
		//!	@param[in]	_speedFramePerFrame		アニメーション速度
		//!										- SetAnimationSpeed()を参照
		//!										- FLT_MAX を指定すると、現在の速度を維持する
		void ChangeAnimation(
			const uint		_anmStackIdx,
			const bool		_isLoop,
			const uint		_interpoleFrame, 
			const float		_speedFramePerFrame = FLT_MAX);

		//!	@brief		アニメーションの速度を設定
		//!	@details	1フレームで進行するアニメーションのフレーム数
		//!	@param[in]	_speedFramePerFrame	1フレームのアニメーション進行フレーム
		//!				- デフォルトは 1 
		//!				- 負の値を設定すると逆再生
		void SetAnimationSpeed(const float _speedFramePerFrame = 1.0f) { m_anmSpeedFrame = _speedFramePerFrame; }

		//!	@brief		アニメーションのフレームを指定する
		void SetAnimationFrame(const float _frame);


		//!	@brief	モデルデータのフォーマットを取得する
		DWORD	GetFVF() const;

		//!	@brief	モデルの元データを取得する
		const NModel::CManager*	GetModelManager() const { return m_pModelManager.get(); }

		//!	@brief	メッシュノードの数を取得する
		const uint GetMeshNodeCount() const { return m_meshNodes.size(); }

		//!	@brief	メッシュノードを取得する
		const SMeshNode&	GetMeshNode(const uint _nodeIndex) const;

		//!	@brief		ノードを描画する行列を取得する
		//!	@param[out]	_rDest			描画する行列の取得先
		//!	@param[in]	_nodeIndex		描画するノード
		//!	@param[in]	_worldMatrix	オブジェクトのワールド変換行列(GameObjectのワールド変換行列)
		void GetMeshNodeMatrix(
			NMath::SMatrix44&		_rDest,
			const uint				_nodeIndex,
			const NMath::SMatrix44&	_worldMatrix) const;

		//!	@brief		現在のアニメーションインデックスを取得する
		uint	GetAnimationIndex() const { return m_anmStackIndex; }

		//!	@brief		現在のアニメーションフレームを取得する
		//!	@note		0 からのフレーム数で取得する。
		float	GetAnimationFrame() const { return m_anmFrame - GetAnimationStartFrame(); }

		//!	@brief		同じアニメーションを再生した時間を取得する
		//!	@note		時間はm_anmSpeedFrameずつ増える
		float	GetAnimationFrameLoop() const { return m_anmFrameLoop; }

		//!	@brief		アニメーションフレームの長さを取得
		//!	@param[in]	_anmStackIdx	取得するアニメーションのインデックス
		//!				- UINT_MAX を指定すると現在再生中のアニメーションフレームの長さを取得する
		float	GetAnimationFrameLength(unsigned int _anmStackIdx = UINT_MAX) const;

		//!	@brief		アニメーションの開始フレームを取得する
		//!	@param[in]	_anmStackIdx	取得するアニメーションのインデックス
		//!				- UINT_MAX を指定すると現在再生中のアニメーションフレームの長さを取得する
		uint	GetAnimationStartFrame(unsigned int _anmStackIdx = UINT_MAX) const;

		//!	@brief		アニメーションの終了フレームを取得する
		//!	@param[in]	_anmStackIdx	取得するアニメーションのインデックス
		//!				- UINT_MAX を指定すると現在再生中のアニメーションフレームの長さを取得する
		uint	GetAnimationStopFrame(unsigned int _anmStackIdx = UINT_MAX) const;

		//!	@brief		アニメーションの終了フラグを取得する
		bool	GetIsAnimationEnd() const { return m_isAnmEnd; }

		//!	@brief		クラスターの行列を取得
		void	GetClusterMatrix(
			NMath::SMatrix44&	_rDest,
			const uint			_clusterIndex) const;

	private:

		static constexpr uint	CLUSTER_MAX = 255;		// 対応するボーンの数

		NAsset::CModelFactory::SPAsset				m_pModelManager;		// モデルファイルの情報
		std::vector<SMeshNode>						m_meshNodes;			// 使用したり加工したりするためのモデル情報

		bool										m_hasAnimation;			// アニメーションがあるフラグ
		uint										m_clusterCount;			// クラスター数
		std::array<NMath::SMatrix44, CLUSTER_MAX>	m_matCluster;			// クラスターの変換行列情報を保持する
		std::array<NMath::SMatrix44, CLUSTER_MAX>	m_matClusterNode;		// クラスターの変換行列情報を保持する

		std::array<NMath::SMatrix44, CLUSTER_MAX>	m_matInterpoleClusterStart;			// クラスターの補完開始時の行列を保持する
		std::array<NMath::SMatrix44, CLUSTER_MAX>	m_matInterpoleClusterEnd;			// クラスターの補間完了時の行列を保持する
		std::array<NMath::SMatrix44, CLUSTER_MAX>	m_matInterpoleClusterNodeStart;		// クラスターノードの補完開始時の行列を保持する
		std::array<NMath::SMatrix44, CLUSTER_MAX>	m_matInterpoleClusterNodeEnd;		// クラスターノードの補間完了時の行列を保持する


		uint		m_anmStackIndex;	// 現在再生中のアニメーション
		FbxLongLong	m_fbxTimePerFrame;	// Fbxの1フレームの時間単位
		float		m_anmFrame;			// 現在のアニメーションフレーム
										// ※フレーム数だが小数点を許容する
		float		m_anmFrameLoop;		// アニメーションが開始してからの経過時間
										// ※ 0 始まり、abs(m_anmSpeedFrame)ずつ増える
										//    必ず 0 以上の値になる。
		float		m_anmSpeedFrame;	// アニメーションスピード(1フレームで進行するフレーム数)
										// ※デフォルトは１
		bool		m_isLoop;			// ループするかどうか
		bool		m_isAnmEnd;			// アニメーション終了フラグ(ループ時は必ずfalse)
		uint		m_interpoleFrame;			// 補完時間
		uint		m_interpoleFrameCnt;		// フレームカウンタ
		bool		m_isInterpole;				// アニメーション補完フラグ
		bool		m_isInterpoleMatrixReady;	// 補完完了後行列の取得完了フラグ
		float		m_interpoleCompleteFrame;		// 補間完了後のフレーム
		bool		m_isSetupMatrix;				// クラスター行列が設定されているフラグ(※生成時の最初のフレームで補間が行われてしまうのを防止するため)

		//!	@brief	モデル情報から必要情報を抽出し、使用可能状態にする
		void SetupMeshNode();

		//!	@brief	アニメーションの更新をまとめた関数
		void UpdateAnimation();

		//!	@brief	アニメーションの再生フレームを更新
		void UpdateAnimationFrame();

		//!	@brief	ノードの変換行列を更新する
		void UpdateNodeMatrix(
			NModel::CFbxLoader&							_rFbxLoader,
			const float									_rCurFrame);

		//!	@brief	頂点変換を行うため、それぞれのクラスターの行列を作成する
		void UpdateClusterMatrix(
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestClusterMatrix,
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestNodeMatrix,
			NModel::CFbxLoader&							_rFbxLoader,
			const uint									_clusterCount,
			const float									_rCurFrame);

		//!	@brief	頂点を変換する
		void UpdateVertex(
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rSrc,
			NModel::CFbxLoader&							_rFbxLoader);

		//----------------------------------------------------------

		//!	@brief	補完時の、補完完了後のクラスター行列を計算する
		void CalcInterpoleMatrix(
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestClusterMatrix,
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestNodeMatrix,
			NModel::CFbxLoader&							_rFbxLoader,
			const uint									_clusterCount,
			const uint									_interpoleFrame);

		//!	@brief	二つの行列間を補完する
		void InterpoleMatrix(
			const uint									_clusterCount,
			const float									_ratio,
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_destMatrix,
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_startMatrix,
			std::array<NMath::SMatrix44, CLUSTER_MAX>&	_endMatrix);

	public:
		CObject();
		~CObject();
	};

}
