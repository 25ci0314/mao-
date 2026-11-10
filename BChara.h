#pragma once
#pragma warning(disable:4996)

//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数


	static int winner;
	//勝者
	//-1：未決定
	// 0：先手
	// 1：後手

	enum class Motion
	{
		Unnon = -1,	//無効
		Stand,		//盤上で待機
		Select,		//選択中
		Move,		//移動中
		Captured	//持ち駒
	};

	struct DrawInfo
	{
		ML::Box2D draw;
		ML::Box2D src;
		ML::Color color;
	};

	//駒の画面座標
	ML::Vec2 pos;

	//移動先までの移動量
	ML::Vec2 moveVec;

	//盤上の位置
	int boardX;
	int boardY;

	//移動先の位置
	int targetBoardX;
	int targetBoardY;

	//行動状態
	Motion motion;


	//カウンタ
	int moveCnt;
	int animCnt;

	//移動できるマス
	int			movePossible[9][9];

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加する事★★
	BChara()
		: pos(0, 0)
		, moveVec(0, 0)
		, boardX(0)
		, boardY(0)
		, targetBoardX(0)
		, targetBoardY(0)
		, motion(Motion::Stand)
		, moveCnt(0)
		, animCnt(0)
		, movePossible{
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0}
		}
	{
	}
	virtual  ~BChara() {}

	//モーションを更新
	bool UpdateMotion(Motion nm_);

	//移動可能マスを初期化
	void ClearMovePossible();

	//盤面の範囲外か判定
	bool CheckBoardOut(int boardX_, int boardY_);

	//盤上座標を画面座標に変換
	ML::Vec2 BoardToScreen(int boardX_, int boardY_);

	//盤上の位置を設定
	void SetBoardPosition(int boardX_, int boardY_);

	//移動先の位置を設定
	void SetTargetPosition(int boardX_, int boardY_);
};
