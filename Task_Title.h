#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  Title
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("title");	//グループ名
	const  string  defName("NoName");			//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;

	public:
		Resource();
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
		DG::Image::SP	img;//背景画像
		DG::Image::SP	kyanseru; //キャンセルボタン画像
		DG::Image::SP	randommode;//モード選択時のランダムモード
		DG::Image::SP	randommode1;//現在のモードがランダムモードであることを表示する画像
		DG::Image::SP	randommode2;//現在のモードがランダムモードであることを表示する画像
		DG::Image::SP	randommode3;//現在のモードがランダムモードであることを表示する画像
		DG::Image::SP	tuuzyoumode;//モード選択時の通常モード
		DG::Image::SP	tuuzyoumode1;//現在のモードが通常モードであることを表示する画像
		DG::Image::SP	tuuzyoumode2;//通常モードの二人対戦の確認画面
		DG::Image::SP	tuuzyoumode3;//通常モードのCPU戦の確認画面
		DG::Image::SP	state;//運ゲーか通常かを分ける
		XI::Mouse::SP   mouse;//マウスを獲得
		DG::Image::SP	TitleLogo01;
		DG::Image::SP	TitleLogo02;
		DG::Image::SP	TitleLogo03;
		DG::Image::SP	StartLogo;//ボタンを押してね画像
		DG::Image::SP	mode;//モード選択画像
		DG::Image::SP	twobattle;//二人対戦画像
		DG::Image::SP	cpubattle;//CPU戦画像
		DG::Image::SP	Yes;//はい画像
		DG::Image::SP	No;//いいえ画像
		DG::Image::SP	BG;//黒背景画像
		DG::Image::SP	setumei;//説明画像
		DG::Image::SP	GB;//緑背景画像
		DG::Font::SP    fontA;//現在の状態を表示(通常モード)
		DG::Font::SP    fontB;//現在の状態を表示(ランダムモード)
		DG::Font::SP    fontC;//通常モードの二人対戦で開始しますか?
		DG::Font::SP    fontD;//通常モードのCPU対戦で開始しますか?
		DG::Font::SP    fontE;//ランダムモードの二人対戦で開始しますか?
		DG::Font::SP    fontF;//ランダムモードのCPU対戦で開始しますか?
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;	//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();	//「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		XI::GamePad::SP controller;

		float bgmSpeed = 1.f;
		int bgmVol = 100;

		int seSpeed = 100;
		int seVol = 100;

		long tVol = 100;

		enum class GameMode
		{
			Normal,//通常モード
			Random,//ランダムモード
		};

		static GameMode gameMode;

		DG::Font::SP TestFont;
	};
}