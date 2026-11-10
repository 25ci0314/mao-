#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//カーソル
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Cursor
{
	//タスクに割り当てるグループ名と固有名
	const string defGroupName("本編");
	const string defName("カーソル");

	//-------------------------------------------------------------------
	class Resource : public BResource
	{
		bool Initialize() override;
		bool Finalize() override;
		Resource();

	public:
		~Resource();

		typedef shared_ptr<Resource> SP;
		typedef weak_ptr<Resource> WP;

		static WP instance;
		static Resource::SP Create();

		//共有する変数はここに追加する
		DG::Image::SP img;
	};

	//-------------------------------------------------------------------
	class Object : public BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual ~Object();

		typedef shared_ptr<Object> SP;
		typedef weak_ptr<Object> WP;

		static Object::SP Create(bool flagGameEnginePushBack_);

		Resource::SP res;

	private:
		Object();

		bool B_Initialize();
		bool B_Finalize();

		bool Initialize();
		void UpDate() override;
		void Render2D_AF() override;
		bool Finalize();

		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:

		int boardX;
		int boardY;

	};
}

