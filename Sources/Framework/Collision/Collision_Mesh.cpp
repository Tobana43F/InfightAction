#include	"Collision_Mesh.h"

#include	"Collision_Util.h"
#include	"Collision_Common.h"
#include	"Debug\Debug_Util.h"
#include	"System\System_Error.h"
#include	"Debug\Debug_Message.h"
#include	"Math\Math_Common.h"
#include	"Math\Math_Util.h"
#include	"Mesh\Mesh_Line.h"
#include	"Model\Model_Def.h"

namespace {

	// 描画する法線の長さ
	const float RENDER_NORMAL_LENGTH = 0.3f;

	//!	@brief		渡されたメッシュの情報から当たり判定情報を生成する
	//!	@param[out]	_rPlaneInfo	面情報の出力先
	//!	@param[out]	_rFaceCount	面の数の出力先
	//!	@param[in]	_rModel		面情報を作成するのに使用するモデルデータ
	void CreatMeshPlaneInfo(
		std::vector<NCollision::SPlaneInfo>&	_rPlaneInfo,
		uint&									_rFaceCount,
		NModel::CFbxLoader&						_rModel);
}

namespace NCollision {

	CMesh::CMesh()
	{
		m_planeInfo.clear();
		m_faceCount = 0;
	}

	CMesh::~CMesh()
	{
		Release();
	}

	void CMesh::Load(NAsset::CModelFactory::Key _key)
	{
		m_model = NAsset::CModelFactory::GetInst().Load(_key);
		CreatMeshPlaneInfo(
			m_planeInfo,
			m_faceCount,
			m_model->GetModel());
	}

	void CMesh::Release()
	{

	}

	void CMesh::RenderNormal()
	{
		// 全ての面
		for (uint i = 0; i < m_planeInfo.size(); ++i) {

			float3 s, e;
			s = m_planeInfo[i].center;
			e = s + (m_planeInfo[i].normal * RENDER_NORMAL_LENGTH);

			NDebug::RenderLine(s, e, float4(0, 1, 1, 1));

			// ワイヤーフレーム
			s = m_planeInfo[i].p0;
			e = m_planeInfo[i].p1;
			NDebug::RenderLine(s, e, float4(1, 0, 0, 1));

			s = m_planeInfo[i].p1;
			e = m_planeInfo[i].p2;
			NDebug::RenderLine(s, e, float4(1, 0, 0, 1));

			s = m_planeInfo[i].p2;
			e = m_planeInfo[i].p0;
			NDebug::RenderLine(s, e, float4(1, 0, 0, 1));

			//// 各辺へのベクトル
			//s = m_planeInfo[i].center;
			//e = s + m_planeInfo[i].vecPTo01Vert;
			//NMesh::SetLineColor(float4(1, 0, 0, 1));
			//NMesh::RenderLine(s, e);

			//s = m_planeInfo[i].center;
			//e = s + m_planeInfo[i].vecPTo12Vert;
			//NMesh::SetLineColor(float4(1, 0, 0, 1));
			//NMesh::RenderLine(s, e);

			//s = m_planeInfo[i].center;
			//e = s + m_planeInfo[i].vecPTo20Vert;
			//NMesh::SetLineColor(float4(1, 0, 0, 1));
			//NMesh::RenderLine(s, e);
		}
	}

	bool CMesh::SearchMesh(
		float3&			_dest,
		const float3&	_pos,
		const float3&	_dir, 
		SPlaneInfo&		_rDestPlaneInfo)
	{
		float3 pos = _pos;
		float3 offsetDir = _dir;

		float3 dir = _dir;
		float3 nearCrossPos(0, 0, 0);

		bool isFind = false;	// ポリゴンが見つかったかどうか

		// 全ポリゴン数分ループ
		for (uint i = 0; i < m_faceCount; i++) {

			bool	sts = false;
			float3	curCrossPos(0, 0, 0);

			// 平面と直線の交点を求める
			sts = NMath::CalcLineAndPlaneCross(
				m_planeInfo[i].plane,
				pos,
				dir,
				curCrossPos);

			// 衝突がなければ無視
			if (!sts)
				continue;

			// 交点が３角形の内部にあるか？
			sts = NCollision::CheckPointAndTriangle(
				m_planeInfo[i].p0,
				m_planeInfo[i].p1,
				m_planeInfo[i].p2,
				curCrossPos);

			// 衝突がなければ無視
			if (!sts)
				continue;

			// 交点が以前取得したものより近いか判断
			if (!isFind) {					// 初めて衝突した場合
				// 衝突した点がdirより長い場合は無視
				float3 curVec = curCrossPos - pos;
				if (curVec.GetLength() > dir.GetLength())
					continue;

				nearCrossPos = curCrossPos;	// そのまま代入
				isFind = true;
			}
			else {
				float3	nearCrossVec = nearCrossPos - _pos;
				float	nearLen = nearCrossVec.GetLength();

				float3	curCrossVec = curCrossPos - _pos;
				float	curLen = curCrossVec.GetLength();

				// より近ければ、最短点を更新
				if (curLen < nearLen) {
					nearCrossPos = curCrossPos;
				}
			}

			_rDestPlaneInfo = m_planeInfo[i];
			break;
		}

		if (isFind) {
			_dest = nearCrossPos;
		}

		return isFind;
	}

	bool CMesh::SearchMesh(
		float3&			_destCross,
		const float3&	_posStart,
		const float3&	_posEnd,
		SPlaneInfo*		_pDestPlaneInfo)
	{
		bool sts = false;
		NMath::SVector3	vecStartToEnd = _posEnd - _posStart;
		NMath::SVector3	dir = vecStartToEnd;
		dir.Normalize();
		SPlaneInfo		destPlaneInfo;
		float3			destCross;
		// 衝突を検知
		sts = SearchMesh(
			destCross,
			_posStart,
			dir,
			destPlaneInfo);

		// 衝突がない場合は無視
		if (!sts) {
			return false;
		}

		// 最短点へのベクトルを取得
		NMath::SVector3	vecStartToCross = destCross - _posStart;

		// 距離を判断して、線分との衝突を検知
		float vecStartToEndLenSq = vecStartToEnd.GetLengthSquare();
		float vecStartToCrossLenSq = vecStartToCross.GetLengthSquare();

		// 線分の長さが交差点への長さよ長い場合は交差している
		sts = (vecStartToEndLenSq > vecStartToCrossLenSq);

		if (!sts) {
			return false;
		}

		// 取得先の変数に代入
		_destCross = destCross;
		if (_pDestPlaneInfo != nullptr) {
			*_pDestPlaneInfo = destPlaneInfo;
		}
		return true;
	}

	bool CMesh::SerchGroundOrigToDirection(
		float3&			_destCross,
		const float3&	_orig,
		SPlaneInfo*		_pDestPlaneInfo)
	{
		const float3			orig		= _orig;
		const NMath::SVector3	dir			= float3(0, -1, 0);	// 真下方向に例を飛ばす
		float3					nearCross;
		SPlaneInfo				nearCrossPlane;
		bool					isFind		= false;

		// すべてのポリゴンを探索
		for (uint i = 0; i < m_faceCount; ++i) {
			const NCollision::SPlaneInfo& planeInfo = m_planeInfo[i];
			bool	sts = false;	// 汎用状態判断変数
			float3	curCross;		// 面との交点

			// 平面と直線の交点を求める
			sts = NMath::CalcLineAndPlaneCross(
				planeInfo.plane,
				orig,
				dir,
				curCross);

			// 衝突がなければ無視
			if (!sts) { continue; }

			// 取得した座標がレイを飛ばした方向にあるかを確認
			const NMath::SVector3	vecOrigToCross = curCross - orig;
			const float dot = NMath::Dot(dir, vecOrigToCross);
			
			// 反対方向を向いている場合は無視
			if (dot < FLT_EPSILON) { continue; }

			// 交点が面の内部にあるかを判断
			sts = NCollision::CheckPointAndTriangle(
				planeInfo.p0,
				planeInfo.p1,
				planeInfo.p2,
				curCross);

			// 面の内部でなければ無視
			if (!sts) { continue; }

			// 最短衝突点を更新
			if (isFind) {
				// 現在の最短点との距離と取得した交点との距離を比較し、最短点を更新する
				const NMath::SVector3	vecOrigToNearCross = nearCross - orig;
				const NMath::SVector3	vecOrigToCurCross = curCross - orig;

				float lenSqOrigToNearCross = vecOrigToNearCross.GetLengthSquare();
				float lenSqOrigToCurCross = vecOrigToCurCross.GetLengthSquare();

				// 現在の交点のほうが短い場合は最短交差点を更新
				if (lenSqOrigToCurCross < lenSqOrigToNearCross) {
					nearCross = curCross;
					nearCrossPlane = planeInfo;
				}
			}
			// 最初の見つけた場合は最短点として取得
			else {
				isFind = true;

				// 最短点として設定
				nearCross = curCross;
				nearCrossPlane = planeInfo;
			}
		}

		// 交点が取得できなかった場合は結果のみ戻す
		if (!isFind) { return false; }

		// 交点が求まった場合は値を取得する
		_destCross = nearCross;
		if (_pDestPlaneInfo != nullptr) {
			*_pDestPlaneInfo = nearCrossPlane;
		}
		return true;
	}

	bool CMesh::SerchGroundOrigToDirection(
		float3&			_destCross,
		const float3&	_orig,
		const float		_lenSq,
		SPlaneInfo*		_pDestPlaneInfo)
	{
		float3		cross;
		SPlaneInfo	planeInfo;
		// 真下方向の交点を取得する
		bool sts = SerchGroundOrigToDirection(
			cross,
			_orig,
			&planeInfo);

		// 交点が取得できない場合は結果のみ通知
		if (!sts) { return false; }

		// 交点との長さを取得し、範囲内化を判断
		NMath::SVector3	vecOrigToCross = cross - _orig;
		float lenSqCross = vecOrigToCross.GetLengthSquare();

		// 長さが範囲内かどうかを取得
		sts = lenSqCross <= _lenSq;

		// 範囲外の場合は結果のみ通知
		if (!sts) { return false; }

		// 交点が求まった場合は値を取得
		_destCross = cross;
		if (_pDestPlaneInfo != nullptr) {
			*_pDestPlaneInfo = planeInfo;
		}
		return true;
	}

	bool CMesh::SearchGround(
		const float3&	_rCurPos,
		float3&			_rDestCrossPos,
		SPlaneInfo*		_pDestPlaneInfo)
	{
		float3 dir(0, 1, 0);
		float3 crossPos(0, 0, 0);
		float3 curPos = _rCurPos;
		curPos.y = 0;

		bool isFind = false;								// ポリゴンが見つかったかどうか
		bool status = false;

		// 全ポリゴン数分ループ
		for (uint i = 0; i < m_faceCount; i++) {

			// 平面と直線の交点を求める
			status = NMath::CalcLineAndPlaneCross(
				m_planeInfo[i].plane,
				curPos,
				dir,
				crossPos);

			// 衝突がなければ無視
			if (!status)
				continue;
			
			// 交点が３角形の内部にあるか？
			status = NCollision::CheckPointAndTriangle(
				m_planeInfo[i].p0,
				m_planeInfo[i].p1,
				m_planeInfo[i].p2,
				crossPos);

			// 衝突がなければ無視
			if (!status)
				continue;
			
			curPos.y = crossPos.y;
			isFind = true;
			if (_pDestPlaneInfo != nullptr) {
				*_pDestPlaneInfo = m_planeInfo[i];
			}
			break;
		}

		if (isFind) {
			_rDestCrossPos = curPos;
			return true;
		}
		else {
			_rDestCrossPos = float3(0, 0, 0);
			return false;
		}
	}
}

namespace {

	void CreatMeshPlaneInfo(
		std::vector<NCollision::SPlaneInfo>&	_rPlaneInfo,
		uint&									_rFaceCount,
		NModel::CFbxLoader&						_rModel)
	{
		using VertexType = NModel::SMeshNode::VertexType;
		using IndexType = NModel::SMeshNode::IndexType;

		int	idx0, idx1, idx2;				// ３角形ポリゴンの頂点インデックス格納用

		int	allFaceCount = 0;	// 全メッシュノードのポリゴン数

		// 全てのメッシュ分ループ
		for (uint i = 0; i < _rModel.GetMeshNodeCount(); ++i) {
			auto meshNode = _rModel.GetMeshNode(i);

			// メッシュのポリゴン数を取得
			const uint	faceCount = meshNode.polygonCount;

			// 合計を求めていく
			allFaceCount += faceCount;

			// 面情報を作成する
			NCollision::SPlaneInfo	planeInfo;

			// ポリゴン数分ループ
			uint	index = 0;
			for (uint i = 0; i < faceCount; i++) {
				idx0 = meshNode.indexes[index++];			// ３角形ポリゴンの頂点インデックス取得（０番目）
				idx1 = meshNode.indexes[index++];			// ３角形ポリゴンの頂点インデックス取得（１番目）
				idx2 = meshNode.indexes[index++];			// ３角形ポリゴンの頂点インデックス取得（２番目）

				float3	pos0(
					StaticCast<float>(meshNode.positions[idx0][0]),
					StaticCast<float>(meshNode.positions[idx0][1]),
					StaticCast<float>(meshNode.positions[idx0][2]));
				float3	pos1(
					StaticCast<float>(meshNode.positions[idx1][0]), 
					StaticCast<float>(meshNode.positions[idx1][1]), 
					StaticCast<float>(meshNode.positions[idx1][2]));
				float3	pos2(
					StaticCast<float>(meshNode.positions[idx2][0]),
					StaticCast<float>(meshNode.positions[idx2][1]), 
					StaticCast<float>(meshNode.positions[idx2][2]));

				const NMath::SMatrix44& mat = meshNode.matrix;
				NMath::TransformVector(pos0, mat);
				NMath::TransformVector(pos1, mat);
				NMath::TransformVector(pos2, mat);
	
				// 各頂点座標を取得
				planeInfo.p0 = pos0;
				planeInfo.p1 = pos1;
				planeInfo.p2 = pos2;

				//平面の方程式を作成する
				CreatePlaneInfo(
					planeInfo.plane,
					planeInfo.p0,
					planeInfo.p1,
					planeInfo.p2);

				// 中心(重心)を算出
				planeInfo.center.x = (planeInfo.p0.x + planeInfo.p1.x + planeInfo.p2.x) / 3.0f;
				planeInfo.center.y = (planeInfo.p0.y + planeInfo.p1.y + planeInfo.p2.y) / 3.0f;
				planeInfo.center.z = (planeInfo.p0.z + planeInfo.p1.z + planeInfo.p2.z) / 3.0f;

				// 法線情報を取得
				NMath::CalcNormal(
					planeInfo.normal,
					planeInfo.p0,
					planeInfo.p1,
					planeInfo.p2);

				// 各辺への垂直ベクトルを求める
				std::array<float3, 3>	pos = {
					planeInfo.p0,
					planeInfo.p1,
					planeInfo.p2
				};
				std::array<float3*, 3>	pDestVecVert = {
					&planeInfo.vecPTo01Vert,
					&planeInfo.vecPTo12Vert,
					&planeInfo.vecPTo20Vert
				};
				for (uint idxTri = 0; idxTri < pos.size(); ++idxTri) {
					int idxOrig = idxTri;
					int idxTo = (idxTri + 1) % pos.size();

					NMath::SVector3	vec = pos[idxTo] - pos[idxOrig];
					float3	nearPoint;
					NMath::CalcPointAndLineNearestPoint(
						nearPoint,
						planeInfo.center,
						pos[idxOrig],
						pos[idxTo]);

					*pDestVecVert[idxTri] = nearPoint - planeInfo.center;
				}

				// 作成した面情報を格納
				_rPlaneInfo.emplace_back(planeInfo);
			}
		}

		// 面数を取得
		_rFaceCount = allFaceCount;
	}
}
