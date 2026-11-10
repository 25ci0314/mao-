#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//駒
//-------------------------------------------------------------------
#include "BChara.h"

namespace Koma
{
	//タスクに割り当てるグループ名と固有名
	const string defGroupName("本編");	//グループ名
	const string defName("駒");		//タスク名

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
		DG::Image::SP moveImg;
	};

	//-------------------------------------------------------------------
	class Object : public BChara
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

		//「初期化」タスク生成時に１回だけ行う処理
		bool Initialize();

		//「更新」１フレーム毎に行う処理
		void UpDate() override;

		//「２Ｄ描画」１フレーム毎に行う処理
		void Render2D_AF() override;

		//「終了」タスク消滅時に１回だけ行う処理
		bool Finalize();

		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:

		//駒の種類
		enum class Type
		{
			Ou,		//王
			Hisya,	//飛車
			Kaku,	//角
			Kin,	//金
			Gin,	//銀
			Kei,	//桂馬
			Kyo,	//香車
			Fu		//歩
		};


		//駒の種類に応じて移動可能マスを設定
		void SetMovePossible();

		//各駒の移動可能マスを設定
		void SetMoveFu();
		void SetMoveKyo();
		void SetMoveKei();
		void SetMoveGin();
		void SetMoveKin();
		void SetMoveKaku();
		void SetMoveHisya();
		void SetMoveOu();

		//指定したマスを移動可能にする
		void SetMovePoint(int x_, int y_);

		//指定方向へ連続移動可能マスを設定
		void SetMoveLine(int addX_, int addY_);

		//手番
		enum class Owner
		{
			Sente,	//先手
			Gote	//後手
		};


		//自分自身を保持する
		Object::WP self;

		//駒の種類
		Type type;

		//先手・後手
		Owner owner;

		//成り状態
		bool nari;

		//成れるか判定
		bool CheckNari();

		//馬の移動可能マスを設定
		void SetMoveUma();

		//龍の移動可能マスを設定
		void SetMoveRyu();

		//思考＆状況判断（ステータス決定）
		void Think();

		//モーションに対応した処理
		void Move();

		//アニメーション制御
		BChara::DrawInfo Anim();

		//ランダムイベント
		void RandomEvent();

		//駒ランダム選定
		static Type RandomKoma();

		//次の手番の制限時間を減らす秒数
		static int timePenalty;

		//2ターン後に破壊する列
		//-1なら予約なし
		static int destroyColumn;

		//縦列破壊までの残りターン数
		static int destroyTurn;

		//爆発者だよん
		static int bombOwner;

		//手番交代時のランダムイベント処理
		static void RandomEventTurn();

		bool GameStart;
	};
}