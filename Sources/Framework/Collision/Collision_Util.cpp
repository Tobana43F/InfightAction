#include	"Collision_Util.h"

#include	<vector>
#include	"Debug\Debug_Assert.h"
#include	"Math\Math_Common.h"
#include	"Math\Math_Util.h"
#include	"Model\Model_Manager.h"
#include	"Model\Model_Object.h"

namespace {

	//!	@brief	固有ベクトルを求めるときにループする回数
	const uint		EIGEN_ROOP_COUNT	= 100;
	const float		EIGEN_TOL			= 1.0e-10;
	const uint		EIGEN_N				= 4;


	//!	@brief		ボーンに影響する頂点座標をコンテナに追加する
	//!	@note		指定のノード内の指定のボーンに影響する頂点座標をすべて取得し、追加する
	//!	@param[in]	_rDestPositions		頂点座標の格納先(push_backで追加する。)
	void AddBoneInfuluenceVertexPos(
		std::vector<float3>&			_rDestPositions,
		const NModel::SFbxMeshNode&		_rMeshNode,
		const uint						_boneIndex);

	//!	@brief		共分散行列を作成する
	//!	@param[in]	_rDestMat		共分散行列の格納先
	//!	@param[in]	_rSrcPositions	ボーンに影響する頂点群
	void MakeConvarianceMatrix(
		NMath::SMatrix44&			_rDestMat,
		const std::vector<float3>&	_rSrcPositions);

	//!	@brief		固有ベクトルを求める
	//!	@param[in]	_rDestMat			各軸の固有ベクトルの格納先
	//!	@param[in]	_rMatConvariance	共分散行列
	void MakeEigenVector(
		NMath::SMatrix44&		_rDestMat,
		const NMath::SMatrix44&	_rMatConvariance);

	//!	@brief		ボーンに影響する頂点を内包するOBBを作成する
	//!	@param[in]	_rDest				作成したOBB情報の格納先
	//!	@param[in]	_rSrcPositions	ボーンに影響する頂点群
	//!	@param[in]	_rMatEigenVector	各軸の固有ベクトルを格納した行列
	void CreateBoneOBBData(
		NCollision::SOBBox&			_rDest,
		const std::vector<float3>&	_rSrcPositions,
		const NMath::SMatrix44&		_rMatEigenVector,
		const NMath	::SVector3&		_rClusterPos);
}

namespace NCollision {

	void CreatePlaneInfo(
		SPlane&			_dest,
		const float3&	_p0,
		const float3&	_p1,
		const float3&	_p2)
	{
		NMath::SVector3	normal;
		NMath::CalcNormal(normal, _p0, _p1, _p2);

		_dest.a = normal.x;
		_dest.b = normal.y;
		_dest.c = normal.z;

		_dest.d = -(_dest.a * _p0.x + _dest.b * _p0.y + _dest.c * _p0.z);
	}
	

	void CreateBoneOBB(
		NCollision::SOBBox&			_rDestOBB,
		const NModel::CObject&		_rRefModel,
		const uint					_boneIndex)
	{
		const auto& rModel = _rRefModel.GetModelManager()->GetModel();

		Assert(rModel.GetMeshNodeCount() > 0 && "メッシュノードが存在しません。");
		Assert(_boneIndex < rModel.GetMeshNode(0).clusterCount && "指定されたボーンインデックスが不正です。");

		// 全てのノード内の指定のボーンに影響する頂点を求める
		std::vector<float3> vertexes;	//	ボーンに影響するすべての頂点座標の格納先
		vertexes.clear();

		// 全メッシュノード分ループ
		uint nodeCount = rModel.GetMeshNodeCount();
		for (uint i = 0; i < nodeCount; ++i) {
			const NModel::SFbxMeshNode& rFbxMeshNode = rModel.GetMeshNode(i);
			// 頂点を追加
			AddBoneInfuluenceVertexPos(
				vertexes,
				rFbxMeshNode,
				_boneIndex);
		}
		if (vertexes.size() <= 0) {
			std::string err = "ボーンに影響数頂点が存在しません。\n";
			err += "指定されたボーンインデックス : ";
			err += std::to_string(_boneIndex);
			throw(err.data());
		}

		// ボーンの座標を取得(正しい)
		FbxAMatrix matFbx;
		rModel.GetMeshNode(0).pFbxSkin->GetCluster(_boneIndex)->GetTransformLinkMatrix(matFbx);
		float3 clusterPos;
		for (int i = 0; i < 3; ++i) { clusterPos.array[i] = StaticCast<float>(matFbx.Get(3, i)); }
		clusterPos.x *= -1;
		//g_pVec->emplace_back(clusterPos);

		// 共分散行列を作成
		NMath::SMatrix44	matConvariance;
		MakeConvarianceMatrix(
			matConvariance,
			vertexes);

		// 固有ベクトルを作成
		NMath::SMatrix44	matEigen;
		MakeEigenVector(
			matEigen,
			matConvariance);

		// 固有ベクトルをもとにしてOBBを作成
		CreateBoneOBBData(
			_rDestOBB,
			vertexes,
			matEigen,
			clusterPos);
	}
}

namespace {

	// ここでとれる頂点座標は描画されているモデルのものと等しい
	void AddBoneInfuluenceVertexPos(
		std::vector<float3>&			_rDestPositions,
		const NModel::SFbxMeshNode&		_rMeshNode,
		const uint						_boneIndex)
	{
		// 全頂点分ループ
		uint vertexCount = _rMeshNode.optimizedVertex.size();
		for (uint i = 0; i < vertexCount; ++i) {
			const NModel::SFbxVertex& rVertex = _rMeshNode.optimizedVertex[i];

			// ボーンに対して影響値が設定されている場合は、頂点を追加する
			if (rVertex.weights[_boneIndex] > FLT_EPSILON) {
				// 頂点座標を追加
				NMath::SVector3	pos;
				pos.x = StaticCast<float>(rVertex.pos.mData[0]);
				pos.y = StaticCast<float>(rVertex.pos.mData[1]);
				pos.z = StaticCast<float>(rVertex.pos.mData[2]);

				NMath::TransformVector(pos, _rMeshNode.matrix);

				_rDestPositions.emplace_back(pos);
			}
		}
	}


	void MakeConvarianceMatrix(
		NMath::SMatrix44&			_rDestMat,
		const std::vector<float3>&	_rSrcPositions)
	{
		uint positionCount = _rSrcPositions.size();

		// 全ての頂点の合計値を取得
		float3 vertexPosSum(0, 0, 0);	// 全ての頂点座標の合計値
		for (uint i = 0; i < positionCount; ++i) {
			vertexPosSum += _rSrcPositions[i];
		}

		// 頂点座標の平均を計算
		const float3 vertexPosAve = vertexPosSum / StaticCast<float>(positionCount);

		// 共分散行列の各成分を求めるう
		for (uint i = 0; i < positionCount; ++i) {
			_rDestMat.e11 += (_rSrcPositions[i].x - vertexPosAve.x) * (_rSrcPositions[i].x - vertexPosAve.x);
			_rDestMat.e22 += (_rSrcPositions[i].y - vertexPosAve.y) * (_rSrcPositions[i].y - vertexPosAve.y);
			_rDestMat.e33 += (_rSrcPositions[i].z - vertexPosAve.z) * (_rSrcPositions[i].z - vertexPosAve.z);
			_rDestMat.e12 += (_rSrcPositions[i].x - vertexPosAve.x) * (_rSrcPositions[i].y - vertexPosAve.y);
			_rDestMat.e13 += (_rSrcPositions[i].x - vertexPosAve.x) * (_rSrcPositions[i].z - vertexPosAve.z);
			_rDestMat.e23 += (_rSrcPositions[i].y - vertexPosAve.y) * (_rSrcPositions[i].z - vertexPosAve.z);
		}

		_rDestMat.e11 /= positionCount;
		_rDestMat.e22 /= positionCount;
		_rDestMat.e33 /= positionCount;
		_rDestMat.e12 /= positionCount;
		_rDestMat.e13 /= positionCount;
		_rDestMat.e23 /= positionCount;
		_rDestMat.e21 = _rDestMat.e12;
		_rDestMat.e31 = _rDestMat.e13;
		_rDestMat.e32 = _rDestMat.e23;
	}

	void MakeEigenVector(
		NMath::SMatrix44&		_rDestMat,
		const NMath::SMatrix44&	_rMatConvariance)
	{
		int		i, j, k, m;
		double	amax, amax0, theta, co, si, co2, si2, cosi, pi = 4.0 * atan(1.0);
		double	aii, aij, ajj, aik, ajk;
		bool	result = false;

		// 初期化
		NMath::SMatrix44	matConvariance = _rMatConvariance;
		_rDestMat.SetIdentity();

		uint count = 0;
		while (count <= EIGEN_ROOP_COUNT) {
			//非対角要素の最大値を探索
			amax = 0.0;
			for (k = 0; k < EIGEN_N - 1; k++) {
				for (m = k + 1; m < EIGEN_N; m++) {
					//				amax0 = fabs(*(a.m[EIGEN_N*k+m]));			// おかしい
					float temp = fabs(matConvariance.flt[k][m]);		// ダブル型としてみていたので修正
					amax0 = temp;
					if (amax0 > amax) {
						i = k;
						j = m;
						amax = amax0;
					}
				}
			}

			//収束判定
			if (amax <= EIGEN_TOL) {
				result = true;
				break;
			}
			else {
				aii = matConvariance.flt[i][i];
				aij = matConvariance.flt[i][j];
				ajj = matConvariance.flt[j][j];
				//回転角度計算
				if (fabs(aii - ajj) < EIGEN_TOL) {
					theta = 0.25 * pi * aij / fabs(aij);
				}
				else {
					theta = 0.5 * atan(2.0 * aij / (aii - ajj));
				}
				co = cos(theta);
				si = sin(theta);
				co2 = co * co;
				si2 = si * si;
				cosi = co * si;

				//相似変換行列
				matConvariance.flt[i][i] = (float)(co2 * aii + 2.0 * cosi * aij + si2 * ajj);
				matConvariance.flt[j][j] = (float)(si2 * aii - 2.0 * cosi * aij + co2 * ajj);
				matConvariance.flt[i][j] = 0.0f;
				matConvariance.flt[j][i] = 0.0f;

				for (k = 0; k < EIGEN_N; k++) {
					if (k != i && k != j) {
						aik = matConvariance.flt[k][i];
						ajk = matConvariance.flt[k][j];
						matConvariance.flt[k][i] = (float)(co * aik + si * ajk);
						matConvariance.flt[i][k] = (float)(matConvariance.flt[k][i]);
						matConvariance.flt[k][j] = (float)(-si * aik + co * ajk);
						matConvariance.flt[j][k] = (float)(matConvariance.flt[k][j]);
					}
				}

				//固有ベクトル
				for (k = 0; k < EIGEN_N; k++) {
					aik = _rDestMat.flt[k][i];
					ajk = _rDestMat.flt[k][j];
					_rDestMat.flt[k][i] = (float)(co * aik + si* ajk);
					_rDestMat.flt[k][j] = (float)(-si * aik + co * ajk);
				}
				count++;
			}
		}
	}

	void CreateBoneOBBData(
		NCollision::SOBBox&			_rDest,
		const std::vector<float3>&	_rSrcPositions,
		const NMath::SMatrix44&		_rMatEigenVector,
		const NMath::SVector3&		_rClusterPos)
	{
		NMath::SVector3 rvec, svec, tvec, vertexvec, center;
		float rmin = 0.0, rmax = 0.0, smin = 0.0, smax = 0.0, tmin = 0.0, tmax = 0.0;
		float dot;
		NCollision::SOBBox resultOBB;

		//固有ベクトルが格納された行列を各ベクトルへ代入し正規化
		rvec.x = _rMatEigenVector.e11; rvec.y = _rMatEigenVector.e21; rvec.z = _rMatEigenVector.e31;
		svec.x = _rMatEigenVector.e12; svec.y = _rMatEigenVector.e22; svec.z = _rMatEigenVector.e32;
		tvec.x = _rMatEigenVector.e13; tvec.y = _rMatEigenVector.e23; tvec.z = _rMatEigenVector.e33;

		rvec.Normalize();
		svec.Normalize();
		tvec.Normalize();

		//各ベクトルとすべての頂点の内積から最小値と最大値を求める
		uint vertexCount = _rSrcPositions.size();
		for (uint i = 0; i < vertexCount; i++) {
			vertexvec.x = _rSrcPositions[i].x; 
			vertexvec.y = _rSrcPositions[i].y;
			vertexvec.z = _rSrcPositions[i].z;

			dot = NMath::Dot(rvec, vertexvec);	 	// Rベクトルに射影
			if (i == 0 || dot < rmin) rmin = dot;	// 最小値を求める
			if (i == 0 || dot > rmax) rmax = dot;	// 最大値を求める

			dot = NMath::Dot(svec, vertexvec);	 	// Sベクトルに射影
			if (i == 0 || dot < smin) smin = dot;	// 最小値を求める
			if (i == 0 || dot > smax) smax = dot;	// 最大値を求める

			dot = NMath::Dot(tvec, vertexvec);	 	// Tベクトルに射影
			if (i == 0 || dot < tmin) tmin = dot;	// 最小値を求める
			if (i == 0 || dot > tmax) tmax = dot;	// 最大値を求める
		}

		//BBOX構造体に値を格納
		// 中心座標を求める(正しい)
		center.x = (rmax + rmin) / 2.0f;	
		center.y = (smax + smin) / 2.0f;	
		center.z = (tmax + tmin) / 2.0f;			// 原点からの距離

		resultOBB.centerPos.x = rvec.x * center.x + svec.x * center.y + tvec.x * center.z;
		resultOBB.centerPos.y = rvec.y * center.x + svec.y * center.y + tvec.y * center.z;
		resultOBB.centerPos.z = rvec.z * center.x + svec.z * center.y + tvec.z * center.z;
		//g_pVec->emplace_back(resultOBB.centerPos);

		// クラスターからの相対座標に変換
		resultOBB.centerPos -= _rClusterPos;
		//g_pVec->emplace_back(resultOBB.centerPos);

		resultOBB.size.x = (rmax - rmin); 
		resultOBB.size.y = (smax - smin);
		resultOBB.size.z = (tmax - tmin);		// BBOXの幅、高さ、奥行

		resultOBB.axisDir.SetRow(0, rvec);
		resultOBB.axisDir.SetRow(1, svec);
		resultOBB.axisDir.SetRow(2, tvec);
		resultOBB.axisDir.SetRow(3, float4(0, 0, 0, 1));

		_rDest = resultOBB;
	}
}
