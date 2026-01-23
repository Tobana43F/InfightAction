#include	"Collision_Game.h"

#include	"Collision\Collision_Util.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"
#include	"Debug\Debug_Assert.h"
#include	"Debug\Debug_Util.h"
#include	"Math\Math_Common.h"

#include	<fstream>
#include	<string>
#include	"_External\json11\json11.hpp"
#include	"System\System_Error.h"	

namespace {
	// テスト
	json11::Json g_data;

	void CreateMeshBox(
		LPD3DXMESH& _rDest,
		const float3& _rSize)
	{
		ReleaseSafe(_rDest);
		HRESULT hr = E_FAIL;
		hr = D3DXCreateBox(
			NDirectX::GetD3DDevice(),
			_rSize.x,				// 幅
			_rSize.y,				// 高さ
			_rSize.z,				// 奥行
			&_rDest,
			nullptr);

		if (FAILED(hr)) {
			ErrorEx("メッシュの作成に失敗しました。");
		}
	}
}

namespace NCollision {

	CGame::CGame() :
		m_isEnable(false),
		m_isHit(false),
		m_isGroupHit(false),
		m_pRefOwner(nullptr),
		m_isRefBone(false),
		m_isCreateOBBByBone(false),
		m_refBoneIndex(UINT_MAX),		// 不正な値を設定しておく
		m_isRefOwnerTransform(false),
		m_boxMesh(nullptr)
	{

	}

	CGame::~CGame()
	{

	}

	void CGame::Load(
		NObject::CGameObject*	_pRefOwner,
		const uint				_groupID,
		const uint				_colGameID,
		const char*				_pCollisionInfoFilePath,
		const ModelData*		_pRefModelData)
	{
		try {
			// ファイルオープン
			std::fstream file(_pCollisionInfoFilePath);
			if (file.fail()) {
				throw("ファイル読み込み失敗");
			}
			std::istreambuf_iterator<char>	begin(file);
			std::istreambuf_iterator<char>	end;
			std::string	fileContent(begin, end);

			std::string	err;
			g_data = json11::Json::parse(fileContent, err);
			if (!err.empty()) {
				throw(err.data());
			}

			// 当たり判定IDを設定
			m_collisionID = _colGameID;

			// 所有者を設定
			m_pRefOwner = _pRefOwner;
			m_pRefModel = _pRefModelData;

			// 基本的な情報を読み込む
			LoadCollisionInfo(
				_groupID,
				_colGameID,
				_pCollisionInfoFilePath);
		}
		catch (const char* _pErr) {
			std::string err = "";
			err += "当たり判定の読み込みでエラーが発生しました。\n\n";
			err += _pErr;
			err += "\n";
			err += "FileName : ";
			err += _pCollisionInfoFilePath;
			ErrorEx(err.data());
		}
	}

	void CGame::Release()
	{
		ReleaseSafe(m_boxMesh);
	}

	void CGame::Init()
	{
		m_isEnable = false;
	}

	void CGame::Update()
	{
		m_isHit = false;
		m_isGroupHit = false;

		NMath::SMatrix44	matOwnerTransform;
		NMath::SMatrix44	matBone;
		if (m_isRefBone) {	
			// ボーン行列を設定
			m_pRefModel->GetClusterMatrix(matBone, m_refBoneIndex);
		}
		if (m_isRefOwnerTransform) {
			// 親の行列を設定
			m_pRefOwner->GetTransform().CalcWorldMatrix(matOwnerTransform);
		}

		// 最終的な行列を計算する
		NMath::SMatrix44	matFinal;
		matFinal = matBone * matOwnerTransform;

		// OBB更新
		m_obb.Update(matFinal);
	}

	void CGame::RenderDebug()
	{
		NMath::SMatrix44	matWorld;
		matWorld = m_obb.GetOBBox().axisDir;
		matWorld.vec[3].vec3 = m_obb.GetOBBox().centerPos;
		matWorld.vec[3].w = 1.0f;

		D3DXMATRIX	d3dMatWorld;
		NDirectX::ToD3DMatrix(d3dMatWorld, matWorld);

		// マテリアルを設定
		if (m_isEnable) {
			NDirectX::GetD3DDevice()->SetMaterial(&m_materialEnable);

			if (m_isHit) {
				NDirectX::GetD3DDevice()->SetMaterial(&m_materialHit);
			}
			else if (m_isGroupHit) {
				NDirectX::GetD3DDevice()->SetMaterial(&m_materialGroupHit);
			}
		}
		else {
			NDirectX::GetD3DDevice()->SetMaterial(&m_materialDisable);
		}

		NDirectX::GetD3DDevice()->SetTexture(0, nullptr);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_WORLD, &d3dMatWorld);
		m_boxMesh->DrawSubset(0);
	}


	//----------------------------------------------------------

	//!	@brief	当たり判定に共通で必要なデータを読み込む
	void CGame::LoadCollisionInfo(
		const uint				_groupID,
		const uint				_colGameID,
		const char*				_pCollisionInfoFilePath)
	{
		// Gameの値(ゲーム配列をループし、自分の情報を探す)
		json11::Json jsonGroup = g_data["Group"][_groupID];
		json11::Json jsonCollision = jsonGroup["Collision"].array_items()[_colGameID];

		// 所有者の変換行列の参照をするかどうか
		if (jsonCollision["IsRefOwnerTransform"].bool_value()) {
			m_isRefOwnerTransform = true;
		}

		// 参照ボーン
		m_isCreateOBBByBone = jsonCollision["IsCreateByBone"].bool_value();
		if (!jsonCollision["RefBoneID"].is_null()) {
			AssertNotNull(m_pRefModel && "ボーンを参照する設定ですが、参照するモデルデータが設定されていません。");
			m_isRefBone = true;
			m_refBoneIndex = jsonCollision["RefBoneID"].int_value();
		}
		else {
			if (m_isCreateOBBByBone) {
				throw("ボーン情報をもとにあたり判定を作成する設定ですが、参照するボーンが設定されていません。");
			}
		}

		// OBB情報
		LoadOBBData(_groupID, _colGameID);

		NDirectX::MakeMaterialData(m_materialEnable, float4(0, 0, 1, 0.25f));
		NDirectX::MakeMaterialData(m_materialHit, float4(1, 0, 0, 0.25f));
		NDirectX::MakeMaterialData(m_materialGroupHit, float4(1, 0.125f, 0, 0.25f));
		NDirectX::MakeMaterialData(m_materialDisable, float4(0, 0, 0, 0.25f));
	}

	void CGame::LoadOBBData(
		const uint				_groupID,
		const uint				_colGameID)
	{
		json11::Json jsonGroup = g_data["Group"][_groupID];
		json11::Json jsonCollision = jsonGroup["Collision"].array_items()[_colGameID];

		float3 pos;
		float3 size;
		float3 posture;

		// 各軸の要素を取得
		for (int i = 0; i < 3; ++i) {
			pos.array[i]		= StaticCast<float>(jsonCollision["Pos"][i].number_value());
			size.array[i]		= StaticCast<float>(jsonCollision["Size"][i].number_value());
			posture.array[i]	= StaticCast<float>(jsonCollision["Posture"][i].number_value());
		}
		// 回転要素から、姿勢を表す行列を作成
		NMath::SMatrix44	matRot;
		NMath::MakeMatrixRotation(matRot, posture.x, posture.y, posture.z);

		// データファイル内の情報をもとに、OBB情報を読み込む場合
		if (!m_isCreateOBBByBone) {
			// OBBデータとして設定する
			m_obb.SetData(
				pos,
				size,
				matRot);
		}
		// ボーンに影響する頂点情報から、OBB情報を作成する場合
		else {
			AssertNotNull(m_pRefModel->GetModelManager() && "モデルマネージャがNullです。");

			// 警告表示
		#ifdef _GAME_DEBUG
			std::string	str = "ボーンに影響する頂点情報からあたり判定を作成する設定の場合、";
			if (posture.GetLengthSquare() > FLT_EPSILON) {
				str += "postureの情報は無視されます。";
				OutWarningMessageEx(str.data());
			}
		#endif

			// 指定されたボーンに影響する頂点から、当たり判定情報を作成する
			NCollision::SOBBox obb;
			NCollision::CreateBoneOBB(
				obb,
				*m_pRefModel,
				m_refBoneIndex);

			// sizeの情報は、倍率として使用する
			obb.size *= size;

			// ボーンに追従させるため、座標はボーンの原点、姿勢もボーンに従うため、
			// サイズ以外のデータは無視する
			m_obb.SetData(
				obb.centerPos,
				obb.size,
				obb.axisDir);

			// デバッグ用の見た目を作成するために設定
			size = obb.size;
		}

		// デバッグ用
		CreateMeshBox(
			m_boxMesh,
			size);
	}
}
