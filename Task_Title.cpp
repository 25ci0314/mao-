//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"      
#include "randomLib.h"
#include  "Task_Effect00.h"
#include "sound.h"

#include "Task_Game.h"
#include "Task_AI.h"

namespace  Title {

	Object::GameMode Object::gameMode =
		Object::GameMode::Normal;


	DG::Image::SP       img;//タイトル
	ML::Point           mPos;//マウス

	int timeCount = 0;

	enum class TitleState {
		Title,//タイトル画面
		ModeSelect,//セレクト画面
		confirmation,//確認
		explanation0,//説明ページ1
		explanation1,//説明ページ2
		explanation2,//説明ページ3

	};

	TitleState state = TitleState::Title;



	enum class BattleMode {
		TwoPlayer,	//1対1
		CPU			//CPU戦
	};

	BattleMode battleMode = BattleMode::TwoPlayer;

	enum class MoveMode {
		Foot,	//足
		Horse,	//馬
		Doragon,//ドラゴン
	};
	MoveMode moveMode = MoveMode::Foot;

	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize () {

		this->img = DG::Image::Create ("./data/image/title.png");//背景画像
		this->kyanseru = DG::Image::Create ("./data/image/kyanseru.png"); //キャンセルボタン画像
		this->state = DG::Image::Create ("./data/image/state.png");//運ゲーか通常かを分ける
		this->ruru = DG::Image::Create ("./data/image/ru-ru.png");//モード選択画像
		this->randommode = DG::Image::Create ("./data/image/Randommode.png");//モード選択時のランダムモード
		this->randommode1 = DG::Image::Create ("./data/image/randommode1.png");//現在のモードがランダムモードであることを表示する画像
		this->randommode2 = DG::Image::Create ("./data/image/kakuninnRandomtwo.png");//現在のモードがランダムモードであることを表示する画像
		this->randommode3 = DG::Image::Create ("./data/image/kakuninnRandomCPU.png");//現在のモードがランダムモードであることを表示する画像
		this->tuuzyoumode = DG::Image::Create ("./data/image/Normalmode.png");//モード選択時の通常モード
		this->tuuzyoumode1 = DG::Image::Create ("./data/image/Normalmode1.png");//現在のモードが通常モードであることを表示する画像
		this->tuuzyoumode2 = DG::Image::Create ("./data/image/kakuninntuuzyoutwo.png");//現在のモードが通常モードであることを表示する画像
		this->tuuzyoumode3 = DG::Image::Create ("./data/image/kakuninntuuzyouCPU.png");//現在のモードが通常モードであることを表示する画像
		this->twobattle = DG::Image::Create ("./data/image/twobattle.png");//二人対戦画像
		this->foottwo = DG::Image::Create ("./data/image/foottwobattle.png");//徒歩モードの二人対戦画像
		this->horsetwo = DG::Image::Create ("./data/image/horsetwobattle.png");//馬モードの二人対戦画像
		this->doragontwo = DG::Image::Create ("./data/image/doragontwobattle.png");//竜モードの二人対戦画像
		this->footCPU = DG::Image::Create ("./data/image/footCPUbattle.png");//徒歩モードのCPU戦画像
		this->horseCPU = DG::Image::Create ("./data/image/houseCPUbattle.png");//馬モードのCPU戦画像
		this->doragonCPU = DG::Image::Create ("./data/image/doragonCPUbattle.png");//竜モードのCPU戦画像
		this->cpubattle = DG::Image::Create ("./data/image/cpubattle.png");//CPU戦画像
		this->Yes = DG::Image::Create ("./data/image/Yes.png");//はい画像
		this->No = DG::Image::Create ("./data/image/No.png");//いいえ画像
		this->BG = DG::Image::Create ("./data/image/BG0.png");//黒背景画像
		this->setumei0 = DG::Image::Create ("./data/image/setumei.png");//説明画像1ページ
		this->setumei1 = DG::Image::Create ("./data/image/kari1.png");//説明画像2ページ
		this->setumei2 = DG::Image::Create ("./data/image/kari2.jpg");//説明画像3ページ
		this->foot = DG::Image::Create ("./data/image/foot.jpg");//足
		this->horse = DG::Image::Create ("./data/image/horse.png");//馬
		this->doragon = DG::Image::Create ("./data/image/doragon.png");//ドラゴン
		this->space = DG::Image::Create ("./data/image/key.png");//スペースキーをおしてね
		this->pageback = DG::Image::Create ("./data/image/pageback.png");//前のページへ
		this->pagemoveon = DG::Image::Create ("./data/image/pagemoveon.png");//次のページへ


		this->GB = DG::Image::Create ("./data/image/GB.png");//背景画像
		this->fontA = DG::Font::Create ("HGS教科書体", 40, 80);
		this->fontB = DG::Font::Create ("HGS教科書体", 40, 80);
		this->fontC = DG::Font::Create ("HGS教科書体", 40, 80);
		this->fontD = DG::Font::Create ("HGS教科書体", 40, 80);
		this->fontE = DG::Font::Create ("HGS教科書体", 40, 80);
		this->fontF = DG::Font::Create ("HGS教科書体", 40, 80);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize () {
		this->img.reset ();
		this->kyanseru.reset ();
		this->state.reset ();
		this->ruru.reset ();
		this->randommode.reset ();
		this->randommode1.reset ();
		this->randommode2.reset ();
		this->randommode3.reset ();
		this->tuuzyoumode.reset ();
		this->tuuzyoumode1.reset ();
		this->tuuzyoumode2.reset ();
		this->tuuzyoumode3.reset ();
		this->twobattle.reset ();
		this->foottwo.reset ();
		this->horsetwo.reset ();
		this->doragontwo.reset ();
		this->cpubattle.reset ();
		this->Yes.reset ();
		this->No.reset ();
		this->BG.reset ();
		this->setumei0.reset ();
		this->setumei1.reset ();
		this->setumei2.reset ();
		this->foot.reset ();
		this->horse.reset ();
		this->doragon.reset ();
		this->space.reset ();
		this->pageback.reset ();
		this->pagemoveon.reset ();
		this->GB.reset ();
		this->fontA.reset ();
		this->fontB.reset ();
		this->fontC.reset ();
		this->fontD.reset ();
		this->fontE.reset ();
		this->fontF.reset ();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize () {
		//スーパークラス初期化
		__super::Initialize (defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create ();

		//★データ初期化
		this->controller = ge->in1;

		this->render2D_Priority[1] = 0.5f;
		int RenderTime = 0;

		ge->stage = 1;


		//音楽ファイルの読み込み
		//BGM
		// BGMの利用するファイルは曲の長さによってサイズが変わります。
		// 一般的にWavは中身が波形データと呼ばれるサイズが大きなデータにります。
		// 一方mp3はネットなどでの扱いを想定した圧縮形式です。
		// BGMを用いる場合はmp3形式のファイルを利用しましょう。
		// ちなみにこのサンプルのファイルは
		// 再生時間1：30ほぼの曲で
		// mp3 = 4.3MB wav = 19MBです。
		bgm::LoadFile ("bgm1", "./data/sound/bgm/弾むキモチ_2.mp3");
		bgm::Play ("bgm1");

		//se
		// seは効果音です。
		// bgmとの最大の違いはひとつの音楽ファイルの同時再生に対応していることです。
		// seはwavファイルしか扱うことが出来ません。mp3はエラーになります。
		// 同時再生の必要がないものはBGM
		// 同時再生が必要なものはseと使い分けてください。
	  // またこのサンプルのような日本語ファイル名はやめた方が良いです。
		// 読み込みエラーの元になります。
		se::LoadFile ("se1", "./data/sound/se/「すごいすごい」.wav");

		//		se::LoadFile("se2", "./data/sound/se/「頑張ったね」.mp3");これはエラー wavじゃないとダメ
		se::LoadFile ("se2", "./data/sound/se/「頑張ったね」.wav");
		se::LoadFile ("se3", "./data/sound/se/「さあ、いくぞ！」.wav");
		se::LoadFile ("se4", "./data/sound/se/ok.wav");
		se::LoadFile ("se5", "./data/sound/se/poku.wav");
		//★タスクの生成
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize () {
		bgm::Stop ("bgm1");

		if (!ge->QuitFlag () && this->nextTaskCreate) {
			//魔王配達のゲーム本編を生成
			Game::Object::Create (true);
		}
		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate () {

		timeCount++;
		if (timeCount >= 90) {
			timeCount = 30;
		}


		auto inp = ge->in1->GetState ();
		auto ms = ge->mouse->GetState ();
		mPos.x = ms.pos.x;
		mPos.y = ms.pos.y;
		//マウス入力への対応
		if (ms.LB.down) {

			// キャンセルボタン
			if (mPos.x >= 1180 && mPos.x <= 1280 &&
				mPos.y >= 0 && mPos.y <= 100) {
				se::Play ("se5");
				state = TitleState::Title;  // タイトルへ戻る
			}
			//タイトル状態のとき
			if (state == TitleState::Title) {
				// 二人対戦
				if (mPos.x >= 260 && mPos.x <= 615 &&
					mPos.y >= 550 && mPos.y <= 720) {

					battleMode = BattleMode::TwoPlayer;

					state = TitleState::ModeSelect;//セレクト画面へ
					se::Play ("se4");
				}

				// CPU戦
				if (mPos.x >= 660 && mPos.x <= 980 &&
					mPos.y >= 550 && mPos.y <= 720) {

					battleMode = BattleMode::CPU;
					state = TitleState::ModeSelect;//セレクト画面へ
					se::Play ("se4");
				}

				//説明画面へ
				if (mPos.x >= 0 && mPos.x <= 350 &&
					mPos.y >= 300 && mPos.y <= 500) {

					se::Play ("se4");
					state = TitleState::explanation0;//説明状態に
				}
			}



			//セレクト画面
			if (state == TitleState::ModeSelect) {

				//足
				{
					if (mPos.x >= 0 && mPos.x <= 426 &&
						mPos.y >= 465 && mPos.y <= 520) {
						se::Play ("se4");
						state = TitleState::confirmation;//確認画面へ
						moveMode = MoveMode::Foot; //移動方法を足に
					}
				}
				//馬
				{
					if (mPos.x >= 427 && mPos.x <= 852 &&
						mPos.y >= 465 && mPos.y <= 520) {
						se::Play ("se4");
						state = TitleState::confirmation;//確認画面へ
						moveMode = MoveMode::Horse;//移動方法を馬に
					}
				}
				//ドラゴン
				{
					if (mPos.x >= 853 && mPos.x <= 1278 &&
						mPos.y >= 465 && mPos.y <= 520) {
						se::Play ("se4");
						state = TitleState::confirmation;//確認画面へ
						moveMode = MoveMode::Doragon;//移動方法をドラゴンに
					}
				}


			}

			/* はい
			if (mPos.x >= 370 && mPos.x <= 600 &&
				mPos.y >= 550 && mPos.y <= 615) {

				state = TitleState::explanation2;
				se::Play ("se5");
			}*/

			/* いいえ
			else if (mPos.x >= 670 && mPos.x <= 890 &&
				mPos.y >= 550 && mPos.y <= 615) {

				state = TitleState::explanation0;
				se::Play ("se5");
			}*/
			switch (state) {
				// 説明画面1ページ
			case TitleState::explanation0:

				// はい
				if (mPos.x >= 670 && mPos.x <= 890 &&
					mPos.y >= 550 && mPos.y <= 615) {
					state = TitleState::explanation1;
					se::Play ("se5");
				}
				break;


				// 説明画面2ページ
			case TitleState::explanation1:

				// はい
				if (mPos.x >= 670 && mPos.x <= 890 &&
					mPos.y >= 550 && mPos.y <= 615) {

					state = TitleState::explanation2;
					se::Play ("se5");
				}

				// いいえ
				else if (mPos.x >= 370 && mPos.x <= 600 &&
					mPos.y >= 550 && mPos.y <= 615) {

					state = TitleState::explanation0;
					se::Play ("se5");
				}
				break;


				// 説明画面3ページ
			case TitleState::explanation2:

				// いいえ
				if (mPos.x >= 370 && mPos.x <= 600 &&
					mPos.y >= 550 && mPos.y <= 615) {

					state = TitleState::explanation1;
					se::Play ("se5");
				}
				break;
			}


		}
		//確認画面の時
		if (state == TitleState::confirmation) {
			if (inp.S1.down) {
				state = TitleState::Title;
				ge->StartCounter ("test", 45);//ゲーム画面に移行
				ge->CreateEffect (98, ML::Vec2 (0, 0));
				se::Play ("se3");

			}
		}
		int input = 0;

		if (ge->getCounterFlag ("test") == ge->LIMIT) {
			this->Kill ();
		}

		if (input & 0x0002) {
			se::Play ("se1");
		}
		if (input & 0x0004) {
			se::Play ("se2");
		}
		if (input & 0x0008) {
			se::Play ("se3");
		}
		if (input & 0x0016) {
			se::Play ("se4");
		}
		if (input & 0x0032) {
			se::Play ("se5");
		}
		return;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF () {
		//背景画像
		if (state == TitleState::ModeSelect || state == TitleState::Title) {
		
			ML::Box2D  draw (0, 0, 1280, 780);
			ML::Box2D  src (0, 0, 1536, 1024);

			draw.Offset (0, 0);
			this->res->img->Draw (draw, src);

			
		}
		/*現在の状態を表示 (通常モード)
		if (gameMode == GameMode::Normal)
		{
			ML::Box2D  draw(0, 0, 500, 200);
			ML::Box2D  src(0, 0, 2172, 724);
			draw.Offset(0, 0);
			this->res->tuuzyoumode1->Draw(draw, src);
		}
		//現在の状態を表示(ランダムモード)

		if (gameMode == GameMode::Random) {
			ML::Box2D  draw(10, 0, 500, 200);
			ML::Box2D  src(0, 0, 2172, 724);
			draw.Offset(0, 0);
			this->res->randommode1->Draw(draw, src);
		}*/

		//タイトル状態のとき
		if (state == TitleState::Title) {
			//スタートキー画像
			if (timeCount > 60) {
				ML::Box2D  draw (350, 300, 600, 300);
				ML::Box2D  src (0, 0, 1774, 887);

				draw.Offset (0, 0);
				this->res->state->Draw (draw, src);
			}
			//モード選択画像
			{
				ML::Box2D draw;

				if (mPos.x >= 0 && mPos.x <= 350 &&
					mPos.y >= 300 && mPos.y <= 500) {
					draw = ML::Box2D (-10, 290, 370, 220);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (0, 300, 350, 200);
				}

				ML::Box2D src (0, 0, 1536, 1024);
				this->res->ruru->Draw (draw, src);
			}
			//二人で対戦画像
			{
				ML::Box2D draw;
				if (mPos.x >= 252 && mPos.x <= 630 &&
					mPos.y >= 550 && mPos.y <= 720)
					//大きくする
				{
					draw = ML::Box2D (240, 540, 400, 190);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (252, 550, 378, 170);
				}

				ML::Box2D src (0, 0, 1536, 1024);
				this->res->twobattle->Draw (draw, src);
			}
			//CPU戦画像
			{
				ML::Box2D draw;

				if (mPos.x >= 650 && mPos.x <= 1028 &&
					mPos.y >= 550 && mPos.y <= 730)
					//大きくする
				{
					draw = ML::Box2D (638, 540, 400, 200);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (650, 550, 378, 180);
				}

				ML::Box2D src (0, 0, 1536, 1024);
				this->res->cpubattle->Draw (draw, src);
			}
		}
		//モードセレクト状態のとき
		if (state == TitleState::ModeSelect) {
			// 足画像
			{
				ML::Box2D draw;

				if (mPos.x >= 0 && mPos.x <= 426 &&
					mPos.y >= 465 && mPos.y <= 520)
					// 大きくする
				{

					draw = ML::Box2D (0, 420, 456, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (0, 400, 426, 200);

				}

				ML::Box2D src (0, 0, 640, 360);
				this->res->foot->Draw (draw, src);
			}

			// 馬画像
			{
				ML::Box2D draw;

				if (mPos.x >= 427 && mPos.x <= 852 &&
					mPos.y >= 465 && mPos.y <= 520)
					// 大きくする
				{

					draw = ML::Box2D (412, 420, 456, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (427, 400, 426, 200);
				}

				ML::Box2D src (0, 0, 1280, 1920);
				this->res->horse->Draw (draw, src);
			}
			// ドラゴン画像
			{
				ML::Box2D draw;

				if (mPos.x >= 853 && mPos.x <= 1278 &&
					mPos.y >= 465 && mPos.y <= 520)
					// 大きくする
				{

					draw = ML::Box2D (838, 420, 456, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (853, 400, 426, 200);
				}


				ML::Box2D src (0, 0, 360, 360);
				this->res->doragon->Draw (draw, src);
			}
			//キャンセル画像
			{
				ML::Box2D  draw (1180, 0, 100, 100);
				ML::Box2D  src (0, 0, 1200, 1200);

				draw.Offset (0, 0);
				this->res->kyanseru->Draw (draw, src);
			}
		}
		//確認状態のとき
		if (state == TitleState::confirmation) {
			//黒背景
			{
				ML::Box2D  draw (0, 0, 1280, 780);
				ML::Box2D  src (0, 0, 640, 360);

				draw.Offset (0, 0);
				this->res->BG->Draw (draw, src);
			}
			//スペースキー
			if (timeCount > 60) {
				ML::Box2D  draw (300, 385, 600, 200);
				ML::Box2D  src (0, 0, 2172, 724);

				draw.Offset (0, 0);
				this->res->space->Draw (draw, src);
			}
			//二人対戦のとき
			if (battleMode == BattleMode::TwoPlayer) {
				//二人対戦の足モード
				if (moveMode == MoveMode::Foot) {
					ML::Box2D draw (200, 200, 900, 250);
					ML::Box2D  src (0, 0, 2172, 724);

					draw.Offset (0, 0);
					this->res->foottwo->Draw (draw, src);
				}
				//二人対戦の馬モード
				if (moveMode == MoveMode::Horse) {
					ML::Box2D draw (200, 200, 900, 250);
					ML::Box2D  src (0, 0, 2172, 724);

					draw.Offset (0, 0);
					this->res->horsetwo->Draw (draw, src);
				}
				//二人対戦のドラゴンモード
				if (moveMode == MoveMode::Doragon) {
					ML::Box2D draw (200, 200, 900, 250);
					ML::Box2D  src (0, 0, 2172, 724);

					draw.Offset (0, 0);
					this->res->doragontwo->Draw (draw, src);
				}
			}
			//CPU戦のとき
			if (battleMode == BattleMode::CPU) {
				{
					//CPU戦の足モード
					if (moveMode == MoveMode::Foot) {
						ML::Box2D draw (200, 200, 900, 250);
						ML::Box2D  src (0, 0, 2172, 724);

						draw.Offset (0, 0);
						this->res->footCPU->Draw (draw, src);
					}
					//CPU戦の馬モード
					if (moveMode == MoveMode::Horse) {
						ML::Box2D draw (200, 200, 900, 250);
						ML::Box2D  src (0, 0, 2172, 724);

						draw.Offset (0, 0);
						this->res->horseCPU->Draw (draw, src);
					}
					//CPU戦のドラゴンモード
					if (moveMode == MoveMode::Doragon) {
						ML::Box2D draw (200, 200, 900, 250);
						ML::Box2D  src (0, 0, 2172, 724);

						draw.Offset (0, 0);
						this->res->doragonCPU->Draw (draw, src);
					}
				}
			}
			/*通常状態なら
				if (battleMode == BattleMode::TwoPlayer) {
					ML::Box2D draw(200, 200, 900, 250);
					ML::Box2D  src(0, 0, 2172, 724);

					draw.Offset(0, 0);
					this->res->tuuzyoumode2->Draw(draw, src);
				}
				else {
					ML::Box2D draw(200, 200, 900, 250);
					ML::Box2D  src(0, 0, 2172, 724);

					draw.Offset(0, 0);
					this->res->tuuzyoumode3->Draw(draw, src);
				}

				if (battleMode == BattleMode::TwoPlayer) {
					ML::Box2D draw(200, 200, 900, 250);
					ML::Box2D  src(0, 0, 2172, 724);

					draw.Offset(0, 0);
					this->res->randommode2->Draw(draw, src);
				}
				else {
					ML::Box2D draw(200, 200, 900, 250);
					ML::Box2D  src(0, 0, 2172, 724);

					draw.Offset(0, 0);
					this->res->randommode3->Draw(draw, src);
				}*/

				//キャンセル画像
			{
				ML::Box2D  draw (1180, 0, 100, 100);
				ML::Box2D  src (0, 0, 1200, 1200);

				draw.Offset (0, 0);
				this->res->kyanseru->Draw (draw, src);
			}
		}
		//説明画面1ページ
		if (state == TitleState::explanation0) {
			//緑背景
			{
				ML::Box2D  draw (0, 0, 1280, 780);
				ML::Box2D  src (0, 0, 1536, 1024);

				draw.Offset (0, 0);
				this->res->GB->Draw (draw, src);
			}
			//説明画面
			{
				ML::Box2D  draw (200, 0, 780, 560);
				ML::Box2D  src (0, 0, 1536, 1024);

				draw.Offset (0, 0);
				this->res->setumei0->Draw (draw, src);
			}
			// はい画像
			{
				ML::Box2D draw;

				if (mPos.x >= 670 && mPos.x <= 890 &&
					mPos.y >= 550 && mPos.y <= 615)
					// 大きくする
				{
					draw = ML::Box2D (585, 540, 280, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (600, 550, 250, 120);
				}

				ML::Box2D src (0, 0, 447, 447);
				this->res->pagemoveon->Draw (draw, src);
			}

			//キャンセル画像
			{
				ML::Box2D  draw (1180, 0, 100, 100);
				ML::Box2D  src (0, 0, 1200, 1200);

				draw.Offset (0, 0);
				this->res->kyanseru->Draw (draw, src);
			}

		}

		//説明画面2ページ
		if (state == TitleState::explanation1) {
			//緑背景
			{
				ML::Box2D  draw (0, 0, 1280, 780);
				ML::Box2D  src (0, 0, 1536, 1024);

				draw.Offset (0, 0);
				this->res->GB->Draw (draw, src);
			}
			//説明画面
			{
				ML::Box2D  draw (200, 0, 780, 560);
				ML::Box2D  src (0, 0, 1536, 1024);

				draw.Offset (0, 0);
				this->res->setumei1->Draw (draw, src);
			}

			// はい画像
			{
				ML::Box2D draw;

				if (mPos.x >= 670 && mPos.x <= 890 &&
					mPos.y >= 550 && mPos.y <= 615)
					// 大きくする
				{
					draw = ML::Box2D (585, 540, 280, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (600, 550, 250, 120);
				}

				ML::Box2D src (0, 0, 447, 447);
				this->res->pagemoveon->Draw (draw, src);
			}

			// いいえ画像
			{
				ML::Box2D draw;

				if (mPos.x >= 370 && mPos.x <= 600 &&
					mPos.y >= 550 && mPos.y <= 615)
					// 大きくする
				{
					draw = ML::Box2D (285, 540, 280, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (300, 550, 250, 120);
				}

				ML::Box2D src (0, 0, 1300, 1300);
				this->res->pageback->Draw (draw, src);
			}
		
			//キャンセル画像
			{
				ML::Box2D  draw (1180, 0, 100, 100);
				ML::Box2D  src (0, 0, 1200, 1200);

				draw.Offset (0, 0);
				this->res->kyanseru->Draw (draw, src);
			}
		}

		//説明画面3ページ
		if (state == TitleState::explanation2) {
			//緑背景
			{
				ML::Box2D  draw (0, 0, 1280, 780);
				ML::Box2D  src (0, 0, 1536, 1024);

				draw.Offset (0, 0);
				this->res->GB->Draw (draw, src);
			}
			//説明画面
			{
				ML::Box2D  draw (200, 0, 780, 560);
				ML::Box2D  src (0, 0, 1536, 1024);

				draw.Offset (0, 0);
				this->res->setumei2->Draw (draw, src);
			}

			// いいえ画像
			{
				ML::Box2D draw;

				if (mPos.x >= 370 && mPos.x <= 600 &&
					mPos.y >= 550 && mPos.y <= 615)
					// 大きくする
				{
					draw = ML::Box2D (285, 540, 280, 150);
				}
				else
					// 通常サイズ
				{
					draw = ML::Box2D (300, 550, 250, 120);
				}

				ML::Box2D src (0, 0, 1300, 1300);
				this->res->pageback->Draw (draw, src);
			}
			//キャンセル画像
			{
				ML::Box2D  draw (1180, 0, 100, 100);
				ML::Box2D  src (0, 0, 1200, 1200);

				draw.Offset (0, 0);
				this->res->kyanseru->Draw (draw, src);
			}
		}

	}


	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create (bool  flagGameEnginePushBack_) {
		Object::SP  ob = Object::SP (new  Object ());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack (ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize ()) {
				ob->Kill ();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize () {
		return  this->Initialize ();
	}
	//-------------------------------------------------------------------
	Object::~Object () { this->B_Finalize (); }
	bool  Object::B_Finalize () {
		auto  rtv = this->Finalize ();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object () {}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create () {
		if (auto sp = instance.lock ()) {
			return sp;
		}
		else {
			sp = Resource::SP (new  Resource ());
			if (sp) {
				sp->Initialize ();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource () {}
	//-------------------------------------------------------------------
	Resource::~Resource () { this->Finalize (); }
}