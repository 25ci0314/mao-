#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//将棋盤
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "Task_Koma.h"

namespace Board
{
	//タスクに割り当てるグループ名と固有名
	const string defGroupName("本編");	//グループ名
	const string defName("将棋盤");		//タスク名

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
		DG::Image::SP imgKoma;
		DG::Image::SP bomImg;
		DG::Image::SP tatami;
		DG::Image::SP komadai;
		DG::Image::SP sakura;
		DG::Image::SP imgRed;
		DG::Image::SP imghenge; //駒変化
		DG::Image::SP imgNari;       //成り進化
		DG::Image::SP imgTimeMinus;  //相手-5秒
		DG::Image::SP imgHandGet;    //持ち駒GET
		DG::Image::SP imgBombEvent;  //2ターン後列破壊
	};

	//-------------------------------------------------------------------
	class Object : public BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual ~Object();

		typedef shared_ptr<Object> SP;
		typedef weak_ptr<Object> WP;

		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static Object::SP Create(bool flagGameEnginePushBack_);

		Resource::SP res;

	private:
		Object();

		bool B_Initialize();
		bool B_Finalize();

		bool Initialize();				//「初期化」タスク生成時に１回だけ行う処理
		void UpDate() override;			//「更新」１フレーム毎に行う処理
		void Render2D_AF() override;	//「２Ｄ描画」１フレーム毎に行う処理
		bool Finalize();				//「終了」タスク消滅時に１回だけ行う処理

		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		int boardX;		//盤の左上X
		int boardY;		//盤の左上Y
		int cellSize;	//1マスの大きさ

		//盤面上の駒を管理
		Koma::Object::WP board[9][9];

		//盤面配列を初期化
		void Board_Initialize();

		//指定したマスの駒を取得
		Koma::Object::SP GetKoma(int boardX_, int boardY_);

		//指定したマスに駒を登録
		void SetKoma(
			int boardX_,
			int boardY_,
			Koma::Object::SP koma_);

		//指定したマスを空にする
		void ClearKoma(int boardX_, int boardY_);

		//現在の手番
		//0：先手　1：後手
		int turn;

		//手番を交代する
		void ChangeTurn();

		//持ち駒
		//0：先手　1：後手
		//駒の種類8種類
		int handKoma[2][8];

		//現在選択している持ち駒
		//-1なら選択していない
		int selectHandType;

		//持ち駒表示用フォント
		DG::Font::SP handFont;

		//持ち駒を初期化
		void Hand_Initialize();

		//持ち駒を増やす
		void AddHand(
			Koma::Object::Type type_,
			Koma::Object::Owner owner_);

		//持ち駒を1枚使用する
		bool UseHand(
			Koma::Object::Type type_,
			Koma::Object::Owner owner_);

		//駒の種類を配列番号に変換
		int TypeToNo(Koma::Object::Type type_);

		//配列番号を駒の種類に変換
		Koma::Object::Type NoToType(int no_);

		//イベントで入手した持ち駒の種類
		int eventHandType = -1;

		//イベントで入手したプレイヤー
		int eventHandOwner = -1;

		//色を戻すまでの残りターン
		int eventHandTurn = 0;

		//持ち駒を盤上に配置
		void PutHandKoma(
			int boardX_,
			int boardY_,
			Koma::Object::Type type_,
			Koma::Object::Owner owner_);

		//現在発動中のランダム効果
		std::string randomEventText;

		//表示時間
		int randomEventCnt;
	};
}