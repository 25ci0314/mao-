//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include "randomLib.h"
#include  "Task_Ending.h"
#include "Task_Board.h"
#include "sound.h"
#include "Task_Player.h"
#include "BChara.h"
#include "Task_UI.h"
#include "Task_Enemy.h"
#include "Task_AI.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		bgm::LoadFile("normal_bgm", "./data/sound/bgm/ゆるくておそい天国と地獄.mp3");
		this->imgSU = DG::Image::Create("./data/image/数文字素材02.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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

		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("HG丸ｺﾞｼｯｸM-PRO", 30, 30);



		this->time =this->limit; // 3分
		auto ui = UI::Object::Create(true);
		ui->pos = ML::Vec2(1000, 500);

		//迷路マップ生成
		auto board = Board::Object::Create(true);

		//魔王プレイヤー生成
		auto player = Player::Object::Create(true);

		//勇者生成
		auto Enemy = Enemy::Object::Create(true);

		//勇者AI生成
		auto AI = AI::Object::Create(true);

		//ゲーム終了フラグ
		this->gameEnd = false;

		//★タスクの生成


		this->gameEnd = false;

		bgm::Play("normal_bgm");

		return true;

	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("本編");
		ge->KillAll_G("UI");


		bgm::Stop("normal_bgm");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto next = Ending::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		//残り時間計測
		this->time--;
		//残り時間渡し
		auto ui = ge->GetTask<UI::Object>(UI::defGroupName, UI::defName);
		ui->time = this->time / 60;
		if (this->time == 0) {
			BChara::winner = 1;
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));
		}
		if (inp.S1.down) {
			BChara::winner = 0;
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));
		}
		//---------------------------------------------------------------
		//フェードが終了したらゲームを終了
		if (ge->getCounterFlag("test") == ge->LIMIT)
		{
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void Object::Render2D_AF()
	{

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
	Object::Object() {}
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
