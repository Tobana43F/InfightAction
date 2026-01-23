#include	"Collision_Proc.h"

#include	"Collision\Collision_Common.h"
#include	"Debug\Debug_Assert.h"
#include	"Object\Object_GameObject.h"
	
namespace NCollision {

	CProc::SCollideData::SCollideData(
		uint		_type,
		CHolder*	_pRefHolder,
		CSequence*	_pRefSequence,
		CGroup*		_pRefGroup,
		CGame*		_pRefGame)
	{
		AssertNotNull(_pRefHolder);
		AssertNotNull(_pRefHolder->GetOwner());
		AssertNotNull(_pRefSequence);
		AssertNotNull(_pRefGroup);
		AssertNotNull(_pRefGame);

		type = _type;
		pRefHolder = _pRefHolder;
		pRefOwner = _pRefHolder->GetOwner();
		pRefSequence = _pRefSequence;
		pRefGroup = _pRefGroup;
		pRefGame = _pRefGame;

		// それぞれのIDを設定
		sequenceID	= pRefSequence->GetSequenceID();
		groupID		= pRefGroup->GetGroupID();
		collisionID = pRefGame->GetCollisionID();
	}

	//----------------------------------------------------------

	void CProc::Setup()
	{
		Assert(!m_isSetupOK);
		m_isSetupOK = true;

		// 種類の数を設定
		SetupTypeCount();

		// タイプ数分確保
		Assert(m_typeCount >= 2);
		m_typeHolder.resize(m_typeCount);	// タイプの登録先配列を確保

		// 判定マトリックスを確保
		m_collisionMatrix.resize(m_typeCount);
		for (uint i = 0; i < m_typeCount; ++i) {
			m_collisionMatrix[i].resize(m_typeCount);
		}

		// 自分同士以外の判定をとるように初期化
		for (uint i = 0; i < m_typeCount; ++i) {
			for (uint j = 0; j < m_typeCount; ++j) {
				if (i == j) {	// 自分同士
					// 両側の値の整合性を保つ
					m_collisionMatrix[i][j] = false;
					m_collisionMatrix[j][i] = false;
				}
				else {			// それ以外
					// 両側の値の整合性を保つ
					m_collisionMatrix[i][j] = true;
					m_collisionMatrix[j][i] = true;
				}
			}
		}

		// 判定する種類を設定する
		SetupCollisionMatrix();
	}

	//	@note	ネストが深いが、関数に分けるより、全体が見えるほうがいいと思うのでそのままにしておく
	void CProc::Update() {
		Assert(m_isSetupOK);

		// 当たり判定前処理
		ProcBeforeCollide();

		//----------------------------------------------------------
		// 全判定を行う
		//----------------------------------------------------------
		for (uint iTypeHold1 = 0; iTypeHold1 < m_typeHolder.size(); ++iTypeHold1) {
			std::vector<SData>& type1 = m_typeHolder[iTypeHold1];

			for (uint iTypeHold2 = m_typeHolder.size() - 1; iTypeHold2 > iTypeHold1; --iTypeHold2) {
				Assert(iTypeHold2 != UINT_MAX);	// ありえないが念のためアンダーフローをチェック

				std::vector<SData>& type2 = m_typeHolder[iTypeHold2];

				// 判定を行うかどうか判定マトリックスから判断
				if (!GetIsActive(iTypeHold1, iTypeHold2)) {
					continue;
				}

				ProcBeforeTypeCollide();

				//----------------------------------------------------------
				// 種類ごとの判定
				//----------------------------------------------------------
				for (uint iType1 = 0; iType1 < type1.size(); ++iType1) {
					CHolder* pHold1 = type1[iType1].pRefHolder;

					// ホルダーが無効な場合無視
					if (!pHold1->GetIsEnable()) { 
						continue; 
					}

					// 現在のシーケンスを取得
					CSequence& rSeq1 = pHold1->GetCurSequence();

					for (uint iType2 = 0; iType2 < type2.size(); ++iType2) {
						CHolder* pHold2 = type2[iType2].pRefHolder;

						// ホルダーが無効な場合無視
						if (!pHold2->GetIsEnable()) {
							continue;
						}

						// 現在のシーケンスを取得
						CSequence& rSeq2 = pHold2->GetCurSequence();

						ProcBeforeGroupCollide();

						//----------------------------------------------------------
						// 当たり判定グループ
						//----------------------------------------------------------
						for (uint iGroup1 = 0; iGroup1 < rSeq1.GetGroupCount(); ++iGroup1) {
							CGroup&	rGroup1 = rSeq1.GetGroup(iGroup1);

							// グループが有効でない場合は無視
							if (!rGroup1.GetIsEnable()) {
								continue;
							}

							for (uint iGroup2 = 0; iGroup2 < rSeq2.GetGroupCount(); ++iGroup2) {
								CGroup&	rGroup2 = rSeq2.GetGroup(iGroup2);

								// グループが有効でない場合は無視
								if (!rGroup2.GetIsEnable()) {
									continue;
								}

								ProcBeforeCollisionCollide(rGroup1, rGroup2);

								//----------------------------------------------------------
								// 実際の当たり判定
								//----------------------------------------------------------
								for (uint iCol1 = 0; iCol1 < rGroup1.GetCollisionCount(); ++iCol1) {
									CGame& col1 = rGroup1.GetCollision(iCol1);
									if (!col1.GetIsEnable()) {
										continue;
									}
									const SOBBox& rObb1 = col1.GetOBB().GetOBBox();

									for (uint iCol2 = 0; iCol2 < rGroup2.GetCollisionCount(); ++iCol2) {
										CGame& col2 = rGroup2.GetCollision(iCol2);
										if (!col2.GetIsEnable()) {
											continue;
										}
										const SOBBox& rObb2 = col2.GetOBB().GetOBBox();

										// 衝突チェック
										bool isHit = false;
										isHit = CheckOBBAndOBB(rObb1, rObb2);
										
										//----------------------------------------------------------
										// 衝突時
										//----------------------------------------------------------
										if (isHit) {
											// 衝突時の処理に必要な情報を作成
											SCollideData	dataA(
												iTypeHold1,
												pHold1,
												&rSeq1,
												&rGroup1,
												&col1);

											SCollideData	dataB(
												iTypeHold2,
												pHold2,
												&rSeq2,
												&rGroup2,
												&col2);

											ProcCollide(dataA, dataB);

										}// if(isHit)

									} // for(Col2)
								}// for(Col1)

								ProcAfterCollisionCollide(rGroup1, rGroup2);

							}// for(Group2)
						}// for(Group1)

						ProcAfterGroupCollide();

					}// for(Type2)
				}// for(Type1)

				ProcAfterTypeCollide();

			}// for(TypeHold2)
		}// for(TypeHold1)

		ProcAfterCollide();
	};

	//----------------------------------------------------------

	void CProc::Register(
		CHolder& _rHolder,
		const uint _type)
	{
		Assert(m_isSetupOK);

		SData work;
		work.pRefHolder = &_rHolder;
		m_typeHolder[_type].emplace_back(work);
	}

	void CProc::SetCollisionMatrix(
		const uint	_typeA, 
		const uint	_typeB, 
		const bool	_state)
	{
		Assert(m_isSetupOK);
		Assert(_typeA < m_typeCount);
		Assert(_typeB < m_typeCount);

		// 両側の値の整合性を保つ
		m_collisionMatrix[_typeA][_typeB] = _state;
		m_collisionMatrix[_typeB][_typeA] = _state;
	}

	//----------------------------------------------------------
	// タイミングでの当たり判定群

	void CProc::ProcCollide(SCollideData& _rColA, SCollideData& _rColB)
	{
		// グループの衝突を設定
		_rColA.pRefGroup->SetIsHit();
		_rColB.pRefGroup->SetIsHit();

		// 1つの当たり判定の衝突を設定
		_rColA.pRefGame->SetIsHit();
		_rColB.pRefGame->SetIsHit();

		// グループに属するすべての当たり判定の衝突フラグを設定
		uint colCountA = _rColA.pRefGroup->GetCollisionCount();
		for (uint i = 0; i < colCountA; ++i) {
			_rColA.pRefGroup->GetCollision(i).SetIsGroupHit();
		}
		uint colCountB = _rColB.pRefGroup->GetCollisionCount();
		for (uint i = 0; i < colCountB; ++i) {
			_rColB.pRefGroup->GetCollision(i).SetIsGroupHit();
		}
	}
}
