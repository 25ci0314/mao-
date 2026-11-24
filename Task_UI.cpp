//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_UI.h"
#include  "Task_Game.h"

namespace  UI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->font = DG::Image::Create("./data/image/pixel_font.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->font.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		this->Timer_Render(this->tPos.x, this->tPos.y, this->time);
	}

	BChara::DrawInfo Object::font_ward(int ward_)
	{
		ML::Color  defColor(1, 1, 1, 1);
		int size = 2;
		BChara::DrawInfo ward[] = {
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 0,9 * 0,8,9),defColor}
		};
		return BChara::DrawInfo();
	}

	BChara::DrawInfo Object::font_number(int num_) {
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo number[] = {
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 0,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 1,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 2,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 3,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 4,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 5,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 6,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 7,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 8,9 * 1,8,9),defColor},
			{ML::Box2D(-4 * size,-4 * size,8 * size,9 * size),ML::Box2D(8 * 9,9 * 1,8,9),defColor},
		};
		return BChara::DrawInfo(number[num_]);
	}

	void Object::Timer_Render(int px_, int py_, int time_) {
		int h, t, w;
		{
			h = time_ / 100;
			BChara::DrawInfo  dh = this->font_number(h);
			dh.draw.Offset(px_, py_);
			this->res->font->Draw(dh.draw, dh.src);
		}

		{
			t = (time_ % 100) / 10;
			BChara::DrawInfo dt = this->font_number(t);
			dt.draw.Offset(px_ + (this->size * 8), py_);
			this->res->font->Draw(dt.draw, dt.src);
		}

		{
			w = time_ % 10;
			BChara::DrawInfo dw = this->font_number(w);
			dw.draw.Offset(px_ + (this->size * 8 * 2), py_);
			this->res->font->Draw(dw.draw, dw.src);
		}

	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
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