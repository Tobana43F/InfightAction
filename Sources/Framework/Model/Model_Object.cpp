#include	"Model_Object.h"

#include	<array>
#include	<string>
#include	<vector>
#include	"Asset\Factory\Asset_ShaderFactory.h"
#include	"Debug\Debug_Assert.h"
#include	"DirectX\DirectX_Manager.h"
#include	"Fbx\Model_FbxDef.h"
#include	"Fbx\Model_FbxUtil.h"
#include	"System\System_Error.h"

// テスト
#include	"Math\Math_Common.h"
#include	"Camera\Camera_GlobalManager.h"
#include	"DirectX\DirectX_DebugFont.h"

#include	<omp.h>
#include	<mmsystem.h>
#pragma		comment(lib, "winmm.lib")

namespace {

	NAsset::CShaderFactory::SPAsset	g_shaderDebug;	// デバッグ用シェーダー
	NAsset::CShaderFactory::SPAsset	g_shaderDebugAlpha;	// デバッグ用シェーダー


	const DWORD	MODEL_FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	//!	@brief	ノードの変換行列を取得
	void GetGeometry(FbxAMatrix& _rDest, FbxNode* pNode)
	{
		AssertNotNull(pNode);

		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

		_rDest = FbxAMatrix(lT, lR, lS);

	}


	//!	@brief	モデル情報から頂点バッファを作成
	void CreateVertexBuffer(
		LPDIRECT3DDEVICE9		_pD3DDevice,
		NModel::SFbxMeshNode&	_rFbxMeshNode,
		NModel::SMeshNode&		_rDest)
	{
		AssertNotNull(_pD3DDevice);
		const uint vertexCount = _rFbxMeshNode.optimizedVertex.size();
		if (vertexCount <= 0) {
			return;
		}

		_rDest.vertexCount = vertexCount;

		//----------------------------------------------------------
		// 頂点データを作成
		std::vector<NModel::SVertex>	vertices(vertexCount);

		for (uint i = 0; i < vertexCount; ++i) {
			const FbxVector4&	pos = _rFbxMeshNode.optimizedVertex[i].pos;
			const FbxVector4&	normal = _rFbxMeshNode.optimizedVertex[i].normal;
			const FbxVector2&	texCoord = _rFbxMeshNode.optimizedVertex[i].texCoord;

			vertices[i].pos = NMath::SFloat3Data(
				StaticCast<float>(pos.mData[0]), 
				StaticCast<float>(pos.mData[1]), 
				StaticCast<float>(pos.mData[2]));
			vertices[i].normal = NMath::SFloat3Data(
				StaticCast<float>(normal.mData[0]), 
				StaticCast<float>(normal.mData[1]), 
				StaticCast<float>(normal.mData[2]));

			// U反転
			vertices[i].texCoord = NMath::SFloat2Data(
				StaticCast<float>(abs(texCoord.mData[0])),
				StaticCast<float>(abs(1.0f - texCoord.mData[1])));
		}

		//----------------------------------------------------------
		// 頂点バッファを作成
		uint length = vertexCount * sizeof(NModel::SMeshNode::VertexType);
		DWORD	fvf = MODEL_FVF;
		HRESULT	hr = E_FAIL;
		hr = _pD3DDevice->CreateVertexBuffer(
			length,
			0,
			fvf,
			D3DPOOL_MANAGED,
			&_rDest.pVB,
			nullptr);
		if (FAILED(hr)) {
			ErrorEx("頂点バッファの作成に失敗しました。");
		}

		// 頂点バッファにデータを設定する為にロックする
		NModel::SMeshNode::VertexType*	pVertexDest = nullptr;
		hr = _rDest.pVB->Lock(
			0,
			0,
			(void**)&pVertexDest,
			0);
		if (FAILED(hr)) {
			ErrorEx("頂点バッファのロックに失敗しました。");
		}

		// 頂点データを設定
		memcpy(pVertexDest, &vertices[0], length);

		// アンロック
		_rDest.pVB->Unlock();

		//----------------------------------------------------------
		// 作成したデータを初期状態として保持
		_rDest.initVertices.swap(vertices);

		// 後処理
		vertices.clear();

		//----------------------------------------------------------
		// ウェイトを取得

		// ウェイトを格納する配列を確保
		_rDest.weights.resize(vertexCount);
		for (uint i = 0; i < vertexCount; ++i) {
			_rDest.weights[i].resize(_rFbxMeshNode.clusterCount);
		}

		// 値を確認
		for (uint i = 0; i < _rFbxMeshNode.optimizedVertex.size(); ++i) {
			float weightSum = 0;
			for (uint j = 0; j < _rFbxMeshNode.optimizedVertex[i].weights.size(); ++j) {
				float weight = _rFbxMeshNode.weights[i][j];
				weightSum += weight;
			}

			// 法線が正規化されていない場合は正規化する
			float nromalWeightSum = 0;
			if (abs(1.0f - weightSum) > FLT_EPSILON) {
				for (uint j = 0; j < _rFbxMeshNode.optimizedVertex[i].weights.size(); ++j) {
					_rFbxMeshNode.weights[i][j] /= weightSum;
					nromalWeightSum += _rFbxMeshNode.weights[i][j];
				}
			}
		}

		// 値を取得
		for (uint i = 0; i < _rFbxMeshNode.optimizedVertex.size(); ++i) {
			const auto& rFbxWeights		= _rFbxMeshNode.optimizedVertex[i].weights;
			auto&		rDestWeights	= _rDest.weights[i];

			Assert(rFbxWeights.size() == _rFbxMeshNode.clusterCount);

			for (uint j = 0; j < rFbxWeights.size(); ++j) {
				rDestWeights[j] = rFbxWeights[j];
			}
		}
	}

	//!	@brief	モデル情報からインデックスバッファを作成
	void CreateIndexBuffer(
		LPDIRECT3DDEVICE9		_pD3DDevice,
		NModel::SFbxMeshNode&	_rFbxMeshNode,
		NModel::SMeshNode&		_rDest)
	{
		AssertNotNull(_pD3DDevice);
		uint indexCount = _rFbxMeshNode.optimizedIndex.size();
		if (indexCount <= 0) {
			return;
		}

		_rDest.indexCount = indexCount;

		// インデックスデータを作成
		uint				length = indexCount * sizeof(uint);

		// インデックスバッファを作成
		HRESULT hr = E_FAIL;
		hr = _pD3DDevice->CreateIndexBuffer(
			length,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX32,
			D3DPOOL_MANAGED,
			&_rDest.pIB,
			nullptr);
		if (FAILED(hr)) {
			ErrorEx("インデックスバッファの作成に失敗。");
		}

		// インデックスデータをロック
		NModel::SMeshNode::IndexType*	pIndexDest = nullptr;
		hr = _rDest.pIB->Lock(
			0,
			0,
			(void**)&pIndexDest,
			0);

		// インデックスデータを設定
		memcpy(pIndexDest, &_rFbxMeshNode.optimizedIndex[0], length);

		// アンロック　
		_rDest.pIB->Unlock();
	}

	//!	@brief	マテリアル情報を作成
	void CreateMaterial(
		LPDIRECT3DDEVICE9		_pD3DDevice,
		NModel::SFbxMeshNode&	_rFbxMeshNode,
		NModel::SMeshNode&		_rDest)
	{
		AssertNotNull(_pD3DDevice);
		if (_rFbxMeshNode.materials.size() <= 0) {
			return;
		}

		// 今回は先頭のマテリアルだけ使う
		NModel::SFbxMaterialNode fbxMaterial = _rFbxMeshNode.materials[0];
		_rDest.material.specularPower = fbxMaterial.shininess;
		_rDest.material.transparencyFactor = fbxMaterial.transparencyFactor;

		_rDest.material.ambient
			= float4(fbxMaterial.ambient.r, fbxMaterial.ambient.g, fbxMaterial.ambient.b, fbxMaterial.ambient.a);
		_rDest.material.diffuse
			= float4(fbxMaterial.diffuse.r, fbxMaterial.diffuse.g, fbxMaterial.diffuse.b, fbxMaterial.diffuse.a);
		_rDest.material.specular
			= float4(fbxMaterial.specular.r, fbxMaterial.specular.g, fbxMaterial.specular.b, fbxMaterial.specular.a);
		_rDest.material.emmisive
			= float4(fbxMaterial.emmisive.r, fbxMaterial.emmisive.g, fbxMaterial.emmisive.b, fbxMaterial.emmisive.a);

		// Diffuseのテクスチャを取得する
		if (fbxMaterial.diffuse.textureSets.size() > 0) {

			auto itr = fbxMaterial.diffuse.textureSetObjs.begin();
			if (itr->second.size() > 0) {
				Assert(itr->second[0] != nullptr && "テクスチャが読み込まれていません。");

				// テクスチャへの参照を保持する
				_rDest.material.texture = itr->second[0];
			}
			Assert(_rDest.material.texture != nullptr && "テクスチャがnullptrでした。");
		}
	}

	//!	@brief	メッシュの頂点情報を更新
	void UpdateMeshVertex(
		IDirect3DVertexBuffer9*&					_pVB,
		const std::vector<NModel::SVertex>&			_initVertices,
		const std::vector<std::vector<float>>&		_weights,
		const NMath::SMatrix44&						_rNodeMatrixInv,
		const uint									_clusterCount,
		const std::array<NMath::SMatrix44, 255>&	_matCluster)
	{
		Assert(_weights.size() == _initVertices.size());
		std::vector<NModel::SVertex>	vertices;
		vertices.resize(_initVertices.size());
		memcpy(
			&vertices[0],
			&_initVertices[0],
			sizeof(NModel::SVertex) * _initVertices.size());

		int vertexCount = vertices.size();
		
		// ボーン行列とノード行列を使って更新
	#pragma omp parallel for
		for (int iVert = 0; iVert < vertexCount; ++iVert) {

			NMath::SMatrix44 mat;
			mat.SetZero();

			// 影響するすべてのクラスターの行列を加算
			bool	isBoneExist = false;	// 影響するボーンがあることを示すフラグ

			for (uint iClst = 0; iClst < _clusterCount; ++iClst) {
				Assert(_weights[iVert].size() == _clusterCount);

				// ボーン行列の影響がない場合は無視
				float weight = _weights[iVert][iClst];
				if (weight < FLT_EPSILON) {
					continue;
				}

				isBoneExist = true;

				// 影響するボーン行列を合成していく
				mat += _matCluster[iClst] * weight;
			}

			if (isBoneExist) {
				NMath::SMatrix44 matAdjust;
				matAdjust.e11 = -1;
				mat = matAdjust * mat * _rNodeMatrixInv;

				// 頂点を変換
				NModel::SVertex& v = vertices[iVert];
				NMath::SVector3	pos(v.pos.x, v.pos.y, v.pos.z);
				NMath::TransformVector(pos, mat);
				v.pos.x = pos.x * -1;
				v.pos.y = pos.y;
				v.pos.z = pos.z;
			}
		}

		//----------------------------------------------------------
		// 頂点バッファに代入する
		//----------------------------------------------------------
		HRESULT hr = E_FAIL;
		NModel::SVertex*	pVertexDest = nullptr;
		hr = _pVB->Lock(
			0,
			0,
			(void**)&pVertexDest,
			0);
		Assert(SUCCEEDED(hr));

		// 値をコピー
		memcpy(pVertexDest, &vertices[0], sizeof(NModel::SVertex) * _initVertices.size());

		// アンロック
		_pVB->Unlock();
	}

	//!	@brief	クラスター行列を更新
	void CalcClusterMatrix(
		NMath::SMatrix44&		_rMatDest,
		NMath::SMatrix44&		_rMatDestClusterNode,
		const FbxLongLong		_curFbxTime,
		const int				_clusterIndex,
		const FbxMesh*			_pFbxMesh,
		FbxSkin*				_pFbxSkin)
	{
		// クラスターを取得
		FbxCluster*	pFbxCluster = _pFbxSkin->GetCluster(_clusterIndex);

		//----------------------------------------------------------
		// FBXサンプルを少し改造
		//----------------------------------------------------------
		FbxAMatrix lReferenceGlobalInitPosition;
		pFbxCluster->GetTransformMatrix(lReferenceGlobalInitPosition);

		FbxAMatrix lReferenceGeometry;
		FbxAMatrix matGeometry;
		GetGeometry(lReferenceGeometry, _pFbxMesh->GetNode());
		matGeometry = lReferenceGlobalInitPosition * lReferenceGeometry;

		FbxAMatrix lClusterGlobalInitPosition;
		pFbxCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

		FbxNode* node = pFbxCluster->GetLink();
		FbxAMatrix lClusterGlobalCurrentPosition = node->EvaluateGlobalTransform(_curFbxTime);	// 結構時間かかる

		FbxAMatrix lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * matGeometry;
		FbxAMatrix lClusterRelativeCurrentPositionInverse = lClusterGlobalCurrentPosition;

		FbxAMatrix matrixFinal;
		matrixFinal = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;

		// 自分の行列に変換
		NMath::SMatrix44	matrix;
		NModel::ToSMatrix44(matrix, matrixFinal);

		// 格納
		_rMatDest = matrix;

		//----------------------------------------------------------
		FbxAMatrix	matAdjust;
		matAdjust.SetS(FbxVector4(-1, 1, 1, 1));
		FbxAMatrix	matClusterNode;
		matClusterNode =
			matAdjust * 
			lClusterGlobalCurrentPosition * 
			lReferenceGlobalInitPosition;
		NModel::ToSMatrix44(_rMatDestClusterNode, matClusterNode);

		// 姿勢部分を正規化しスケーリングを無視する
		for (int i = 0; i < 3; ++i) {
			_rMatDestClusterNode.vec[i].Normalize();
		}
	}
}

namespace NModel {

	CObject::CObject() :
		m_pModelManager(nullptr),
		m_anmStackIndex(0),
		m_fbxTimePerFrame(0),
		m_anmFrame(0),
		m_anmFrameLoop(0),
		m_anmSpeedFrame(1),
		m_hasAnimation(false),
		m_isLoop(true),
		m_isAnmEnd(false),
		m_interpoleFrame(0),
		m_isInterpole(false),
		m_isInterpoleMatrixReady(false),
		m_interpoleCompleteFrame(0),
		m_isSetupMatrix(false)
	{

	}

	CObject::~CObject()
	{
		Release();
	}

	void CObject::Load(NAsset::CModelFactory::Key _key)
	{
		// モデルデータを取得
		m_pModelManager = NAsset::CModelFactory::GetInst().Load(_key);

		// モデルデータから作成
		SetupMeshNode();

		// デバッグ用シェーダーをロード
		NAsset::CShaderFactory::Key keyDebugShader = "Debug/DebugTexture.hlsl";
		NAsset::CShaderFactory::GetInst().SetLoadInfo(
			keyDebugShader,
			"VS",
			"PS");
		g_shaderDebug = NAsset::CShaderFactory::GetInst().Load(keyDebugShader);

		NAsset::CShaderFactory::Key keyDebugAlphaShader = "Debug/DebugAlpha.hlsl";
		NAsset::CShaderFactory::GetInst().SetLoadInfo(
			keyDebugAlphaShader,
			"VS",
			"PS");
		g_shaderDebugAlpha = NAsset::CShaderFactory::GetInst().Load(keyDebugAlphaShader);
	}

	void CObject::Release()
	{
		for (uint i = 0; i < m_meshNodes.size(); ++i) {
			m_meshNodes[i].Release();
		}
		m_meshNodes.clear();
	}

	void CObject::Update()
	{
		UpdateAnimation();
	}

	void CObject::RenderDebug(NMath::SMatrix44& _matWorld)
	{
		uint nodeCount = m_meshNodes.size();
		if (nodeCount <= 0) {
			return;
		}

		// カメラを設定
		NCamera::SetViewMatrix();
		NCamera::SetProjMatrix();
		NCamera::SetViewport();

		UpdateAnimation();

		for (uint i = 0; i < nodeCount; ++i) {

			SMeshNode& meshNode = m_meshNodes[i];

			if (meshNode.vertexCount <= 0) {
				continue;
			}

			// DirectXの描画設定
			NDirectX::GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			NDirectX::GetD3DDevice()->SetFVF(MODEL_FVF);
			g_shaderDebug->SetShaderToPipeline();

			// ワールド変換行列の設定
			D3DXMATRIX mat;
			NMath::SMatrix44 matWorld;

			matWorld = meshNode.matrix * _matWorld;

			// シェーダ―へ行列を設定
			NDirectX::ToD3DMatrix(mat, matWorld);
			g_shaderDebug->GetVSCT()->SetMatrix(NDirectX::GetD3DDevice(), "g_world", &mat);

			NMath::SMatrix44 view;
			NCamera::GetViewMatrix(view);
			NDirectX::ToD3DMatrix(mat, view);
			g_shaderDebug->GetVSCT()->SetMatrix(NDirectX::GetD3DDevice(), "g_view", &mat);

			NMath::SMatrix44 proj;
			NCamera::GetProjMatrix(proj);
			NDirectX::ToD3DMatrix(mat, proj);
			g_shaderDebug->GetVSCT()->SetMatrix(NDirectX::GetD3DDevice(), "g_projection", &mat);

			bool hasTexture = false;
			if (meshNode.material.texture != nullptr) {
				hasTexture = true;
			}
			g_shaderDebug->GetPSCT()->SetBool(NDirectX::GetD3DDevice(), "g_hasTexture", hasTexture);
			int idx = g_shaderDebug->GetPSCT()->GetSamplerIndex("g_sampler");
			NDirectX::GetD3DDevice()->SetTexture(idx, meshNode.material.texture);

			// 頂点バッファを設定
			NDirectX::GetD3DDevice()->SetStreamSource(
				0,
				meshNode.pVB,
				0,
				sizeof(SVertex));

			// インデックスバッファを設定
			NDirectX::GetD3DDevice()->SetIndices(meshNode.pIB);

			uint polygonCount = (int)(meshNode.indexCount / 3.0f);
			HRESULT hr;
			hr = NDirectX::GetD3DDevice()->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				meshNode.indexCount,
				0,
				polygonCount);

			NDirectX::GetD3DDevice()->SetVertexShader(nullptr);
			NDirectX::GetD3DDevice()->SetPixelShader(nullptr);
			NDirectX::GetD3DDevice()->SetStreamSource(0, nullptr, 0, 0);
			NDirectX::GetD3DDevice()->SetIndices(nullptr);
			NDirectX::GetD3DDevice()->SetTexture(0, nullptr);

		}
	}

	void CObject::RenderDebugAlpha(NMath::SMatrix44& _matWorld)
	{
		uint nodeCount = m_meshNodes.size();
		if (nodeCount <= 0) {
			return;
		}

		// カメラを設定
		NCamera::SetViewMatrix();
		NCamera::SetProjMatrix();
		NCamera::SetViewport();

		UpdateAnimation();

		for (uint i = 0; i < nodeCount; ++i) {

			SMeshNode& meshNode = m_meshNodes[i];

			if (meshNode.vertexCount <= 0) {
				continue;
			}

			// DirectXの描画設定
			NDirectX::GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			NDirectX::GetD3DDevice()->SetFVF(MODEL_FVF);
			g_shaderDebugAlpha->SetShaderToPipeline();

			// ワールド変換行列の設定
			D3DXMATRIX mat;
			NMath::SMatrix44 matWorld;

			matWorld = meshNode.matrix * _matWorld;

			// シェーダ―へ行列を設定
			NDirectX::ToD3DMatrix(mat, matWorld);
			g_shaderDebugAlpha->GetVSCT()->SetMatrix(NDirectX::GetD3DDevice(), "g_world", &mat);

			NMath::SMatrix44 view;
			NCamera::GetViewMatrix(view);
			NDirectX::ToD3DMatrix(mat, view);
			g_shaderDebugAlpha->GetVSCT()->SetMatrix(NDirectX::GetD3DDevice(), "g_view", &mat);

			NMath::SMatrix44 proj;
			NCamera::GetProjMatrix(proj);
			NDirectX::ToD3DMatrix(mat, proj);
			g_shaderDebugAlpha->GetVSCT()->SetMatrix(NDirectX::GetD3DDevice(), "g_projection", &mat);

			bool hasTexture = false;
			if (meshNode.material.texture != nullptr) {
				hasTexture = true;
			}
			g_shaderDebugAlpha->GetPSCT()->SetBool(NDirectX::GetD3DDevice(), "g_hasTexture", hasTexture);
			int idx = g_shaderDebugAlpha->GetPSCT()->GetSamplerIndex("g_sampler");
			NDirectX::GetD3DDevice()->SetTexture(idx, meshNode.material.texture);

			// 頂点バッファを設定
			NDirectX::GetD3DDevice()->SetStreamSource(
				0,
				meshNode.pVB,
				0,
				sizeof(SVertex));

			// インデックスバッファを設定
			NDirectX::GetD3DDevice()->SetIndices(meshNode.pIB);

			uint polygonCount = (int)(meshNode.indexCount / 3.0f);
			HRESULT hr;
			hr = NDirectX::GetD3DDevice()->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				meshNode.indexCount,
				0,
				polygonCount);

			NDirectX::GetD3DDevice()->SetVertexShader(nullptr);
			NDirectX::GetD3DDevice()->SetPixelShader(nullptr);
			NDirectX::GetD3DDevice()->SetStreamSource(0, nullptr, 0, 0);
			NDirectX::GetD3DDevice()->SetIndices(nullptr);
			NDirectX::GetD3DDevice()->SetTexture(0, nullptr);

		}
	}


	void CObject::RenderNode(const uint _nodeIndex)
	{
		Assert(_nodeIndex < m_meshNodes.size());
		SMeshNode& meshNode = m_meshNodes[_nodeIndex];

		// 頂点がない場合は何もしない
		if (meshNode.vertexCount <= 0) {
			return;
		}

		// 頂点バッファを設定
		NDirectX::GetD3DDevice()->SetStreamSource(
			0,
			meshNode.pVB,
			0,
			sizeof(SVertex));

		// インデックスバッファを設定
		NDirectX::GetD3DDevice()->SetIndices(meshNode.pIB);

		// バッファを使用して描画
		uint polygonCount = StaticCast<int>(meshNode.indexCount / 3.0f);
		HRESULT hr;
		hr = NDirectX::GetD3DDevice()->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			meshNode.indexCount,
			0,
			polygonCount);

		// リセット
		NDirectX::GetD3DDevice()->SetStreamSource(0, nullptr, 0, 0);
		NDirectX::GetD3DDevice()->SetIndices(nullptr);
	}

	void CObject::ChangeAnimation(
		const uint		_anmStackIdx,
		const bool		_isLoop,
		const uint		_interpoleFrame,
		const float		_speedFramePerFrame)
	{
		// アニメーションが存在しない場合は警告を出力し、無視する
		if (!m_hasAnimation) {
			OutWarningMessageEx("アニメーションが存在しないのに、アニメーションの変更が呼ばれました。");
			return;
		}

		Assert(_anmStackIdx < m_pModelManager->GetModel().GetAnimationCount());

		m_anmStackIndex = _anmStackIdx;
		m_isLoop = _isLoop;
		m_isAnmEnd = false;	// アニメーション終了フラグをリセット

		// アニメーション速度を設定
		if (_speedFramePerFrame != FLT_MAX) {	// FLT_MAXが渡されたときは、スピードを変更しない
			SetAnimationSpeed(_speedFramePerFrame);
		}

		// アニメーションの初期フレームを設定
		const SFbxAnimationStack& anmStack = m_pModelManager->GetModel().GetAnmStack(_anmStackIdx);
		// 0または正の値の時は、開始フレームを設定
		if (m_anmSpeedFrame >= 0) {
			m_anmFrame = static_cast<float>(anmStack.startFrame);
		}
		else {
			m_anmFrame = static_cast<float>(anmStack.stopFrame);
		}
		m_anmFrameLoop = 0;

		Assert(_interpoleFrame * abs(m_anmSpeedFrame) <= GetAnimationFrameLength() &&
				"補完するフレーム数がアニメーションの長さを超えています。");

		// 補完設定
		m_interpoleFrameCnt = 1;
		if (_interpoleFrame > 0) {
			m_interpoleFrame			= _interpoleFrame;
			m_isInterpole				= true;
			m_isInterpoleMatrixReady	= false;
		}
		else {
			m_interpoleFrame			= 0;
			m_isInterpole				= false;
			m_isInterpoleMatrixReady	= false;
		}
	}

	void CObject::SetAnimationFrame(const float _frame)
	{
		Assert(_frame < GetAnimationStartFrame() && "指定したフレームが範囲外です");
		Assert(_frame > GetAnimationStopFrame() && "指定したフレームが範囲外です");
		m_anmFrame = _frame;
	}

	DWORD CObject::GetFVF() const 
	{
		return MODEL_FVF;
	}

	const SMeshNode& CObject::GetMeshNode(const uint _nodeIndex) const
	{
		Assert(_nodeIndex < m_meshNodes.size());
		return m_meshNodes[_nodeIndex];
	}

	void CObject::GetMeshNodeMatrix(
		NMath::SMatrix44&		_rDest,
		const uint				_nodeIndex,
		const NMath::SMatrix44&	_worldMatrix) const
	{
		Assert(_nodeIndex < m_meshNodes.size());
		_rDest = m_meshNodes[_nodeIndex].matrix * _worldMatrix;
	}

	float CObject::GetAnimationFrameLength(unsigned int _anmStackIdx) const
	{
		// 今のアニメーションインデックスを設定する
		unsigned int anmStackIdx = m_anmStackIndex;

		// 指定されたアニメーションのフレームを取得する場合
		if (_anmStackIdx != UINT_MAX) {
			Assert(_anmStackIdx < m_pModelManager->GetModel().GetAnimationCount() && "指定したアニメーションインデックスが不正です。");
			anmStackIdx = _anmStackIdx;
		}
		const SFbxAnimationStack& anmStack = m_pModelManager->GetModel().GetAnmStack(anmStackIdx);

		float anmStartFrame = static_cast<float>(anmStack.startFrame);
		float anmStopFrame = static_cast<float>(anmStack.stopFrame);

		return (anmStopFrame - anmStartFrame);
	}

	uint CObject::GetAnimationStartFrame(unsigned int _anmStackIdx) const 
	{
		// 今のアニメーションインデックスを設定する
		unsigned int anmStackIdx = m_anmStackIndex;

		// 指定されたアニメーションのフレームを取得する場合
		if (_anmStackIdx != UINT_MAX) {
			Assert(_anmStackIdx < m_pModelManager->GetModel().GetAnimationCount() && "指定したアニメーションインデックスが不正です。");
			anmStackIdx = _anmStackIdx;
		}
		const SFbxAnimationStack& anmStack = m_pModelManager->GetModel().GetAnmStack(anmStackIdx);
		return anmStack.startFrame;
	}

	uint CObject::GetAnimationStopFrame(unsigned int _anmStackIdx) const 
	{
		// 今のアニメーションインデックスを設定する
		unsigned int anmStackIdx = m_anmStackIndex;

		// 指定されたアニメーションのフレームを取得する場合
		if (_anmStackIdx != UINT_MAX) {
			Assert(_anmStackIdx < m_pModelManager->GetModel().GetAnimationCount() && "指定したアニメーションインデックスが不正です。");
			anmStackIdx = _anmStackIdx;
		}
		const SFbxAnimationStack& anmStack = m_pModelManager->GetModel().GetAnmStack(anmStackIdx);
		return anmStack.stopFrame;
	}

	void CObject::GetClusterMatrix(
		NMath::SMatrix44&	_rDest,
		const uint			_clusterIndex) const 
	{
		Assert(_clusterIndex < m_clusterCount);
		_rDest = m_matClusterNode[_clusterIndex];
	}

	//----------------------------------------------------------

	void CObject::SetupMeshNode()
	{
		AssertNotNull(m_pModelManager);

		// アニメーションが存在するかを取得
		if(m_pModelManager->GetModel().GetAnimationCount() > 0){
			m_hasAnimation = true;
		}

		// アニメーションの単位時間を取得
		m_fbxTimePerFrame = m_pModelManager->GetModel().GetFbxTimePerFrame();

		// メッシュノード数を取得
		const uint nodeCount = m_pModelManager->GetModel().GetMeshNodeCount();
		AssertPlus(nodeCount);

		// すべてのメッシュノードを必要な形に整形し取得
		m_meshNodes.resize(nodeCount);
		for (uint i = 0; i < nodeCount; ++i) {
			SMeshNode		meshNode;
			SFbxMeshNode&	rFbxMeshNode = m_pModelManager->GetModel().GetMeshNode(i);

			// インデックスバッファを作成
			CreateIndexBuffer(
				NDirectX::GetD3DDevice(),
				rFbxMeshNode,
				meshNode);

			// 頂点バッファを作成
			CreateVertexBuffer(
				NDirectX::GetD3DDevice(),
				rFbxMeshNode,
				meshNode);

			// 変換行列を取得
			meshNode.matrix = rFbxMeshNode.matrix;

			// マテリアル情報を作成
			CreateMaterial(
				NDirectX::GetD3DDevice(),
				rFbxMeshNode,
				meshNode);

			// クラスタ数を取得
			m_clusterCount = rFbxMeshNode.clusterCount;

			// デバッグ情報を取得
			meshNode.nodeName = rFbxMeshNode.nodeName;

			// 格納
			m_meshNodes[i] = meshNode;
		}
	}

	void CObject::UpdateAnimation()
	{
		// アクセス省略
		NModel::CFbxLoader&	rFbx = m_pModelManager->GetModel();

		UpdateNodeMatrix(rFbx, m_anmFrame);

		// アニメーションがない場合は何もしない
		if (!m_hasAnimation) {
			return;
		}

		// 使用するアニメーションを切り替える
		rFbx.SetAnmStack(m_anmStackIndex);

		//----------------------------------------------------------
		// 処理が多い

		if (m_isInterpole && m_isSetupMatrix) {
			// 補完に必要な行列を計算する
			if (!m_isInterpoleMatrixReady) {
				// 補完後の行列を作成する
				CalcInterpoleMatrix(
					m_matInterpoleClusterEnd,
					m_matInterpoleClusterNodeEnd,
					rFbx,
					m_clusterCount,
					m_interpoleFrame);

				// 補完開始時の行列を保持する(現在の行列をコピー)
				memcpy(
					&m_matInterpoleClusterStart[0],
					&m_matCluster[0],
					sizeof(std::array<NMath::SMatrix44, CLUSTER_MAX>));
				memcpy(
					&m_matInterpoleClusterNodeStart[0],
					&m_matClusterNode[0],
					sizeof(std::array<NMath::SMatrix44, CLUSTER_MAX>));

				m_isInterpoleMatrixReady = true;
			}

			// 行列を補完する
			float	ratio = StaticCast<float>(m_interpoleFrameCnt) / StaticCast<float>(m_interpoleFrame + 1);

			// クラスター行列を補完
			InterpoleMatrix(
				m_clusterCount,
				ratio,
				m_matCluster,
				m_matInterpoleClusterStart,
				m_matInterpoleClusterEnd);

			// クラスターノードの行列を補完
			InterpoleMatrix(
				m_clusterCount,
				ratio,
				m_matClusterNode,
				m_matInterpoleClusterNodeStart,
				m_matInterpoleClusterNodeEnd);

			// 補完の終了を検知
			if (m_interpoleFrameCnt >= m_interpoleFrame) {
				m_isInterpole				= false;
				m_isInterpoleMatrixReady	= false;
				
				// 次のフレームを補間完了後のフレームに設定
				m_anmFrame = m_interpoleCompleteFrame - m_anmSpeedFrame;
			}
			else {
				m_interpoleFrameCnt++;
			}
		}
		else {
			// 頂点変換用のボーン行列を作成
			UpdateClusterMatrix(m_matCluster, m_matClusterNode, rFbx, m_clusterCount, m_anmFrame);
			m_isSetupMatrix = true;
		}

		// 作成したボーン行列で頂点変換
		UpdateVertex(m_matCluster, rFbx);
		//----------------------------------------------------------

		// アニメーションの制御
		UpdateAnimationFrame();
	}

	void CObject::UpdateAnimationFrame()
	{
		const SFbxAnimationStack& anmStack = m_pModelManager->GetModel().GetAnmStack(m_anmStackIndex);

		// loop用カウンタの終了時動作用に、更新前の値を保持する
		float prevAnmFrameLoop = m_anmFrameLoop;
		m_anmFrameLoop += abs(m_anmSpeedFrame);

		// フレームを更新
		m_anmFrame += m_anmSpeedFrame;

		// アニメーション終了フラグをリセット
		m_isAnmEnd = false;

		// 終了した時の動作
		// 通常再生時
		if (m_anmSpeedFrame >= 0) {
			if (m_anmFrame >= static_cast<float>(anmStack.stopFrame)) {
				if (m_isLoop) {
					// ループ時は、最初のフレームに戻す
					m_anmFrame = static_cast<float>(anmStack.startFrame);
				}
				else {
					// ループしない場合は、最後のフレームを代入し続ける
					m_anmFrame = static_cast<float>(anmStack.stopFrame);
					m_isAnmEnd = true;	// 終了フラグ
					m_anmFrameLoop = prevAnmFrameLoop;
				}
			}
		}
		// 逆再生時
		else {
			// 逆再生で終了した時の動作
			if (m_anmFrame <= static_cast<float>(anmStack.startFrame)) {
				if (m_isLoop) {
					// ループ時は、最後のフレームに戻す
					m_anmFrame = static_cast<float>(anmStack.stopFrame);
				}
				else {
					// 終了時は、最初のフレームを代入し続ける
					m_anmFrame = static_cast<float>(anmStack.startFrame);
					m_isAnmEnd = true;	// 終了フラグ
					m_anmFrameLoop = prevAnmFrameLoop;
				}
			}
		}
	}

	void CObject::UpdateNodeMatrix(
		NModel::CFbxLoader&		_rFbxLoader,
		const float				_rCurFrame)
	{
		const FbxLongLong	fbxTimePerFrame = _rFbxLoader.GetFbxTimePerFrame();
		const FbxLongLong	fbxCurTime = static_cast<FbxLongLong>(_rCurFrame * fbxTimePerFrame);

		uint meshCount = _rFbxLoader.GetMeshNodeCount();

		for (uint iMesh = 0; iMesh < meshCount; ++iMesh) {
			SFbxMeshNode& rFbxMesh = _rFbxLoader.GetMeshNode(iMesh);
			FbxMesh* pFbxMesh = rFbxMesh.pFbxMesh;

			//AssertNotNull(pFbxMesh);
			if (pFbxMesh == nullptr) {
				continue;
			}

			FbxAMatrix mat = pFbxMesh->GetNode()->EvaluateGlobalTransform(fbxCurTime);
			ToSMatrix44(m_meshNodes[iMesh].matrix, mat);			
		}
	}

	void CObject::UpdateClusterMatrix(
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestClusterMatrix,
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestNodeMatrix,
		NModel::CFbxLoader&							_rFbxLoader,
		const uint									_clusterCount,
		const float									_rCurFrame)
	{
		AssertPlus(_rFbxLoader.GetMeshNodeCount());	// メッシュの存在を確認

		// 0番目のメッシュを使い、Fbxのメッシュ情報などにアクセスする
		SFbxMeshNode& rFbxMeshNode = _rFbxLoader.GetMeshNode(0);

		// スキンを取得
		FbxMesh* const pFbxMesh = rFbxMeshNode.pFbxMesh;
		FbxSkin* const pFbxSkin = rFbxMeshNode.pFbxSkin;

		// Fbx単位時間を取得
		const FbxLongLong	fbxTimePerFrame = _rFbxLoader.GetFbxTimePerFrame();


		const FbxLongLong	fbxCurTime = static_cast<FbxLongLong>(_rCurFrame * fbxTimePerFrame);

		// クラスター数を取得
		for (uint iClst = 0; iClst < _clusterCount; ++iClst) {
			::CalcClusterMatrix(
				_rDestClusterMatrix[iClst],
				_rDestNodeMatrix[iClst],
				fbxCurTime,
				iClst,
				pFbxMesh,
				pFbxSkin);
		}
	}

	void CObject::UpdateVertex(
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rSrc,
		NModel::CFbxLoader&							_rFbxLoader)
	{
		// Fbx単位時間を取得
		const FbxLongLong	fbxTimePerFrame = m_fbxTimePerFrame;

		int meshNodeCount = m_meshNodes.size();
		std::vector<SMeshNode>&	meshNodes = m_meshNodes;

		// すべてのメッシュノードを変換する
		for (int iMesh = 0; iMesh < meshNodeCount; ++iMesh) {
			// メッシュノードを取得
			SMeshNode&		rMeshNode = meshNodes[iMesh];

			// ノードの変換行列の逆行列
			NMath::SMatrix44 matMeshNodeInv;
			matMeshNodeInv = rMeshNode.matrix;
			matMeshNodeInv.Inverse();

			//----------------------------------------------------------
			// 変換後の頂点を作成
			//----------------------------------------------------------
			UpdateMeshVertex(
				rMeshNode.pVB,
				rMeshNode.initVertices,
				rMeshNode.weights,
				matMeshNodeInv,
				m_clusterCount,
				m_matCluster);
		}
	}

	//----------------------------------------------------------

	void CObject::CalcInterpoleMatrix(
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestClusterMatrix,
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_rDestNodeMatrix,
		NModel::CFbxLoader&							_rFbxLoader,
		const uint									_clusterCount,
		const uint									_interpoleFrame)
	{
		AssertPlus(_rFbxLoader.GetMeshNodeCount());	// メッシュの存在を確認
		Assert(m_isInterpole);
		Assert(!m_isInterpoleMatrixReady);
		AssertPlus(_interpoleFrame);

		// 0番目のメッシュを使い、Fbxのメッシュ情報などにアクセスする
		SFbxMeshNode& rFbxMeshNode = _rFbxLoader.GetMeshNode(0);

		// スキンを取得
		FbxMesh* const pFbxMesh = rFbxMeshNode.pFbxMesh;
		FbxSkin* const pFbxSkin = rFbxMeshNode.pFbxSkin;

		// 補間時の時間を取得
		float				anmFrameOffset;		// 開始(終了)フレーム
		if (m_anmSpeedFrame >= 0) {
			anmFrameOffset = StaticCast<float>(GetAnimationStartFrame());
		}
		else {
			anmFrameOffset = StaticCast<float>(GetAnimationStopFrame());
		}

		m_interpoleCompleteFrame = (m_anmSpeedFrame * _interpoleFrame) + anmFrameOffset;
		const FbxLongLong	fbxTimePerFrame = _rFbxLoader.GetFbxTimePerFrame();
		const FbxLongLong	fbxTime = static_cast<FbxLongLong>(m_interpoleCompleteFrame * fbxTimePerFrame);

		// クラスター数を取得
		for (uint iClst = 0; iClst < _clusterCount; ++iClst) {
			::CalcClusterMatrix(
				_rDestClusterMatrix[iClst],
				_rDestNodeMatrix[iClst],
				fbxTime,
				iClst,
				pFbxMesh,
				pFbxSkin);
		}
	}


	void CObject::InterpoleMatrix(
		const uint									_clusterCount,
		const float									_ratio,
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_destMatrix,
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_startMatrix,
		std::array<NMath::SMatrix44, CLUSTER_MAX>&	_endMatrix)
	{
		float ratio = _ratio;
		// _ratioをクランプ
		if (ratio < 0) {
			ratio = 0;
		}
		else if (ratio > 1) {
			ratio = 1;
		}

		// 行列からクオータニオンに変換
		for (uint i = 0; i < _clusterCount; ++i) {
			float ratio0 = 1 - ratio;
			float ratio1 = ratio;

			_destMatrix[i] =
				_startMatrix[i] * ratio0 +
				_endMatrix[i] * ratio1;
		}
	}
}
