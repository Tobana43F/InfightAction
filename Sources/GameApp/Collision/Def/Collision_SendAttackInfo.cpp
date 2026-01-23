//!	@note	実装中止、ヘッダ参照

/*

#include	"Collision_SendAttackInfo.h"

namespace NCollision {

	CSendAttackInfo::CSendAttackInfo()
	{

	}

	CSendAttackInfo::~CSendAttackInfo()
	{

	}

	void CSendAttackInfo::Init()
	{
		m_isEnable = false;

		// ほかの値はm_isEnableがfalseの間は使用しないので
		// 初期化は不要な処理になるため行わない

		// 念のためポインタは初期化しておく
		m_pOwner = nullptr;
	}


	void CSendAttackInfo::SetData(
		const CProc::SCollideData&	_rAttackColData,
		const CAttackTypeData&		_rAttackData,
		const CProc::SCollideData&	_rDamageColData,
		const CDamageTypeData&		_rDamageData)
	{
		// データがセットされたので有効状態にする
		m_isEnable = true;

		//----------------------------------------------------------
		// 当たり判定を特定する情報を取得する
		//----------------------------------------------------------
		m_pOwner = _rAttackColData.pRefOwner;
		m_attackSequenceID = _rAttackColData.sequenceID;
		m_attackGroupID = _rAttackColData.groupID;
		m_attackSerialID = _rAttackData.GetSerialID();

		// 攻撃情報を取得する
		m_receiveAttackData = _rAttackData;

		//----------------------------------------------------------
		// その他データの作成
		//----------------------------------------------------------

		//---- 相手の位置と向きを取得
		_rAttackColData.pRefOwner->GetTransform().CalcWorldMatrix(m_attackOwnerDir);
		m_attackOwnerPos = m_attackOwnerDir.vec[3];
		m_attackOwnerDir.vec[3] = float4(0, 0, 0, 1);	// 向きだけの値にする

														//---- 衝突位置を取得(OBB同士の)
		float3 myPos = _rDamageColData.pRefGame->GetOBB().GetOBBox().centerPos;
		float3 attakerPos = _rAttackColData.pRefGame->GetOBB().GetOBBox().centerPos;

		// 中央値を衝突点とする(仮、違和感がなければそのままでOK)
		m_collidePos = (myPos + attakerPos) / 2.0f;

		// 自分の位置を取得
		_rDamageColData.pRefOwner->GetTransform().CalcWorldMatrix(m_myDir);
		m_myPos = m_myDir.vec[3];
		m_myDir.vec[3] = float4(0, 0, 0, 1);	// 向きだけの値にする

												//---- ノックバック方向をタイプによって変換する
		switch (m_receiveAttackData.GetKnockBackType()) {
			case CAttackTypeData::EKnockBackType::RADIATE:
			{
				// 攻撃情報のノックバック方向を無視する
				NMath::SVector3	dir = m_myPos - m_attackOwnerPos;	// 相手から見た方向を取得
				dir.Normalize();
				if (dir.y < 0) {
					dir.y = 0;
				}
				m_knockBackDir = dir;
				break;
			}
			case CAttackTypeData::EKnockBackType::DIRECTIONAL:
			{
				m_knockBackDir =
					m_attackOwnerDir.vec[2] * _rAttackData.GetKnockBackDir().z +
					float3(0, 1, 0)  * _rAttackData.GetKnockBackDir().y +
					m_attackOwnerDir.vec[0] * _rAttackData.GetKnockBackDir().x;
				break;
			}
			case CAttackTypeData::EKnockBackType::ABSOLUTE_DIR:
			{
				// 取得した値をそのまま使用する
				m_knockBackDir = _rAttackData.GetKnockBackDir();
				break;
			}
		}
	}
}

*/