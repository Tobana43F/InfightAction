#include	"Game_ActorParam.h"

namespace NGame {

	//----------------------------------------------------------
	// SActorParam
	//----------------------------------------------------------

	SActorParam::SActorParam() :
		life(0),
		maxLife(0),
		isEliminated(false)
	{
	
	}

	void SActorParam::Setup(NAsset::CJsonFactory::Key _key)
	{
		// データ読み込み
		try {
			NAsset::CJsonFactory::SPAsset pJson;
			pJson = NAsset::CJsonFactory::GetInst().Load(_key);

			// アクセス省略
			auto& json = *pJson;
			maxLife = json["MaxLife"].int_value();
			if (maxLife <= 0) {
				throw("MaxLifeには0以上の値を設定してください。");
			}

			// 初期値として設定
			life = maxLife;
		}
		catch (const char* _err) {
			std::string	err = "";
			err += "ActorMoveのSetupでエラーが起きました。\n";
			err += "FilePath : ";
			err += _key.data();
			err += "\n\n [Message]\n";
			err += _err;
		}
	}

	void SActorParam::Init()
	{
		life = maxLife;
		isEliminated = false;
	}

	//----------------------------------------------------------
	// 
	//----------------------------------------------------------
}