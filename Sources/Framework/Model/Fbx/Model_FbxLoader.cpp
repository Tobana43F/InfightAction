#include	"Model_FbxLoader.h"

#include	<fstream>
#include	<iomanip>
#include	<string>
#include	"Debug\Debug_Assert.h"
#include	"Debug\Debug_Message.h"
#include	"System\System_Error.h"
#include	"System\System_Exception.h"
#include	"Util\Util_Common.h"
#include	"Windows\Windows_Include.h"

#define SafeDestroyFbxObj(pFbxObj) {	\
	if(pFbxObj != nullptr){				\
		pFbxObj->Destroy();				\
		pFbxObj = nullptr;				\
	}									\
}

namespace {

	//!	@brief	FBXSDKを使用可能状態にする
	bool SetupFbxObject(FbxManager*& _pManager, FbxScene*& _pScene)
	{
		// マネージャを作成
		_pManager = FbxManager::Create();
		if (_pManager == nullptr) {
			ErrorEx("FbxManagerの作成に失敗。");
			return false;
		}
		else {
			//m_versionInfo.assign(_pManager->GetVersion());
		}

		// 入出力設定の作成
		FbxIOSettings*	pIOSettings = FbxIOSettings::Create(_pManager, IOSROOT);
		_pManager->SetIOSettings(pIOSettings);

		//	? 実行可能なディレクトリ(オプション)からプラグインをロードする。
		FbxString lPath = FbxGetApplicationDirectory();
		_pManager->LoadPluginsDirectory(lPath.Buffer());

		// シーンの作成
		_pScene = FbxScene::Create(_pManager, "Scene");
		if (_pScene == nullptr) {
			ErrorEx("FbxSceneの作成に失敗。");
			return false;
		}

		return true;
	}

	//!	@brief	FBXのマテリアルから取得した値を整形しセットする
	void SetFbxColor(NModel::SFbxMaterialElement& _rDest, const FbxDouble3& _rSrc)
	{
		_rDest.r = static_cast<float>(_rSrc[0]);
		_rDest.g = static_cast<float>(_rSrc[1]);
		_rDest.b = static_cast<float>(_rSrc[2]);
		_rDest.a = 1.0f;
	}

	//!	@brief	Fbxの行列を独自定義の行列に変換する
	void ToSMatrix44(NMath::SMatrix44& _rDest, const FbxMatrix& _rSrc)
	{
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				_rDest.flt[y][x] = static_cast<float>(_rSrc.Get(y, x));
			}
		}
	}

	//!	@brief	FBXのマテリアル情報を取得する
	void SetMaterialElement(
		NModel::SFbxMaterialElement&	_rDest,
		const FbxSurfaceMaterial*		_pMaterial,
		const char*						_pPropertyName,
		const char*						_pFactorPropertyName,
		const std::string&				_rFilePath)
	{
		// アクセス頻度が高いので省略
		using Type = NModel::SFbxMaterialElement::EType;

		// 初期値を設定
		_rDest.type = Type::NONE;

		// Fbx内の情報を取得
		const FbxProperty property = _pMaterial->FindProperty(_pPropertyName);
		const FbxProperty factorProperty = _pMaterial->FindProperty(_pFactorPropertyName);

		// 色情報を取得
		if (property.IsValid() && factorProperty.IsValid()) {
			FbxDouble3	propertyValue = property.Get<FbxDouble3>();
			double		factor = factorProperty.Get<FbxDouble>();

			// ファクターが存在する場合は設定する
			if (factor != 1) {
				propertyValue[0] *= factor;
				propertyValue[1] *= factor;
				propertyValue[2] *= factor;
			}

			// 値を設定
			SetFbxColor(_rDest, propertyValue);

			// タイプを更新
			_rDest.type = Type::COLOR;
		}

		// テクスチャ情報を取得
		if (property.IsValid()) {
			// 存在するすべてのテクスチャの数をカウントする変数
			int			existTextureCount = 0;

			// 通常のテクスチャ
			const int	textureCount = property.GetSrcObjectCount<FbxFileTexture>();

			// 全てのテクスチャの数だけループ
			for (int i = 0; i < textureCount; ++i) {
				FbxFileTexture* pFileTexture = property.GetSrcObject<FbxFileTexture>(i);
				// 取得できなかった場合は無視
				if (pFileTexture == nullptr) {
					continue;
				}

				FbxString	uvsetName = pFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = pFileTexture->GetFileName();
				std::string relFilePath;
				try {
					size_t offset = filepath.find_last_of('/') + 1;
					if (offset == 0) {
						offset = filepath.find_last_of('\\') + 1;
					}
					std::string fileName = filepath.substr(offset);

					relFilePath = "";
					relFilePath += _rFilePath;
					relFilePath += fileName;
				}
				catch (...) {
					std::string str = "";
					str += "モデル内のテクスチャの相対パスの取得に失敗しました。\n";
					str += "問題のパス : ";
					str += filepath;
					OutErrorMessageEx(str.data());
					throw;
				}
				_rDest.textureSets[uvSetString].emplace_back(relFilePath);
				_rDest.textureSetObjs[uvSetString].emplace_back(nullptr);	// nullptrをセットし、場所だけ確保
				existTextureCount++;
			}

			// レイヤードテクスチャ
			const int layeredTextureCount = property.GetSrcObjectCount<FbxLayeredTexture>();

			// レイヤードテクスチャの数だけループ
			for (int i = 0; i < layeredTextureCount; ++i) {
				FbxLayeredTexture* pLayeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);

				const int textureFileCount = pLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

				for (int j = 0; j < textureFileCount; ++j) {
					FbxFileTexture* pFileTexture = pLayeredTexture->GetSrcObject<FbxFileTexture>(j);
					if (pFileTexture == nullptr) {
						continue;
					}

					FbxString	uvsetName = pFileTexture->UVSet.Get();
					std::string uvSetString = uvsetName.Buffer();
					std::string filepath = pFileTexture->GetFileName();

					_rDest.textureSets[uvSetString].emplace_back(filepath);
					_rDest.textureSetObjs[uvSetString].emplace_back(nullptr);
					existTextureCount++;
				}
			}

			if (existTextureCount > 0) {
				if (_rDest.type == Type::COLOR) {
					_rDest.type = Type::BOTH;
				}
				else {
					_rDest.type = Type::TEXTURE;
				}
			}

		}	// property.IsValid()

	}

	//!	@brief	Fbxのメッシュ情報をコピーする
	//!	@todo	コンテナの可能方法をpush_backではなく初めからresizeでとるようにする。
	void CopyVertices(FbxMesh* _pFbxMesh, NModel::SFbxMeshNode& _rDest)
	{
		AssertNotNull(_pFbxMesh);

		// ポリゴン数を取得
		int polygonCount = _pFbxMesh->GetPolygonCount();

		// 固定値
		_rDest.elementInfo.posCount = 1;
		_rDest.elementInfo.normalCount = 1;

		// 頂点数、ポリゴン数を取得
		_rDest.indexCount = polygonCount * 3;
		_rDest.vertexCount = polygonCount * 3;
		_rDest.polygonCount = polygonCount;

		FbxVector4	pos, normal;	// ワーク変数
		unsigned int loopCount = 0;	// データ格納用配列で使用する添え字

		// 全てのポリゴン分ループ
		for (int iPoly = 0; iPoly < polygonCount; ++iPoly) {
			// 1つのポリゴンの頂点数を取得
			int polygonSize = _pFbxMesh->GetPolygonSize(iPoly);

			// 三角形化を実行しているので、必ず３になるはず
			Assert(polygonSize == 3);

			// 元の頂点インデックスを保持する為に取得(ポリゴン内の0番名の頂点インデックスを取得)

			// posを判定している関係で格納順を変えている
			for (int iVert = polygonSize - 1; iVert >= 0; --iVert) {
				// メッシュ内の頂点インデックスを取得
				int idxVertex = _pFbxMesh->GetPolygonVertex(iPoly, iVert);

				// 頂点情報を取得
				pos = _pFbxMesh->GetControlPointAt(idxVertex);				// 頂点取得
				_pFbxMesh->GetPolygonVertexNormal(iPoly, iVert, normal);	// 法線取得

				// 座標系を合わせるためにx軸を反転
				pos[0]			*= -1;
				normal[0]		*= -1;

				// インデックスバッファを格納
				_rDest.indexes.emplace_back(loopCount);

				_rDest.rawIndexes.emplace_back(idxVertex);
				_rDest.positions.emplace_back(pos);
				_rDest.normals.emplace_back(normal);

				// 添え字を進める
				loopCount++;
			}
		}

		// UVセット(UVは2つ以上ある可能性があるので処理を分ける)
		FbxStringList	uvSetName;
		_pFbxMesh->GetUVSetNames(uvSetName);	// UVSet名一覧を取得
		int uvSetCount = uvSetName.GetCount();
		_rDest.elementInfo.uvSetCount = uvSetCount;

		bool isUnmapped = false;	// 頂点がUVSetを保持しているかのフラグ

		// 全UVSet数分ループ
		for (int iUVSet = 0; iUVSet < uvSetCount; ++iUVSet) {
			// UVSet名とインデックスを対応づける
			_rDest.uvSetID[uvSetName.GetStringAt(iUVSet)] = iUVSet;

			// ポリゴン数分ループ
			for (int iPoly = 0; iPoly < polygonCount; ++iPoly) {
				// 1つのポリゴンの頂点数を取得
				int polygonSize = _pFbxMesh->GetPolygonSize(iPoly);

				// 三角形化を実行しているので、必ず３になるはず
				Assert(polygonSize == 3);

				// 格納順を変えている
				for (int iVert = polygonSize - 1; iVert >= 0; --iVert) {
					FbxString	name = uvSetName.GetStringAt(iUVSet);
					FbxVector2	texCoord;
					_pFbxMesh->GetPolygonVertexUV(iPoly, iVert, name, texCoord, isUnmapped);
					_rDest.texcoords.emplace_back(texCoord);
				}
			}
		}
	}

	//!	@brief	Fbxのマテリアル情報をコピーする
	void CopyMaterials(
		FbxSurfaceMaterial*			_pFbxMaterial,
		NModel::SFbxMaterialNode&	_rDest,
		const std::string&			_rFilePath)
	{
		AssertNotNull(_pFbxMaterial);

		// マテリアルのタイプを取得
		if (_pFbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
			_rDest.type = NModel::SFbxMaterialNode::LAMBERT;
		}
		else if (_pFbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
			_rDest.type = NModel::SFbxMaterialNode::PHONG;
		}

		// マテリアル情報を取得する

		// emmisive
		SetMaterialElement(
			_rDest.emmisive,
			_pFbxMaterial,
			FbxSurfaceMaterial::sEmissive,
			FbxSurfaceMaterial::sEmissiveFactor,
			_rFilePath);

		// ambient
		SetMaterialElement(
			_rDest.ambient,
			_pFbxMaterial,
			FbxSurfaceMaterial::sAmbient,
			FbxSurfaceMaterial::sAmbientFactor,
			_rFilePath);

		// diffuse
		SetMaterialElement(
			_rDest.diffuse,
			_pFbxMaterial,
			FbxSurfaceMaterial::sDiffuse,
			FbxSurfaceMaterial::sDiffuseFactor,
			_rFilePath);

		// specular
		SetMaterialElement(
			_rDest.specular,
			_pFbxMaterial,
			FbxSurfaceMaterial::sSpecular,
			FbxSurfaceMaterial::sSpecularFactor,
			_rFilePath);

		// transparency
		FbxProperty transparencyFactorProperty = _pFbxMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
		if (transparencyFactorProperty.IsValid()) {
			double transparencyFactor = transparencyFactorProperty.Get<FbxDouble>();
			_rDest.transparencyFactor = static_cast<float>(transparencyFactor);
		}

		// specularPwoer
		FbxProperty shininessFactorProperty = _pFbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
		if (shininessFactorProperty.IsValid()) {
			double shininess = shininessFactorProperty.Get<FbxDouble>();
			_rDest.shininess = static_cast<float>(shininess);
		}
	}

	//!	@brief	ノードの行列を設定する
	void SetNodeMatrix(FbxNode* _pNode, NModel::SFbxMeshNode& _rDest)
	{
		if (_pNode == nullptr) {
			return;
		}

		FbxMatrix	matrix;
		matrix.SetIdentity();
		matrix = _pNode->EvaluateGlobalTransform();

		ToSMatrix44(_rDest.matrix, matrix);
	}

	//!	@brief	全てのアニメーションスタック情報を取得する
	//!	@note	vectorコンテナの確保も行う
	void SetupAnimationStack(
		std::vector<NModel::SFbxAnimationStack>&	_rDest,
		const FbxScene*								_pScene,
		const FbxLongLong							_timePerFrame)
	{
		AssertNotNull(_pScene);

		// アニメーションスタック数を取得する
		const int animStackCount = _pScene->GetSrcObjectCount<FbxAnimStack>();

		// アニメーションがない場合は無視
		if (animStackCount < 0) {
			return;
		}

		// 配列確保
		_rDest.resize(animStackCount);

		// すべてのアニメーションスタック分ループ
		for (int i = 0; i < animStackCount; i++) {
			// アニメーションスタックを１つ取得する
			const FbxAnimStack* pAnimStack = _pScene->GetSrcObject<FbxAnimStack>(i);

			// 文字コード変換
			char* anmNameAscii;
			FbxUTF8ToAnsi(pAnimStack->GetName(), anmNameAscii);

			// アニメーション名を取得
			_rDest[i].anmName += anmNameAscii;
			FbxFree(anmNameAscii);

			// Fbx時間単位でのアニメーション時間を取得
			const FbxLongLong	startRaw = pAnimStack->GetLocalTimeSpan().GetStart().Get();
			const FbxLongLong	stopRaw = pAnimStack->GetLocalTimeSpan().GetStop().Get();

			// フレーム数に変換した値を取得する
			const unsigned int	startFrame = StaticCast<unsigned int>(floor(startRaw / _timePerFrame));
			const unsigned int	stopFrame = StaticCast<unsigned int>(floor(stopRaw / _timePerFrame));

			// 値を格納
			_rDest[i].startTime = startRaw;
			_rDest[i].stopTime = stopRaw;
			_rDest[i].startFrame = startFrame;
			_rDest[i].stopFrame = stopFrame;
		}
	}

	//!	@brief		デフォーマー情報を取得する
	//!	@details	- ウェイト値の取得
	//!	@paran[in]	_pFilePath	警告出力時にファイル名を表示するため(オプション)
	void SetSkinDeformer(
		FbxMesh*				_pFbxMesh,
		NModel::SFbxMeshNode&	_rDest,
		const char*				_pFilePath = nullptr)
	{
		// メッシュがない場合は無視
		if (_pFbxMesh == nullptr) {
			return;
		}

		// スキンデフォーマーの数を取得する
		const unsigned int skinCount = _pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);

		// アニメーションがない場合は無視
		if (skinCount <= 0) {
			return;
		}

		// スキンデフォーマーが2つ以上ある場合は警告表示(想定外のモデルデータ)
		if (skinCount >= 2) {
			std::string str = "";
			str += "スキンデフォーマーが ";
			str += skinCount;
			str += " 個存在します。(現在は1個の場合のみ対応している。)\n\n";
			str += "問題のファイル\n";
			str += _pFilePath;
			OutWarningMessageEx(str.data());
			OutWarningMessageEx("読み込みを続行します。(エラーが起きる可能性があります。)");
		}

		// 0番目のスキンデフォーマーのみを取得
		FbxDeformer*	pFbxDeformer = _pFbxMesh->GetDeformer(0, FbxDeformer::eSkin);
		FbxSkin*		pFbxSkin = StaticCast<FbxSkin*>(pFbxDeformer);
		if (pFbxSkin == nullptr) {
			return;
		}

		// スキンへの参照を保持
		_rDest.pFbxSkin = pFbxSkin;

		// クラスター(ボーン = SkeletonNode)の数を取得
		const unsigned int clusterCount = pFbxSkin->GetClusterCount();
		_rDest.clusterCount = clusterCount;

		// メッシュに影響するクラスターがない場合は無視
		if (clusterCount <= 0) {
			return;
		}

		// ウェイトを格納する配列を確保と初期化
		_rDest.weights.resize(_rDest.vertexCount);	// 頂点数分
		for (auto& vertexWeighs : _rDest.weights) {
			vertexWeighs.resize(clusterCount);		// クラスター数分
			std::fill(std::begin(vertexWeighs), std::end(vertexWeighs), 0.0f);
		}

		// クラスターの数分ループ
		for (uint iClst = 0; iClst < clusterCount; ++iClst) {
			FbxCluster*	pFbxCluster = pFbxSkin->GetCluster(iClst);

			// ウェイトの設定を確認
			if (pFbxCluster->GetLinkMode() != FbxCluster::eNormalize) {
				// ウェイトの合計が１にならないような設定の場合は対応していないことを通知
				std::string	str = "";
				str += "ウェイトの格納形式が対応していません。\n";
				str += "問題のファイル\n";
				str += _pFilePath;
				OutWarningMessageEx(str.data());
			}

			// ウェイト配列を取得
			int		indexCount	= pFbxCluster->GetControlPointIndicesCount();	// ボーンが影響する頂点のインデックス数
			int*	indexes		= pFbxCluster->GetControlPointIndices();			// ボーンが影響する頂点のインデックス配列
			double*	weights		= pFbxCluster->GetControlPointWeights();			// ボーンが影響するウェイト配列

			// インデックス数分ループ
			for (int iIdx = 0; iIdx < indexCount; ++iIdx) {
				float	weight	= StaticCast<float>(weights[iIdx]);	// ウェイトを取得
				int		vertIdx = indexes[iIdx];

				// 全ての頂点のうち、同じインデックス情報を保持している頂点に値を格納する
				for (uint i = 0; i < _rDest.indexCount; ++i) {
					if (_rDest.rawIndexes[i] == vertIdx) {
						_rDest.weights[i][iClst] = weight;
					}
				}
			}
		}

	}

	//!	@brief	ノードの状態をダンプする
	void DumpFbxNodeRecursively(std::ofstream& _file, FbxNode* _pNode, int _depth = 0)
	{
		// インデントを整形
		for (int i = 0; i < _depth; ++i) {
			_file << " 　";
		}

		// ノード情報を取得
		char* pNodeNameAscii;
		FbxUTF8ToAnsi(_pNode->GetName(), pNodeNameAscii);
		std::string	nodeName = pNodeNameAscii;
		FbxFree(pNodeNameAscii);

		int			childNum = _pNode->GetChildCount();
		int			attributeNum = _pNode->GetNodeAttributeCount();	// 基本１以下

		// 情報を表示
		_file << "+ " << nodeName.data() << " ";				// ノード名をダンプ
		_file << "(ChildNum[ " << childNum << " ]) ";			// 子の数をダンプ
		_file << "(AttributeNum[ " << attributeNum << " ]) ";	// 属性の数をダンプ

		// 属性の情報を表示
		for (int i = 0; i < attributeNum; ++i) {
			// ノードの属性を取得
			FbxNodeAttribute*	pNodeAttribute = _pNode->GetNodeAttributeByIndex(i);

			char* pAttrClassName;
			char* pAttrName;
			FbxUTF8ToAnsi(pNodeAttribute->GetClassId().GetName(), pAttrClassName);
			FbxUTF8ToAnsi(pNodeAttribute->GetName(), pAttrName);

			_file << "{ Attribute(" << i << ") " <<
				"[" << pAttrClassName << "] " <<
				": " << pAttrName <<
				" }";

			FbxFree(pAttrClassName);
			FbxFree(pAttrName);
		}
		_file << std::endl;

		// 子ノードを解析
		for (int i = 0; i < childNum; ++i) {
			FbxNode* pNode = _pNode->GetChild(i);
			DumpFbxNodeRecursively(_file, pNode, _depth + 1);
		}
	}
}

namespace NModel {

	CFbxLoader::CFbxLoader() :
		m_pFbxManager(nullptr),
		m_pFbxScene(nullptr),
		m_pFbxImporter(nullptr)
	{

	}

	CFbxLoader::~CFbxLoader()
	{
		SafeDestroyFbxObj(m_pFbxImporter);
		SafeDestroyFbxObj(m_pFbxScene);
		SafeDestroyFbxObj(m_pFbxManager);

		// テクスチャの解放を行う
		for (uint i = 0; i < m_meshNodes.size(); ++i) {
			m_meshNodes[i].Release();
		}
	}

	bool CFbxLoader::Load(const char* _filePath, EFbxAxisSystem	_axis)
	{
		bool sts = false;	// 汎用フラグ変数

		try {
			// FBXオブジェクトを初期化
			sts = SetupFbxObject(m_pFbxManager, m_pFbxScene);
			if (!sts) { return false; }

			// インポータ作成
			int fileFmt = -1;
			m_pFbxImporter = FbxImporter::Create(m_pFbxManager, "Importer");
			if (m_pFbxImporter == nullptr) {
				ErrorEx("FbxImporterの作成に失敗。");
				return false;
			}

			// ファイルのフォーマットを確認
			if (!m_pFbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(_filePath, fileFmt)) {
				// 認識されないファイルフォーマット。バイナリ形式で読み込む。
				fileFmt = m_pFbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
			}

			// インポータ初期化
			sts = m_pFbxImporter->Initialize(_filePath, fileFmt);
			if (!sts) {
				ErrorEx("FbxImporterの初期化に失敗。");
				return false;
			}

			// シーン(モデルデータ)の読み込み
			sts = m_pFbxImporter->Import(m_pFbxScene);
			if (!sts) {
				ErrorEx("FbxSceneの読み込みに失敗。");
				return false;
			}

			// 座標系を変換
			FbxAxisSystem myAxisSystem = FbxAxisSystem::DirectX;
			if (_axis == EFbxAxisSystem::OPENGL) {
				myAxisSystem = FbxAxisSystem::OpenGL;
			}
			else if (_axis == EFbxAxisSystem::MAYA_Y_UP) {
				myAxisSystem = FbxAxisSystem::MayaYUp;
			}

			// DirectX系に変換
			FbxAxisSystem sceneAxisSystem = m_pFbxScene->GetGlobalSettings().GetAxisSystem();
			if (sceneAxisSystem != myAxisSystem) {
				//myAxisSystem.ConvertScene(m_pFbxScene);
			}
			myAxisSystem.ConvertScene(m_pFbxScene);

			// 単位系の統一(メートル単位にコンバート)
			FbxSystemUnit sceneSystemUnit = m_pFbxScene->GetGlobalSettings().GetSystemUnit();
			if (sceneSystemUnit != FbxSystemUnit::m) {
				FbxSystemUnit::m.ConvertScene(m_pFbxScene);
			}

			// 三角形化
			// コンバーターを作成
			FbxGeometryConverter geometryConverter(m_pFbxManager);
			sts = geometryConverter.Triangulate(m_pFbxScene, true);	// シーン内の全てのノードを三角形化する。
			if (!sts) {
				ErrorEx("三角形化に失敗。");
				return false;
			}
		}
		catch (NSystem::CException _exception) {
			// エラーが起きたファイル情報を表示する
			std::string	str = "";
			str += _filePath;
			str += " の読み込み時に次の例外が発生しました。";
			OutErrorMessageEx(str.data());
			throw;	// アプリケーションに例外を投げる
		}

		// モデルファイルのパスを保持しておく
		m_filePath = _filePath;

		// ファイルのルートパス名のみ取得する
		size_t size = m_filePath.find_last_of("/") + 1;	// + 1　で/を含める
		if (size != std::string::npos) {
			m_fileDirPath = m_filePath.substr(0, size);
		}
		else {
			std::string str = "";
			str += "テクスチャのパスが不正な可能性があります。\nPath : ";
			str += m_fileDirPath.data();
			OutWarningMessageEx(str.data());
			m_fileDirPath = m_filePath;
		}

	#ifdef	_GAME_DEBUG
		DumpNode(_filePath);
	#endif

		// データの読み込み
		SetupNode();

		// アニメーションの読み込み
		SetupAnimation();

	#ifdef _GAME_DEBUG
		DumpClusterInfo(_filePath);
	#endif

		return true;
	}

	void CFbxLoader::SetAnmStack(const unsigned int _idxAnmStack)
	{
		Assert(_idxAnmStack < GetAnimationCount());

		// シーン内のアニメーション情報を切り替える
		FbxAnimStack* pAnmStack = m_pFbxScene->GetSrcObject<FbxAnimStack>(_idxAnmStack);
		AssertNotNull(pAnmStack);
		m_pFbxScene->SetCurrentAnimationStack(pAnmStack);
	}

	//----------------------------------------------------------

	void CFbxLoader::SetupNode()
	{
		FbxNode* pRootNode = m_pFbxScene->GetRootNode();
		if (pRootNode != nullptr) {
			LoadNodeRecursively(pRootNode, "null", m_fileDirPath);
		}

		OptimizeMesh();

	#ifdef _GAME_DEBUG
		DumpModelInfo(m_filePath.data());
	#endif
	}

	void CFbxLoader::LoadNodeRecursively(
		FbxNode*			_pNode,
		std::string			_parentNodeName,
		const std::string&	_rFilePath)
	{
		if (_pNode == nullptr) {
			return;
		}

		bool isMeshNode = false;

		// メッシュノードデータを作成
		SFbxMeshNode	meshNode;
		
		// 名前を変換
		char* pNodeNameAscii;
		FbxUTF8ToAnsi(_pNode->GetName(), pNodeNameAscii);

		meshNode.nodeName = pNodeNameAscii;
		FbxFree(pNodeNameAscii);

		meshNode.parentNodeName = _parentNodeName;

		// メッシュデータを取得
		FbxMesh* pFbxMesh = _pNode->GetMesh();

		if (pFbxMesh != nullptr) {
			// メッシュへのポインタを保持
			meshNode.pFbxMesh = pFbxMesh;

			// 頂点数を取得
			const int vertexCount = pFbxMesh->GetControlPointsCount();

			if (vertexCount > 0) {
				// 頂点が存在する場合はノードにコピー
				CopyVertices(pFbxMesh, meshNode);
				 
				isMeshNode = true;
			}
		}

		if (isMeshNode) {
			// マテリアルデータを取得
			const int materialCount = _pNode->GetMaterialCount();
			for (int i = 0; i < materialCount; ++i) {
				FbxSurfaceMaterial* pMaterial = _pNode->GetMaterial(i);

				// マテリアルが存在しない場合は無視する(そんな場合はある？)
				if (pMaterial == nullptr) {
					continue;
				}

				// マテリアルデータを取得
				SFbxMaterialNode	materialNode;
				CopyMaterials(pMaterial, materialNode, _rFilePath);
				materialNode.Load();

				// 取得したマテリアルを格納
				meshNode.materials.emplace_back(materialNode);

				// マテリアル情報からテクスチャを読み込む
				//meshNode.materials.back().Load();
			}

			// ノードの変換行列を取得
			SetNodeMatrix(_pNode, meshNode);

			// デフォーマー情報を取得(スキンアニメーション情報のみ)
			SetSkinDeformer(pFbxMesh, meshNode);

			// 最適化するための構造体に値をコピー
			meshNode.optimizedIndex.resize(meshNode.indexes.size());
			memcpy(&meshNode.optimizedIndex[0], &meshNode.indexes[0], meshNode.indexes.size() * sizeof(unsigned int));
			meshNode.optimizedVertex.resize(meshNode.vertexCount);
			for (unsigned int i = 0; i < meshNode.vertexCount; ++i) {
				meshNode.optimizedVertex[i].pos = meshNode.positions[i];
				meshNode.optimizedVertex[i].normal = meshNode.normals[i];
				meshNode.optimizedVertex[i].rawIndex = meshNode.rawIndexes[i];

				if (meshNode.texcoords.size() > 0) {
					meshNode.optimizedVertex[i].texCoord = meshNode.texcoords[i];
				}
				else {
					meshNode.optimizedVertex[i].texCoord = FbxVector2(0, 0);
				}

				if (meshNode.weights.size() > 0) {
					meshNode.optimizedVertex[i].weights.resize(meshNode.weights[i].size());
					memcpy(&meshNode.optimizedVertex[i].weights[0], &meshNode.weights[i][0], meshNode.weights[i].size() * sizeof(float));
				}
			}

			// 作成したメッシュデータを格納
			m_meshNodes.emplace_back(meshNode);
		}

		// 再帰的に読み込む
		const int childCount = _pNode->GetChildCount();
		for (int i = 0; i < childCount; ++i) {
			LoadNodeRecursively(_pNode->GetChild(i), meshNode.nodeName, _rFilePath);
		}
	}

	void CFbxLoader::OptimizeMesh()
	{
		// 全メッシュノードを解析
		for (uint iMesh = 0; iMesh < m_meshNodes.size(); ++iMesh) {
			SFbxMeshNode& meshNode = m_meshNodes[iMesh];

			//!	@todo
			// 重複頂点削除
			std::vector<SFbxVertex>		optimizedVertex;
			std::vector<unsigned int>	optimizedIndex;

			optimizedIndex.clear();
			optimizedVertex.clear();

			// 頂点数分ループ
			for (unsigned int i = 0; i < meshNode.vertexCount; ++i) {
				SFbxVertex& vertex = meshNode.optimizedVertex[i];

				auto itr = std::find(optimizedVertex.begin(), optimizedVertex.end(), vertex);

				// 重複していない場合は追加
				if (itr == optimizedVertex.end()) {
					optimizedIndex.emplace_back(optimizedVertex.size());
					optimizedVertex.emplace_back(vertex);
				}
				// 存在する場合
				else {
					unsigned int index = std::distance(optimizedVertex.begin(), itr);	// 現イテレータの位置を添え字とする
					optimizedIndex.emplace_back(index);
				}
			}

			// 全て置き換えたら最適化したメッシュを保持する
			meshNode.optimizedIndex.swap(optimizedIndex);
			meshNode.optimizedVertex.swap(optimizedVertex);
		}
	}

	void CFbxLoader::DumpNode(const char*	_fileName)
	{
		// 出力先ディレクトリに移動
		const int BUF_SIZE = 256;
		char prevDirectory[BUF_SIZE];
		GetCurrentDirectory(BUF_SIZE, prevDirectory);
		SetCurrentDirectory("Output/Dump/Fbx/Node/");

		// 出力ファイル名を設定
		std::string fileName = "";
		fileName += _fileName;
		fileName += ".FbxNodeDump";
		NUtil::ReplaceString(fileName, "/", "_");

		// ファイルオープン
		std::ofstream	file;
		file.open(fileName.data(), file.out | file.trunc);

		file << "ファイル名 : " << _fileName << std::endl;
		file << std::endl;

		// 再帰的にノードをたどりダンプする
		DumpFbxNodeRecursively(
			file,
			m_pFbxScene->GetRootNode());

		// ファイルクローズ
		file.close();

		// カレントディレクトリを戻す
		SetCurrentDirectory(prevDirectory);
	}

	void CFbxLoader::DumpModelInfo(const char* _fileName)
	{
		// 出力先ディレクトリに移動
		const int BUF_SIZE = 256;
		char prevDirectory[BUF_SIZE];
		GetCurrentDirectory(BUF_SIZE, prevDirectory);
		SetCurrentDirectory("Output/Dump/Fbx/Info/");

		// 出力ファイル名を設定
		std::string fileName = "";
		fileName += _fileName;
		fileName += ".FbxModelInfoDump";
		NUtil::ReplaceString(fileName, "/", "_");

		// ファイルオープン
		std::ofstream	file;
		file.open(fileName.data(), file.out | file.trunc);

		file << "モデル内情報" << std::endl;
		file << std::endl;

		// メッシュノードの頂点数、インデックス数を出力
		uint vertexCountSum = 0;
		uint indexCountSum = 0;
		uint optVertexCountSum = 0;
		uint optIndexCountSum = 0;
		for (uint i = 0; i < m_meshNodes.size(); ++i) {
			SFbxMeshNode& meshNode = m_meshNodes[i];
			vertexCountSum += meshNode.vertexCount;
			indexCountSum += meshNode.indexCount;
			optVertexCountSum += meshNode.optimizedVertex.size();
			optIndexCountSum += meshNode.optimizedIndex.size();
		}

		file << "全メッシュの頂点数　　　　　　　　　　　" << vertexCountSum << std::endl;
		file << "全メッシュのインデックス数　　　　　　　" << indexCountSum << std::endl;
		file << "ポリゴン数(インデックス数 / 3)　　　 　" << StaticCast<uint>(indexCountSum / 3) << std::endl;
		file << "最適化後の全メッシュの頂点数　　　　　　" << optVertexCountSum << std::endl;
		file << "最適化後の全メッシュのインデックス数 　 " << optIndexCountSum << std::endl;

		file << std::endl;

		for (uint i = 0; i < m_meshNodes.size(); ++i) {
			SFbxMeshNode& meshNode = m_meshNodes[i];

			uint vertexCount = meshNode.vertexCount;
			uint indexCount = meshNode.indexCount;

			uint optVertexCount = meshNode.optimizedVertex.size();
			uint optIndexCount = meshNode.optimizedIndex.size();

			file << meshNode.nodeName << std::endl;	// メッシュ名
			file << "    vertexCount    : " << vertexCount <<  std::endl;
			file << "    IndexCount     : " << indexCount << std::endl;

			file << "    optVertexCount : " << optVertexCount << std::endl;
			file << "    optIndexCount  : " << optIndexCount << std::endl;
		}

		file.close();

		// カレントディレクトリを戻す
		SetCurrentDirectory(prevDirectory);
	}

	void CFbxLoader::DumpClusterInfo(const char*	_fileName)
	{	
		Assert(m_meshNodes.size() > 0);

		// 出力先ディレクトリに移動
		const int BUF_SIZE = 256;
		char prevDirectory[BUF_SIZE];
		GetCurrentDirectory(BUF_SIZE, prevDirectory);
		SetCurrentDirectory("Output/Dump/Fbx/Cluster/");

		// 出力ファイル名を設定
		std::string fileName = "";
		fileName += _fileName;
		fileName += ".FbxClusterDump";
		NUtil::ReplaceString(fileName, "/", "_");

		// ファイルオープン
		std::ofstream	file;
		file.open(fileName.data(), file.out | file.trunc);

		file << "モデル内のクラスター情報" << std::endl;
		file << std::endl;

		const FbxSkin*	pFbxSkin = m_meshNodes[0].pFbxSkin;

		// クラスターがない場合
		if (pFbxSkin == nullptr) {
			file << "クラスターは存在しません。" << std::endl;
		}
		// クラスターがある場合
		else {
			const int			clusterCount = pFbxSkin->GetClusterCount();
			for (int i = 0; i < clusterCount; ++i) {
				const FbxCluster*	pFbxCluster = pFbxSkin->GetCluster(i);
				const char*			pClusterName = pFbxCluster->GetName();
				char*				pClusterNameAscii = nullptr;

				FbxUTF8ToAnsi(pClusterName, pClusterNameAscii);

				file << "[" << i << "]  " <<
					pClusterNameAscii << std::endl;

				FbxFree(pClusterNameAscii);
			}
		}

		file.close();

		// カレントディレクトリを戻す
		SetCurrentDirectory(prevDirectory);

	}

	//----------------------------------------------------------

	void CFbxLoader::SetupAnimation()
	{
		// 時間の情報を取得
		FbxGlobalSettings* pGlobalSettings = &m_pFbxScene->GetGlobalSettings();

		// 1フレームの時間を取得
		const FbxTime::EMode	timeMode = pGlobalSettings->GetTimeMode();
		const FbxLongLong		timePerFrame = FbxTime::GetOneFrameValue(timeMode);
		m_timePerFrame = timePerFrame;

		// アニメーションスタック情報を取得する
		SetupAnimationStack(m_anmStack, m_pFbxScene, timePerFrame);

	#ifdef _GAME_DEBUG
		DumpAnimation(m_filePath.data());
	#endif
	}

	void CFbxLoader::DumpAnimation(const char*	_fileName)
	{
		// 出力先ディレクトリに移動
		const int BUF_SIZE = 256;
		char prevDirectory[BUF_SIZE];
		GetCurrentDirectory(BUF_SIZE, prevDirectory);
		SetCurrentDirectory("Output/Dump/Fbx/Animation/");

		// 出力ファイル名を設定
		std::string fileName = "";
		fileName += _fileName;
		fileName += ".FbxAnimationDump";
		NUtil::ReplaceString(fileName, "/", "_");

		// ファイルオープン
		std::ofstream	file;
		file.open(fileName.data(), file.out | file.trunc);

		// アニメーションスタックの情報をダンプ
		file << "AnimationStack数 : " << m_anmStack.size() << std::endl;
		file << std::endl;

		// 全ての情報を書き込む
		for (uint i = 0; i < m_anmStack.size(); ++i) {
			SFbxAnimationStack& anmStack = m_anmStack[i];

			file<<	anmStack.anmName << " "
					"Frame [" << anmStack.startFrame << " - " << anmStack.stopFrame << "]" <<
					std::endl;
		}
		file << std::endl;

		file.close();

		// カレントディレクトリを戻す
		SetCurrentDirectory(prevDirectory);
	}

}
