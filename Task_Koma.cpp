//-------------------------------------------------------------------
//駒
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Koma.h"
#include "Task_Board.h"
#include "Task_Cursor.h"
#include "Task_Game.h"
#include "Task_Game_Random.h"
#include "Task_Title.h"
#include "sound.h"
#include "Task_AI.h"
#include "Task_StartTimer.h"

namespace Koma
{

	//相手の時間を減らす秒数
	int Object::timePenalty = 0;

	//破壊予定の列
	int Object::destroyColumn = -1;

	//破壊までの残りターン数
	int Object::destroyTurn = 0;

	//爆弾を発動した側
	int Object::bombOwner = -1;

	Resource::WP Resource::instance;

	//-------------------------------------------------------------------
	//リソースの初期化
	bool Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/koma.png");
		this->moveImg = DG::Image::Create("./data/image/move.png");

		return true;
	}

	//-------------------------------------------------------------------
	//リソースの解放
	bool Resource::Finalize()
	{
		this->img.reset();
		this->moveImg.reset();
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
		this->render2D_Priority[1] = 0.7f;


		//盤上の初期位置
		this->SetBoardPosition(4, 6);

		//移動先の初期位置
		this->SetTargetPosition(
			this->boardX,
			this->boardY);

		this->type = Type::Fu;
		this->owner = Owner::Sente;
		this->nari = false;



		//列破壊SEを読み込む
		se::LoadFile("bom", "./data/sound/se/bom.wav");

		//駒を指すSE
		se::LoadFile("sasu", "./data/sound/se/将棋・駒を指す03改良版.wav");

		this->motion = Motion::Stand;
		//★タスクの生成

		this->GameStart = false;
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
		this->moveCnt++;
		this->animCnt++;

		//思考・状況判断
		this->Think();

		//スタートタイマータスク取得
		auto timer = ge->GetTask<StartTimer::Object>(
			StartTimer::defGroupName,
			StartTimer::defName);
		if (timer == nullptr) {
			this->GameStart = true;
		}

		//現在のモーションに対応した処理
		this->Move();
	}
	
	//-------------------------------------------------------------------
	//指定したマスを移動可能にする
	void Object::SetMovePoint(int x_, int y_)
	{
		//盤面の範囲外なら設定しない
		if (x_ < 0 || x_ >= 9) {
			return;
		}

		if (y_ < 0 || y_ >= 9) {
			return;
		}

		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		//指定したマスの駒を取得
		auto koma = board->GetKoma(x_, y_);

		//駒がいないマスなら移動可能
		if (koma == nullptr) {
			this->movePossible[y_][x_] = 1;
			return;
		}

		//相手の駒がいるマスなら移動可能
		if (koma->owner != this->owner) {
			this->movePossible[y_][x_] = 1;
		}
	}
	//-------------------------------------------------------------------
	//指定した方向へ連続して移動可能マスを設定
	void Object::SetMoveLine(int addX_, int addY_)
	{
		int x = this->boardX + addX_;
		int y = this->boardY + addY_;

		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		while (true) {

			//盤外なら終了
			if (x < 0 || x >= 9 ||
				y < 0 || y >= 9) {
				break;
			}

			//そのマスの駒を取得
			auto koma = board->GetKoma(x, y);

			//空マス
			if (koma == nullptr) {
				this->movePossible[y][x] = 1;
			}
			else {

				//相手なら取れる
				if (koma->owner != this->owner) {
					this->movePossible[y][x] = 1;
				}

				//駒があったらそこで終了
				break;
			}

			x += addX_;
			y += addY_;
		}
	}
	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	//駒の種類に応じて移動可能マスを設定
	void Object::SetMovePossible()
	{
		//前回の移動可能マスを消す
		this->ClearMovePossible();

		//成っている場合
		if (this->nari == true) {

			switch (this->type) {
			case Type::Fu:
			case Type::Kyo:
			case Type::Kei:
			case Type::Gin:
				//と金・成香・成桂・成銀は金と同じ動き
				this->SetMoveKin();
				return;

			case Type::Kaku:
				//馬
				this->SetMoveUma();
				return;

			case Type::Hisya:
				//龍
				this->SetMoveRyu();
				return;

			case Type::Kin:
			case Type::Ou:
				break;
			}
		}

		//成っていない場合
		switch (this->type) {
		case Type::Fu:
			this->SetMoveFu();
			break;

		case Type::Kyo:
			this->SetMoveKyo();
			break;

		case Type::Kei:
			this->SetMoveKei();
			break;

		case Type::Gin:
			this->SetMoveGin();
			break;

		case Type::Kin:
			this->SetMoveKin();
			break;

		case Type::Kaku:
			this->SetMoveKaku();
			break;

		case Type::Hisya:
			this->SetMoveHisya();
			break;

		case Type::Ou:
			this->SetMoveOu();
			break;
		}
	}
	//-------------------------------------------------------------------
	//成れるか判定
	bool Object::CheckNari()
	{
		//すでに成っている
		if (this->nari == true) {
			return false;
		}

		//王・金は成れない
		if (this->type == Type::Ou ||
			this->type == Type::Kin) {
			return false;
		}

		//先手
		if (this->owner == Owner::Sente) {
			if (this->boardY <= 2) {
				return true;
			}
		}

		//後手
		if (this->owner == Owner::Gote) {
			if (this->boardY >= 6) {
				return true;
			}
		}

		return false;
	}
	//-------------------------------------------------------------------
	//歩の移動可能マスを設定
	void Object::SetMoveFu()
	{
		int dir;

		//先手は上、後手は下
		if (this->owner == Owner::Sente) {
			dir = -1;
		}
		else {
			dir = 1;
		}

		this->SetMovePoint(
			this->boardX,
			this->boardY + dir);
	}
	//-------------------------------------------------------------------
    //王の移動可能マスを設定
	void Object::SetMoveOu()
	{
		//左上
		this->SetMovePoint(
			this->boardX - 1,
			this->boardY - 1);

		//上
		this->SetMovePoint(
			this->boardX,
			this->boardY - 1);

		//右上
		this->SetMovePoint(
			this->boardX + 1,
			this->boardY - 1);

		//左
		this->SetMovePoint(
			this->boardX - 1,
			this->boardY);

		//右
		this->SetMovePoint(
			this->boardX + 1,
			this->boardY);

		//左下
		this->SetMovePoint(
			this->boardX - 1,
			this->boardY + 1);

		//下
		this->SetMovePoint(
			this->boardX,
			this->boardY + 1);

		//右下
		this->SetMovePoint(
			this->boardX + 1,
			this->boardY + 1);
	}
	//-------------------------------------------------------------------
	//金の移動可能マスを設定
	void Object::SetMoveKin()
	{
		int dir;

		//先手は上、後手は下
		if (this->owner == Owner::Sente) {
			dir = -1;
		}
		else {
			dir = 1;
		}

		//前
		this->SetMovePoint(
			this->boardX,
			this->boardY + dir);

		//前左
		this->SetMovePoint(
			this->boardX - 1,
			this->boardY + dir);

		//前右
		this->SetMovePoint(
			this->boardX + 1,
			this->boardY + dir);

		//左
		this->SetMovePoint(
			this->boardX - 1,
			this->boardY);

		//右
		this->SetMovePoint(
			this->boardX + 1,
			this->boardY);

		//後ろ
		this->SetMovePoint(
			this->boardX,
			this->boardY - dir);
	}
	//-------------------------------------------------------------------
    //銀の移動可能マスを設定
	void Object::SetMoveGin()
	{
		int dir;

		if (this->owner == Owner::Sente) {
			dir = -1;
		}
		else {
			dir = 1;
		}

		this->SetMovePoint(this->boardX - 1, this->boardY + dir);
		this->SetMovePoint(this->boardX, this->boardY + dir);
		this->SetMovePoint(this->boardX + 1, this->boardY + dir);

		this->SetMovePoint(this->boardX - 1, this->boardY - dir);
		this->SetMovePoint(this->boardX + 1, this->boardY - dir);
	}
	//-------------------------------------------------------------------
    //香車の移動可能マスを設定
	void Object::SetMoveKyo()
	{
		int dir;

		if (this->owner == Owner::Sente) {
			dir = -1;
		}
		else {
			dir = 1;
		}

		this->SetMoveLine(0, dir);
	}
	//-------------------------------------------------------------------
    //桂馬の移動可能マスを設定
	void Object::SetMoveKei()
	{
		int dir;

		if (this->owner == Owner::Sente) {
			dir = -1;
		}
		else {
			dir = 1;
		}

		this->SetMovePoint(
			this->boardX - 1,
			this->boardY + dir * 2);

		this->SetMovePoint(
			this->boardX + 1,
			this->boardY + dir * 2);
	}
	//-------------------------------------------------------------------
    //角の移動可能マスを設定
	void Object::SetMoveKaku()
	{
		this->SetMoveLine(-1, -1);
		this->SetMoveLine(1, -1);
		this->SetMoveLine(-1, 1);
		this->SetMoveLine(1, 1);
	}
	//-------------------------------------------------------------------
    //飛車の移動可能マスを設定
	void Object::SetMoveHisya()
	{
		this->SetMoveLine(0, -1);
		this->SetMoveLine(0, 1);
		this->SetMoveLine(-1, 0);
		this->SetMoveLine(1, 0);
	}
	//-------------------------------------------------------------------
	//馬の移動可能マスを設定
	void Object::SetMoveUma()
	{
		//角と同じ斜め移動
		this->SetMoveKaku();

		//上下左右に1マス
		this->SetMovePoint(
			this->boardX,
			this->boardY - 1);

		this->SetMovePoint(
			this->boardX,
			this->boardY + 1);

		this->SetMovePoint(
			this->boardX - 1,
			this->boardY);

		this->SetMovePoint(
			this->boardX + 1,
			this->boardY);
	}
	//-------------------------------------------------------------------
	//龍の移動可能マスを設定
	void Object::SetMoveRyu()
	{
		//飛車と同じ縦横移動
		this->SetMoveHisya();

		//斜めに1マス
		this->SetMovePoint(
			this->boardX - 1,
			this->boardY - 1);

		this->SetMovePoint(
			this->boardX + 1,
			this->boardY - 1);

		this->SetMovePoint(
			this->boardX - 1,
			this->boardY + 1);

		this->SetMovePoint(
			this->boardX + 1,
			this->boardY + 1);
	}
	//-------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void Object::Think()
	{
		BChara::Motion nm = this->motion;

		auto cursor = ge->GetTask<Cursor::Object>(
			Cursor::defGroupName,
			Cursor::defName);

		if (cursor == nullptr) {
			return;
		}

		auto ms = ge->mouse->GetState();

		//NPC戦で後手の番なら、プレイヤーの操作を禁止
		//NPCが存在する場合、後手の操作を禁止
		auto ai = ge->GetTask<AI::Object>(
			AI::defGroupName,
			AI::defName);

		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (ai != nullptr &&
			board != nullptr &&
			board->turn == 1) {
			return;
		}
		if (this->GameStart) {
			//モーションの変更以外の処理は行わない
			switch (nm) {
			case Motion::Stand:

				if (ms.LB.down) {

					//将棋盤タスクを取得
					auto board = ge->GetTask<Board::Object>(
						Board::defGroupName,
						Board::defName);

					if (board == nullptr) {
						break;
					}

					//持ち駒を選択中なら盤面の駒を選択しない
					if (board->selectHandType >= 0) {
						break;
					}

					int ownerNo;
					int handLeft;
					int handTop;

					if (board->turn == 0) {
						ownerNo = 0;
						handLeft = 900;
						handTop = 430;
					}
					else {
						ownerNo = 1;
						handLeft = 30;
						handTop = 180;
					}

					for (int type = 1; type < 8; type++) {

						//持っていない駒は無視
						if (board->handKoma[ownerNo][type] <= 0) {
							continue;
						}

						int drawX = handLeft + (type % 4) * 48;

						int drawY = handTop + (type / 4) * 48;

						ML::Box2D handBox(drawX, drawY, 40, 40);
						ML::Box2D mouseBox(ms.pos.x, ms.pos.y, 1, 1);

						//持ち駒をクリックしている
						if (handBox.Hit(mouseBox)) {
							return;
						}
					}

					//先手の番に先手の駒を選択
					if (board->turn == 0 &&
						this->owner != Owner::Sente) {
						break;
					}

					//後手の番に後手の駒を選択
					if (board->turn == 1 &&
						this->owner != Owner::Gote) {
						break;
					}

					//他に選択中の駒があるか調べる
					bool otherSelect = false;

					auto komas = ge->GetTasks<Koma::Object>(
						Koma::defGroupName);

					for (auto it = komas->begin();
						it != komas->end();
						++it) {

						//自分自身は調べない
						if ((*it).get() == this) {
							continue;
						}

						//他の駒が選択中なら新しく選択しない
						if ((*it)->motion == Motion::Select ||
							(*it)->motion == Motion::Move) {

							otherSelect = true;
							break;
						}
					}

					if (otherSelect == true) {
						break;
					}




					//カーソルが自分の駒の位置にある
					if (cursor->boardX == this->boardX &&
						cursor->boardY == this->boardY) {

						//移動可能マスを設定
						this->SetMovePossible();

						//選択状態へ変更
						nm = Motion::Select;
					}
				}

				break;

			case Motion::Select:
				if (ms.LB.down) {

					//今いるマスをクリックしたら選択解除
					if (cursor->boardX == this->boardX &&
						cursor->boardY == this->boardY) {

						nm = Motion::Stand;
					}
					else {
						//移動可能マスをクリックした場合だけ移動
						if (this->movePossible
							[cursor->boardY]
							[cursor->boardX] == 1) {

							this->targetBoardX = cursor->boardX;
							this->targetBoardY = cursor->boardY;
							se::Play("sasu");
							nm = Motion::Move;
						}
					}
				}
				break;

			case Motion::Move:
				if (this->moveCnt >= 1) {
					nm = Motion::Stand;
				}
				break;

			case Motion::Captured:
				break;

			case Motion::Unnon:
				break;
			}
		}
		//モーション更新
		this->UpdateMotion(nm);
	}

	//-------------------------------------------------------------------
	//モーションに対応した処理
	//（モーションは変更しない）
	void Object::Move()
	{
		switch (this->motion) {
		case Motion::Stand:
			break;

		case Motion::Select:
			break;

		case Motion::Move:
			if (this->moveCnt == 0) {
				//自分自身を取得
				auto me = this->self.lock();

				//将棋盤タスクを取得
				auto board = ge->GetTask<Board::Object>(
					Board::defGroupName,
					Board::defName);

				if (board == nullptr) {
					return;
				}

				//移動先にある駒を取得
				auto targetKoma = board->GetKoma(
					this->targetBoardX,
					this->targetBoardY);

				//相手の駒がいる場合
				if (targetKoma != nullptr) {
					//カオスモード判定（仮）
					if (Title::Object::gameMode == Title::Object::GameMode::Random) {
						this->RandomEvent();
					}
					if (targetKoma->owner != this->owner) {
						//取った駒を自分の持ち駒に加える
						board->AddHand(
							targetKoma->type,
							this->owner);

						//相手の駒タスクを消す
						targetKoma->Kill();

					}
				}

				//移動前のマスを空にする
				board->ClearKoma(
					this->boardX,
					this->boardY);

				//移動先のマスを空にする
				board->ClearKoma(
					this->targetBoardX,
					this->targetBoardY);

				//移動先へ位置を変更
				this->SetBoardPosition(
					this->targetBoardX,
					this->targetBoardY);


				if (me == nullptr) {
					return;
				}

				//移動後のマスに自分を登録
				board->SetKoma(
					this->boardX,
					this->boardY,
					me);

				//成り判定
				if (this->CheckNari() == true) {
					if (Title::Object::gameMode == Title::Object::GameMode::Normal) {
						this->nari = true;
					}
					else if (me->type == Type::Fu || me->type == Type::Kei || me->type == Type::Kyo) {
						this->nari = true;
					}
					else {
						this->nari = false;
					}
				}


				//手番を交代
				board->ChangeTurn();
			}
			break;

		case Motion::Captured:
			break;

		case Motion::Unnon:
			break;
		}
	}
	//-------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo Object::Anim()
	{
		ML::Color defColor(1, 1, 1, 1);

		int typeNo = 0;

		switch (this->type) {
		case Type::Ou:
			typeNo = 0;
			break;

		case Type::Hisya:
			typeNo = 1;
			break;

		case Type::Kaku:
			typeNo = 2;
			break;

		case Type::Kin:
			typeNo = 3;
			break;

		case Type::Gin:
			typeNo = 4;
			break;

		case Type::Kei:
			typeNo = 5;
			break;

		case Type::Kyo:
			typeNo = 6;
			break;

		case Type::Fu:
			typeNo = 7;
			break;
		}

		//成っている場合は2段目の画像を使う
		if (this->nari == true) {

			//王と金は成らない
			if (this->type != Type::Ou &&
				this->type != Type::Kin) {

				typeNo += 8;
			}
		}

		//後手画像は3段目・4段目
		if (this->owner == Owner::Gote) {
			typeNo += 16;
		}

		int srcX = (typeNo % 8) * 64;
		int srcY = (typeNo / 8) * 64;

		BChara::DrawInfo rtv;

		rtv.draw = ML::Box2D(0, 0, 64, 64);
		rtv.src = ML::Box2D(srcX, srcY, 64, 64);
		rtv.color = defColor;

		if (this->motion == Motion::Select) {
			rtv.color = ML::Color(1, 1, 0, 1);
		}

		return rtv;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void Object::Render2D_AF()
	{
		if (this->motion == Motion::Captured) {
			return;
		}

		if (this->motion == Motion::Unnon) {
			return;
		}

		


		//駒の描画
		BChara::DrawInfo di = this->Anim();

		di.draw.Offset(this->pos);

		this->res->img->Draw(
			di.draw,
			di.src,
			di.color);
	}
	

	void Object::RandomEvent() {
		auto me = this->self.lock();


		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}


		switch (rand()%5)
		{
		case 0:
			board->randomEventText = "駒変化！";
			board->randomEventCnt = 180;
			if (me->type != Type::Ou) {
				me->type = RandomKoma();
			}
			else {
				return;
			}
			break;
		case 1:
		{
		
		
			//-----------------------------------------------------------
			//成り進化イベント

			//王・金・すでに成っている駒は対象外
			if (this->type == Type::Ou ||
				this->type == Type::Kin ||
				this->nari == true)
			{
				break;
			}

			//駒の種類は変えずに成らせる
			this->nari = true;

			//イベント画像を表示
			board->randomEventText = "成り進化！";
			board->randomEventCnt = 180;

			break;
		}
		case 2:
			//-----------------------------------------------------------
			//相手の制限時間を5秒減らす
			//手番交代後に適用する
			board->randomEventText = "相手-5秒";
			board->randomEventCnt = 180;
			Object::timePenalty = 5;
			break;

		case 3:
		{
			//-----------------------------------------------------------
			//ランダムな持ち駒を1枚入手
			Type addType = RandomKoma();

			board->randomEventText = "持ち駒GET!";
			board->randomEventCnt = 180;

			//持ち駒を追加
			board->AddHand(addType, this->owner);

			//イベントで入手した駒を記録
			board->eventHandType = board->TypeToNo(addType);

			//入手したプレイヤーを記録
			if (this->owner == Owner::Sente)
			{
				board->eventHandOwner = 0;
			}
			else
			{
				board->eventHandOwner = 1;
			}

			//1ターンだけ色を変更
			board->eventHandTurn = 2;

			break;
		}

		case 4:
			//-----------------------------------------------------------
			//現在いる縦列を2ターン後に破壊
			board->randomEventText = "2ターン後列破壊";
			board->randomEventCnt = 180;

			//爆弾を発動した側を記録
			Object::bombOwner = board->turn;

			Object::destroyColumn = this->boardX;
			Object::destroyTurn = 3;
			break;
		}
	}

	//ランダムに駒を選ぶ
	Object::Type Object::RandomKoma() {
		//王以外のランダムな駒を返す
		switch (rand() % 7)
		{
		case 0: return Type::Hisya;
			break;
		case 1: return Type::Kaku;
			break;
		case 2: return Type::Kin;
			break;
		case 3: return Type::Gin;
			break;
		case 4: return Type::Kei;
			break;
		case 5: return Type::Kyo;
			break;
		case 6: return Type::Fu;
			break;
		}
	}
	//-------------------------------------------------------------------
	//手番交代時のランダムイベント処理
	void Object::RandomEventTurn()
	{
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		//---------------------------------------------------------------
		//相手の時間を減らす
		if (Object::timePenalty > 0)
		{
			auto game = ge->GetTask<Game::Object>(
				Game::defGroupName,
				Game::defName);

			if (game != nullptr)
			{
				game->timeLimit -=
					Object::timePenalty * 60;

				if (game->timeLimit < 0) {
					game->timeLimit = 0;
				}
			}

			//一度適用したので解除
			Object::timePenalty = 0;
		}

		//---------------------------------------------------------------
		//縦列破壊の予約がある
		if (Object::destroyColumn >= 0)
		{
			Object::destroyTurn--;

			//まだ2ターン経過していない
			if (Object::destroyTurn > 0) {
				return;
			}


			//列破壊SEを再生
			se::Play("bom");


			//-----------------------------------------------------------
			//予約された縦列の駒を全て消す
			for (int y = 0; y < 9; y++)
			{
				auto koma = board->GetKoma(
					Object::destroyColumn,
					y);

				if (koma == nullptr) {
					continue;
				}

				//爆発エフェクトの表示位置
				ML::Vec2 pos;

				pos.x = 270 + 16 + Object::destroyColumn * 64 + 32;


				const int boardTop = 40;
				const int margin = 16;
				const int cellSize = 64;

				pos.y = boardTop + margin + y * cellSize + cellSize / 2 - 5;

				ge->CreateEffect(8, pos);


				//盤面配列から消す
				board->ClearKoma(
					Object::destroyColumn,
					y);

				//駒タスクを消す
				koma->Kill();
			}

			//縦列破壊の予約を解除
			Object::destroyColumn = -1;
			Object::destroyTurn = 0;
		}
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP Object::Create(bool flagGameEnginePushBack_)
	{
		

		Object::SP ob = Object::SP(new Object());
		if (ob) {
			ob->me = ob;

			//自分自身を保持
			ob->self = ob;

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
	Object::~Object()
	{
		this->B_Finalize();
	}

	bool Object::B_Finalize()
	{
		auto rtv = this->Finalize();
		return rtv;
	}

	//-------------------------------------------------------------------
	Object::Object()
	{
	}

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
	Resource::Resource()
	{
	}

	//-------------------------------------------------------------------
	Resource::~Resource()
	{
		this->Finalize();
	}
}