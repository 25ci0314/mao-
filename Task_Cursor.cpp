//-------------------------------------------------------------------
//カーソル
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Cursor.h"
#include "Task_Board.h"

namespace Cursor
{
	Resource::WP Resource::instance;

	//-------------------------------------------------------------------
	//リソースの初期化
	bool Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/cursor.png");
		return true;
	}

	//-------------------------------------------------------------------
	//リソースの解放
	bool Resource::Finalize()
	{
		this->img.reset();
		return true;
	}

	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);

		//リソースクラス生成orリソース共有
		this->res = Resource::Create();
		//★データ初期化
		this->render2D_Priority[1] = 0.8f;

		this->boardX = 0;
		this->boardY = 0;

		//★タスクの生成


		return true;
	}

	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void Object::UpDate()
	{
		auto ms = ge->mouse->GetState();

		const int boardLeft = 270;
		const int boardTop = 40;
		const int margin = 16;
		const int cellSize = 64;
		const int boardArea = cellSize * 9; // 576

		int mx = ms.pos.x - (boardLeft + margin);
		int my = ms.pos.y - (boardTop + margin);

		if (mx >= 0 && mx < boardArea &&
			my >= 0 && my < boardArea)
		{
			this->boardX = mx / cellSize;
			this->boardY = my / cellSize;
		}

		
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void Object::Render2D_AF()
	{
		const int boardLeft = 270;
		const int boardTop = 40;
		const int margin = 16;
		const int cellSize = 64;

		const int cursorSize = 58;

		int drawX = boardLeft + margin + this->boardX * cellSize + 3;
		int drawY = boardTop + margin + this->boardY * cellSize ;

		ML::Box2D draw(drawX, drawY, cursorSize, cursorSize);
		ML::Box2D src(0, 0, 64, 64);

		this->res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP Object::Create(bool flagGameEnginePushBack_)
	{
		Object::SP ob = Object::SP(new Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);
			}
			if (!ob->B_Initialize()) {
				ob->Kill();
			}
			return ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool Object::B_Initialize()
	{
		return this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool Object::B_Finalize()
	{
		auto rtv = this->Finalize();
		return rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }

}