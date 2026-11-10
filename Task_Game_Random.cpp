//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game_Random.h"
#include "randomLib.h"
#include  "Task_Ending.h"
#include "Task_Board.h"
#include "Task_Koma.h"
#include "Task_Cursor.h"
#include "cstdlib"
#include "ctime"
#include "Task_AI.h"
#include "sound.h"
#include "Task_StartTimer.h"


namespace  Game_Random
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		//BGMファイル読み込み
		bgm::LoadFile("random_bgm", "./data/sound/bgm/PeriTune_Michikusa.mp3");
		this->imgSE = DG::Image::Create("./data/image/先02.png");
		this->imgGO = DG::Image::Create("./data/image/後02.png");
		this->imgTE = DG::Image::Create("./data/image/手02.png");
		this->imgNO = DG::Image::Create("./data/image/の02.png");
		this->imgBA = DG::Image::Create("./data/image/番02.png");
		this->imgSU = DG::Image::Create("./data/image/数文字素材02.png");
		this->imgNKR = DG::Image::Create("./data/image/残り02.png");
		this->imgBY = DG::Image::Create("./data/image/秒02.png");
		this->imgRed = DG::Image::Create("./data/image/Red.png"); 
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
		//前のゲームのランダムイベントをリセット
		Koma::Object::timePenalty = 0;
		Koma::Object::destroyColumn = -1;
		Koma::Object::destroyTurn = 0;
		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("HG丸ｺﾞｼｯｸM-PRO", 30, 30);

		this->timeLimit = 20 * 60; // 20秒

		//ゲーム開始フラグ
		this->gameStart = false;
		auto timer = StartTimer::Object::Create(true);
		timer->pos = ML::Vec2(1150 / 2, 650 / 2);

		//ゲーム終了フラグ
		this->gameEnd = false;

		//★タスクの生成
		this->Cnt = 0;

		//将棋盤の生成
		auto board = Board::Object::Create(true);

		
		srand((unsigned)time(NULL));
		//駒配置

		//--------------------------------------------
		//先手と後手の王をランダム配置
		//--------------------------------------------

		//先手の王の位置
		int senteX = rand() % 9;
		int senteY = rand() % 9;

		//後手の王の位置
		int goteX;
		int goteY;

		//王同士が隣接しない位置を選ぶ
		do {
			goteX = rand() % 9;
			goteY = rand() % 9;

		} while (abs(senteX - goteX) <= 1 &&
			abs(senteY - goteY) <= 1);

		//先手の王を配置
		this->CreateKoma(senteX, senteY, Koma::Object::Type::Ou, Koma::Object::Owner::Sente);

		//後手の王を配置
		this->CreateKoma(goteX, goteY, Koma::Object::Type::Ou, Koma::Object::Owner::Gote);

		//--------------------------------------------
		//王以外の駒をランダム配置
		//--------------------------------------------

		for (int y = 0; y < 9; y++) {
			for (int x = 0; x < 9; x++) {

				//王がいるマスには配置しない
				if ((x == senteX && y == senteY) ||
					(x == goteX && y == goteY)) {
					continue;
				}

				//先手の王の周囲8マス
				bool nearSente =
					abs(x - senteX) <= 1 && abs(y - senteY) <= 1;

				//後手の王の周囲8マス
				bool nearGote =
					abs(x - goteX) <= 1 && abs(y - goteY) <= 1;

				//配置する駒の所有者をランダムで決める
				Koma::Object::Owner owner =
					this->RandomSenko();

				//先手の王の周囲に後手の駒を置かない
				if (nearSente &&
					owner == Koma::Object::Owner::Gote) {
					continue;
				}

				//後手の王の周囲に先手の駒を置かない
				if (nearGote &&
					owner == Koma::Object::Owner::Sente) {
					continue;
				}

				//ランダムで駒を配置
				if (rand() % 2 == 0) {
					this->CreateKoma(x, y, Koma::Object::RandomKoma(), owner);
				}
			}
		}
		//カーソルは最後に生成
		auto cursor = Cursor::Object::Create(true);

		//初期状態は1対1
		if (this->cpuBattle == true) {
			AI::Object::Create(true);
		}
			bgm::Play("random_bgm");

		return true;

	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{

		bgm::Stop("random_bgm");

		//★データ＆タスク解放
		ge->KillAll_G("本編");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto next = Ending::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		//スタートタイマータスク取得
		auto timer = ge->GetTask<StartTimer::Object>(
			StartTimer::defGroupName,
			StartTimer::defName);

		//---------------------------------------------------------------
		//ゲーム中の処理
		if (timer == nullptr) {
			this->gameStart = true;
		}

		if (this->gameEnd == false)
		{
			bool senteOu = false;
			bool goteOu = false;
			//スタートタイマー
			if (this->gameStart) {
				//-----------------------------------------------------------
				//盤上に先手・後手の王がいるか確認
				for (int y = 0; y < 9; y++) {
					for (int x = 0; x < 9; x++) {

						auto koma = board->GetKoma(x, y);

						//駒がない
						if (koma == nullptr) {
							continue;
						}

						//王ではない
						if (koma->type != Koma::Object::Type::Ou) {
							continue;
						}

						//先手の王
						if (koma->owner == Koma::Object::Owner::Sente) {
							senteOu = true;
						}

						//後手の王
						if (koma->owner == Koma::Object::Owner::Gote) {
							goteOu = true;
						}
					}
				}

				//-----------------------------------------------------------
				//両方の王がいない場合
				if (senteOu == false && goteOu == false)
				{
					//爆弾を発動した側の勝ち
					BChara::winner = Koma::Object::bombOwner;

					this->gameEnd = true;

					ge->StartCounter("test", 45);
					ge->CreateEffect(99, ML::Vec2(0, 0));
				}
				//先手の王だけがいない
				else if (senteOu == false)
				{
					BChara::winner = 1;
					this->gameEnd = true;

					ge->StartCounter("test", 45);
					ge->CreateEffect(99, ML::Vec2(0, 0));
				}
				//後手の王だけがいない
				else if (goteOu == false)
				{
					BChara::winner = 0;
					this->gameEnd = true;

					ge->StartCounter("test", 45);
					ge->CreateEffect(99, ML::Vec2(0, 0));
				}
				//-----------------------------------------------------------
				//制限時間を減らす
				if (this->gameEnd == false)
				{
					this->timeLimit--;

					//時間切れ
					if (this->timeLimit <= 0)
					{
						this->timeLimit = 0;
						this->gameEnd = true;

						//現在の手番側が時間切れで負け
						if (board->turn == 0)
						{
							BChara::winner = 1;   //後手勝ち
						}
						else
						{
							BChara::winner = 0;   //先手勝ち
						}

						ge->StartCounter("test", 45);
						ge->CreateEffect(99, ML::Vec2(0, 0));
					}
				}
			}
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
		ML::Box2D turnBox(50, 30, 400, 50);
		ML::Box2D timeBox(50, 80, 400, 50);

		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		//---------------------------------------------------------------
		

		//手番の表示
		if (board->turn == 0) {
			//手番の表示
			ML::Box2D draw(820, 550, 229, 119);
			ML::Box2D src(0, 0, 1408, 768);
			this->res->imgSE->Draw(draw, src);

			ML::Box2D drawt(880, 550, 229, 119);
			ML::Box2D srct(0, 0, 1408, 768);
			this->res->imgTE->Draw(drawt, srct);

			ML::Box2D drawn(965, 560, 180, 95);
			ML::Box2D srcn(0, 0, 1380, 752);
			this->res->imgNO->Draw(drawn, srcn);

			ML::Box2D drawb(1030, 560, 180, 95);
			ML::Box2D srcb(0, 0, 1380, 752);
			this->res->imgBA->Draw(drawb, srcb);

			ML::Box2D drawNKR(1000, 640, 100, 50);
			ML::Box2D srcNKR(0, 0, 64 * 2, 64);
			this->res->imgNKR->Draw(drawNKR, srcNKR);

			ML::Box2D drawBY(1160, 640, 50, 50);
			ML::Box2D srcBY(0, 0, 64, 64);
			this->res->imgBY->Draw(drawBY, srcBY);

			//残り時間の表示
			char scoreTxt[9];
			sprintf(scoreTxt, "%02d", this->timeLimit / 60);
			for (int i = 0; i < 2; i++) {

				int y = 0;
				int w = 64;
				int x = i * 41;
				int h = 64;
				auto draw = ML::Box2D(x + 1080, 640, 50, 50);
				auto src = ML::Box2D((scoreTxt[i] - '0') * w, y, w, h);
				this->res->imgSU->Draw(draw, src);

			}
		}
		else {
			//手番の表示
			ML::Box2D draw(-60, -15, 229, 119);
			ML::Box2D src(0, 0, 1408, 768);
			this->res->imgGO->Draw(draw, src);

			ML::Box2D drawt(0, -15, 229, 119);
			ML::Box2D srct(0, 0, 1408, 768);
			this->res->imgTE->Draw(drawt, srct);

			ML::Box2D drawn(85, -5, 180, 95);
			ML::Box2D srcn(0, 0, 1380, 752);
			this->res->imgNO->Draw(drawn, srcn);

			ML::Box2D drawb(150, -5, 180, 95);
			ML::Box2D srcb(0, 0, 1380, 752);
			this->res->imgBA->Draw(drawb, srcb);

			ML::Box2D drawNKR(50, 71, 100, 50);
			ML::Box2D srcNKR(0, 0, 64 * 2, 64);
			this->res->imgNKR->Draw(drawNKR, srcNKR);

			ML::Box2D drawBY(210, 71, 50, 50);
			ML::Box2D srcBY(0, 0, 64, 64);
			this->res->imgBY->Draw(drawBY, srcBY);

			//残り時間の表示
			char scoreTxt[9];
			sprintf(scoreTxt, "%02d", this->timeLimit / 60);
			for (int i = 0; i < 2; i++) {

				int y = 0;
				int w = 64;
				int x = i * 41;
				int h = 64;
				auto draw = ML::Box2D(x + 128, 70, 50, 50);
				auto src = ML::Box2D((scoreTxt[i] - '0') * w, y, w, h);
				this->res->imgSU->Draw(draw, src);

			}
		}

		////残り時間の表示
		//char timeText[64];
		//sprintf_s(timeText, "残り時間:%d", this->timeLimit / 60);

		//5秒以下なら赤色
		if (this->timeLimit <= 6 * 60)
		{
			this->Cnt++;
			        if (this->Cnt % 80 < 40)
					{
						ML::Box2D drawRed(-200, -130, 1920, 1080);
						ML::Box2D srcRed(0, 0, 1463, 1445);
						this->res->imgRed->Draw(
						drawRed,
						srcRed,
						ML::Color(0.4f, 1, 1, 1));
					}
		}
		//else
		//{
		//	this->TestFont->Draw(
		//		timeBox,
		//		timeText,
		//		ML::Color(1, 1, 1, 1));
		//}
	}
	//-------------------------------------------------------------------
	//駒を1枚生成する
	void Object::CreateKoma(
		int boardX_,
		int boardY_,
		Koma::Object::Type type_,
		Koma::Object::Owner owner_)
	{
		auto koma = Koma::Object::Create(true);

		//盤上の位置を設定
		koma->SetBoardPosition(
			boardX_,
			boardY_);

		//移動先の位置を設定
		koma->SetTargetPosition(
			boardX_,
			boardY_);

		//駒の種類
		koma->type = type_;

		//先手・後手
		koma->owner = owner_;

		//成っていない状態
		koma->nari = false;

		//待機状態
		koma->motion = BChara::Motion::Stand;



		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		//盤面配列に駒を登録
		board->SetKoma(boardX_, boardY_, koma);
	}

	//ランダムに先攻後攻を選ぶ
	Koma::Object::Owner Object::RandomSenko()
	{
		switch (rand() % 2)
		{
		case 0:return Koma::Object::Owner::Sente;
			break;
		case 1:return Koma::Object::Owner::Gote;
			break;
		}
	}

	//その他選定
	void Object::RandomSeter(int x_, int y_) {
		switch (rand() % 3)
		{
		case 0://先手王を返す
			if (rand() % 100 > 60) {
				if (this->SO) {
					this->CreateKoma(x_, y_, Koma::Object::Type::Ou, Koma::Object::Owner::Sente);
					this->SO = false;
				}
			}
			break;
		case 1://後手王を返す
			if (rand() % 100 > 60) {
				if (this->GO) {
					this->CreateKoma(x_, y_, Koma::Object::Type::Ou, Koma::Object::Owner::Gote);
					this->GO = false;
				}
			}
			break;
		case 2://空マスを選択
			break;
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
