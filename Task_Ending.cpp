//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include "randomLib.h"
#include "sound.h"
#include"BChara.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	ML::Point  mpos;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		//画像
		this->imgBG = DG::Image::Create("./data/image/魔王城新築01.png");
		this->imgMaoju = DG::Image::Create("./data/image/魔王城03.png");
		this->imgUI = DG::Image::Create("./data/image/Bpush.png");
		this->imgS = DG::Image::Create("./data/image/先手02.png");
		this->imgK = DG::Image::Create("./data/image/後手01.png");
		this->imgE = DG::Image::Create("./data/image/Elogo.png");
		this->imgAme = DG::Image::Create("./data/image/ame02.png");
		this->imgHensai = DG::Image::Create("./data/image/返済ロゴ.png");
		
		//サウンドエフェクト
		se::LoadFile("push", "./data/sound/se/将棋・駒を指す03改良版.wav");
		se::SetVolume("push", 100);

		//BGM 
		bgm::LoadFile("ending_bgm", "./data/sound/bgm/Ebgm01.mp3");
		bgm::VolumeControl("ending_bgm", 90);
		bgm::LoadFile("ending_bgm2", "./data/sound/bgm/運命02.mp3");
		bgm::VolumeControl("ending_bgm", 90);

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgHensai.reset();
		this->imgMaoju.reset();
		this->imgBG.reset();
		this->imgE.reset();
		this->imgK.reset();
		this->imgS.reset();
		this->imgAme.reset();
		this->imgUI.reset();
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
		//シャイニングスター
		if (BChara::winner == 0) {
			bgm::Play("ending_bgm");
		}
		//運命
		if (BChara::winner == 1) {
			bgm::Play("ending_bgm2");
		}
		//★データ初期化
		this->logoPosY = 270;
		this->boardX = 336;
		this->boardY = 40;
		this->lPosX = 1280 / 6;
		this->lPosY = 720 / 8;
		this->hitBase = ML::Box2D(-60, -45, 120, 90);
		PosX = 640;
		PosY = 610;
		this->timecnt = 0;
		this->logPX = 0;
		this->Tcnt = 0;
		this->ET = 0;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		bgm::Stop("ending_bgm");
		bgm::Stop("ending_bgm2");
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		auto ms = ge->mouse->GetState();
		this->ET++;
		mpos.x = ms.pos.x;
		mpos.y = ms.pos.y;
		//マウスと駒UIの判定処理
		if (ms.LB.down) {
			ML::Box2D hit = hitBase;
			hit.x += PosX;
			hit.y += PosY - 40;
			hit.h += 90;
			if (hit.x <= mpos.x && mpos.x < hit.x + hit.w &&
				hit.y <= mpos.y && mpos.y < hit.y + hit.h) {
				se::Play("push");
				ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
				ge->CreateEffect(98, ML::Vec2(0, 0));
			}
		}
		if (Tcnt == 31) {
			se::Play("push");
		}
		
		if (inp.S1.down) {
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));
		}

		if (ge->getCounterFlag("test") == ge->LIMIT || this->ET > 94 * 60) {
			this->Kill();
		}

		if (logPY >= 720 * 3) {
			this->logPY = 1440;
		}

		if (logPX <= -1280) {
			this->logPX = 0;
		}

		if (Tcnt > 120) {
			this->logPY = logPY + 10;
			this->logPX = logPX - 2;
		}

		this->Tcnt++;
		this->timecnt++;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//返済成功背景
		if (BChara::winner == 0){
			auto dbg = ML::Box2D(0, 0, 1280, 720);
			auto sbg = ML::Box2D(0, 0, 1920, 1080);
			this->res->imgBG->Draw(dbg, sbg);
		}

		//失敗時の背景
		if (BChara::winner == 1) {
			ML::Box2D draw(0, 0, 1280, 720);
			ML::Box2D src(0, 0, 1920, 1080);
			this->res->imgMaoju->Draw(draw, src);
		}

		//紙吹雪
		if (BChara::winner == 1) {
			if (Tcnt > 120) {
				for (int j = -1; j < 2; j++) {
					for (int i = -3; i < 0; i++) {
						ML::Box2D draw(logPX + 1280 * j, logPY + 720 * i, 1280, 720);
						ML::Box2D src(0, 0, 1920, 1080);
						this->res->imgAme->Draw(draw, src);
					}
				}
			}
		}

		if (Tcnt > 30) {
			//借金返済ロゴ
			if (BChara::winner == 0)
			{
				ML::Box2D draw(this->lPosX, this->lPosY, 868, 188 - 30);
				ML::Box2D src(0, 0, 2023, 777);
				this->res->imgHensai->Draw(draw, src);

			}
		}





		/*if (Tcnt > 30) {
			//先手勝利ロゴ
			if (BChara::winner == 0)
			{
				ML::Box2D draw(this->lPosX, this->lPosY, 868, 188 - 30);
				ML::Box2D src(0, 0, 868, 188);
				this->res->imgS->Draw(draw, src);

			}
		}*/


		if (Tcnt > 30) {
			//後手勝利ロゴ
			if (BChara::winner == 1)
			{
				ML::Box2D draw(this->lPosX, this->lPosY, 868, 188 - 30);
				ML::Box2D src(0, 0, 823, 184);
				this->res->imgK->Draw(draw, src);
			}
		}

		if (Tcnt > 120) {
			//タイトルに戻るロゴ
			if (this->timecnt % 80 > 40) {
				ML::Box2D draw(1280 / 4, 720 / 8 * 4, 620, 200);
				ML::Box2D src(0, 0, 1408, 768);
				this->res->imgE->Draw(draw, src);

			}
		}


		if (Tcnt > 120) {
			//駒ボタン
			{
				ML::Box2D draw;

				if (mpos.x >= 520 && mpos.x <= 720 &&
					mpos.y >= 500 && mpos.y <= 690)
				{
					draw = ML::Box2D(-135, -102, 270, 204);
					draw.Offset(PosX, PosY);
				}
				else {
					draw = ML::Box2D(-60 * 2, -45 * 2, 120 * 2, 90 * 2);
					draw.Offset(PosX, PosY);
				}
				ML::Box2D src(0, 0, 3200, 2400);
				this->res->imgUI->Draw(draw, src);
			}
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