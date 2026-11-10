#pragma warning(disable:4996)
#pragma once
#include "GameEngine_Ver3_83.h"
//-------------------------------------------------------------------
//AI
//-------------------------------------------------------------------

namespace AI
{
	//タスクに割り当てるグループ名と固有名
	const string defGroupName("本編");
	const string defName("AI");

	//-------------------------------------------------------------------
	class Object : public BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual ~Object();

		typedef shared_ptr<Object> SP;
		typedef weak_ptr<Object> WP;

		static Object::SP Create(bool flagGameEnginePushBack_);

	private:
		Object();

		bool B_Initialize();
		bool B_Finalize();

		bool Initialize();
		void UpDate() override;
		bool Finalize();

		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:

		//AI思考
		void Think();

		//動かす駒を決める
		int SelectKoma();

		//移動先を決める
		int SelectMove();

		//思考時間
		int thinkCnt;
	};
}