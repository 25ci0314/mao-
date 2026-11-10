#include "MyPG.h"
#include "Task_AI.h"
#include "Task_Board.h"
#include "Task_Koma.h"
#include "sound.h"

namespace AI
{
	//AIが選んだ駒の位置
	int selectKomaX = -1;
	int selectKomaY = -1;

	//-------------------------------------------------------------------
	//初期化
	bool Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);

		//思考時間を初期化
		this->thinkCnt = 0;

		//駒を指すSE
		se::LoadFile("sasu", "./data/sound/se/将棋・駒を指す03改良版.wav");

		return true;
	}

	//-------------------------------------------------------------------
	//終了
	bool Object::Finalize()
	{
		return true;
	}

	//-------------------------------------------------------------------
	//更新
	void Object::UpDate()
	{
		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		//将棋盤が存在しない
		if (board == nullptr) {
			return;
		}

		//後手の番ではない
		if (board->turn != 1) {
			this->thinkCnt = 0;
			return;
		}

		//AIの思考時間を進める
		this->thinkCnt++;

		//60フレーム待つ
		if (this->thinkCnt < 120) {
			return;
		}

		//AIを動かす
		this->Think();

		//思考時間を戻す
		this->thinkCnt = 0;
	}

	//-------------------------------------------------------------------
	//AI思考
	void Object::Think()
	{
		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return;
		}

		//後手の番ではない
		if (board->turn != 1) {
			return;
		}

		//---------------------------------------------------------------
		//動かす駒を選ぶ
		int komaPosition = this->SelectKoma();

		//動かせる駒がない
		if (komaPosition == -1) {
			return;
		}

		//---------------------------------------------------------------
		//NPCが持ち駒を使用する
		//持ち駒を使用するかランダムに決定（50%）
		if (rand() % 2 == 0) {

			//持ち駒の候補
			int handType[8];
			int handCount = 0;

			//後手の持ち駒を調べる
			for (int i = 0; i < 8; i++) {

				if (board->handKoma[1][i] > 0) {
					handType[handCount] = i;
					handCount++;
				}
			}

			//持ち駒がある場合
			if (handCount > 0) {

				//使用する持ち駒をランダムに選択
				int selectNo = rand() % handCount;
				int typeNo = handType[selectNo];

				auto type = board->NoToType(typeNo);

				//空いているマスを調べる
				int putX[81];
				int putY[81];
				int putCount = 0;

				for (int y = 0; y < 9; y++) {
					for (int x = 0; x < 9; x++) {

						//駒があるマスには打てない
						if (board->GetKoma(x, y) != nullptr) {
							continue;
						}

						putX[putCount] = x;
						putY[putCount] = y;
						putCount++;
					}
				}

				//空いているマスがある場合
				if (putCount > 0) {

					int putNo = rand() % putCount;

					int x = putX[putNo];
					int y = putY[putNo];



					//盤上に持ち駒を配置
					board->PutHandKoma(x, y, type, Koma::Object::Owner::Gote);

					return;



				}
			}
		}
		//動かせる駒がない
		if (komaPosition == -1) {
			return;
		}

		//---------------------------------------------------------------
		//移動先を選ぶ
		int movePosition = this->SelectMove();

		//移動先がない
		if (movePosition == -1) {
			return;
		}

		//移動先
		int moveX = movePosition % 9;
		int moveY = movePosition / 9;

		//選択した駒を取得
		auto koma = board->GetKoma(
			selectKomaX,
			selectKomaY);

		if (koma == nullptr) {
			return;
		}

		//後手の駒ではない
		if (koma->owner != Koma::Object::Owner::Gote) {
			return;
		}

		//---------------------------------------------------------------
		//移動先の駒を取得
		auto targetKoma = board->GetKoma(
			moveX,
			moveY);

		//相手の駒がいる場合
		if (targetKoma != nullptr) {

			if (targetKoma->owner != koma->owner) {

				//取った駒を持ち駒へ追加
				board->AddHand(
					targetKoma->type,
					koma->owner);

				//相手の駒を消す
				targetKoma->Kill();
			}
		}

		//---------------------------------------------------------------
		//移動前のマスを空にする
		board->ClearKoma(
			koma->boardX,
			koma->boardY);

		//移動先を空にする
		board->ClearKoma(
			moveX,
			moveY);

		//---------------------------------------------------------------
		//駒の位置を変更
		koma->SetBoardPosition(
			moveX,
			moveY);

		//移動先も現在位置に合わせる
		koma->SetTargetPosition(
			moveX,
			moveY);

		//盤面に駒を登録
		board->SetKoma(
			moveX,
			moveY,
			koma);

		//---------------------------------------------------------------
		//成り判定
		if (koma->CheckNari() == true) {
			koma->nari = true;
		}

		//移動可能マスを消す
		koma->ClearMovePossible();

		//待機状態に戻す
		koma->motion = BChara::Motion::Stand;
		koma->moveCnt = 0;
		koma->animCnt = 0;

		//---------------------------------------------------------------
		//手番交代
		board->ChangeTurn();
	}
	//-------------------------------------------------------------------
	//移動先を選ぶ
	int Object::SelectMove()
	{
		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return -1;
		}

		//選択した駒を取得
		auto koma = board->GetKoma(
			selectKomaX,
			selectKomaY);

		if (koma == nullptr) {
			return -1;
		}

		//後手の駒ではない
		if (koma->owner != Koma::Object::Owner::Gote) {
			return -1;
		}

		//移動可能マスを設定
		koma->SetMovePossible();

		//通常の移動先候補
		int moveX[81];
		int moveY[81];
		int moveCount = 0;

		//相手の駒を取れる移動先候補
		int attackX[81];
		int attackY[81];
		int attackCount = 0;

		//---------------------------------------------------------------
		//移動可能マスを調べる
		for (int y = 0; y < 9; y++) {
			for (int x = 0; x < 9; x++) {

				//移動できないマス
				if (koma->movePossible[y][x] != 1) {
					continue;
				}

				//通常の移動先候補に追加
				moveX[moveCount] = x;
				moveY[moveCount] = y;
				moveCount++;

				//-------------------------------------------------------
				//移動先に相手の駒がいるか調べる
				auto targetKoma = board->GetKoma(x, y);

				if (targetKoma != nullptr) {

					if (targetKoma->owner ==
						Koma::Object::Owner::Sente) {

						//取れるマスを候補に追加
						attackX[attackCount] = x;
						attackY[attackCount] = y;
						attackCount++;
					}
				}
			}
		}

		//---------------------------------------------------------------
		//相手の駒を取れる場合
		if (attackCount > 0) {

			//取れるマスからランダムに選択
			int selectNo = rand() % attackCount;

			int targetX = attackX[selectNo];
			int targetY = attackY[selectNo];

			return targetY * 9 + targetX;
		}

		//---------------------------------------------------------------
		//取れる駒がない場合
		if (moveCount == 0) {
			return -1;
		}

		//通常の移動先からランダムに選択
		int selectNo = rand() % moveCount;

		int targetX = moveX[selectNo];
		int targetY = moveY[selectNo];

		return targetY * 9 + targetX;
	}
	//-------------------------------------------------------------------
	//動かす駒を選ぶ
	//-------------------------------------------------------------------
	int Object::SelectKoma()
	{
		//将棋盤タスクを取得
		auto board = ge->GetTask<Board::Object>(
			Board::defGroupName,
			Board::defName);

		if (board == nullptr) {
			return -1;
		}

		//動かせる駒の座標
		int komaX[81];
		int komaY[81];
		int komaCount = 0;

		//相手の駒を取れる駒の座標
		int attackX[81];
		int attackY[81];
		int attackCount = 0;

		//---------------------------------------------------------------
		//盤上の後手の駒を調べる
		for (int y = 0; y < 9; y++) {
			for (int x = 0; x < 9; x++) {

				auto koma = board->GetKoma(x, y);

				//駒がない
				if (koma == nullptr) {
					continue;
				}

				//後手の駒ではない
				if (koma->owner != Koma::Object::Owner::Gote) {
					continue;
				}

				//移動可能マスを設定
				koma->SetMovePossible();

				bool canMove = false;
				bool canAttack = false;

				//-------------------------------------------------------
				//移動できるマスを調べる
				for (int moveY = 0; moveY < 9; moveY++) {
					for (int moveX = 0; moveX < 9; moveX++) {

						//移動できない
						if (koma->movePossible[moveY][moveX] != 1) {
							continue;
						}

						canMove = true;

						//移動先の駒を取得
						auto targetKoma = board->GetKoma(
							moveX,
							moveY);

						//相手の駒がいる
						if (targetKoma != nullptr) {

							if (targetKoma->owner ==
								Koma::Object::Owner::Sente) {

								canAttack = true;
							}
						}
					}
				}

				//動けない駒は候補に入れない
				if (canMove == false) {
					continue;
				}

				//通常の移動候補に追加
				komaX[komaCount] = x;
				komaY[komaCount] = y;
				komaCount++;

				//相手の駒を取れる場合
				if (canAttack == true) {

					attackX[attackCount] = x;
					attackY[attackCount] = y;
					attackCount++;
				}
			}
		}

		//---------------------------------------------------------------
		//相手の駒を取れる場合は優先する
		if (attackCount > 0) {

			int selectNo = rand() % attackCount;

			selectKomaX = attackX[selectNo];
			selectKomaY = attackY[selectNo];
			se::Play("sasu");
			return selectKomaY * 9 + selectKomaX;
		}

		//---------------------------------------------------------------
		//取れる駒がない場合はランダムに選ぶ
		if (komaCount == 0) {
			return -1;
		}

		int selectNo = rand() % komaCount;

		selectKomaX = komaX[selectNo];
		selectKomaY = komaY[selectNo];
		se::Play("sasu");
		return selectKomaY * 9 + selectKomaX;
	}
	//-------------------------------------------------------------------
	//タスク生成
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
	//初期化窓口
	bool Object::B_Initialize()
	{
		return this->Initialize();
	}

	//-------------------------------------------------------------------
	//終了窓口
	bool Object::B_Finalize()
	{
		return this->Finalize();
	}

	//-------------------------------------------------------------------
	//コンストラクタ
	Object::Object()
	{
	}

	//-------------------------------------------------------------------
	//デストラクタ
	Object::~Object()
	{
		this->B_Finalize();
	}
}