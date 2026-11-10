//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "BChara.h"
#include  "MyPG.h"


//-----------------------------------------------------------------------------
//勝者
int BChara::winner = -1;

//-----------------------------------------------------------------------------
//モーションを更新（変更なしの場合 false）
bool BChara::UpdateMotion(Motion nm_)
{
	if (nm_ == this->motion) {
		return false;
	}
	else {
		this->motion = nm_;
		this->moveCnt = 0;
		this->animCnt = 0;
		return true;
	}
}
//-----------------------------------------------------------------------------
//移動可能マスを初期化
void BChara::ClearMovePossible()
{
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			this->movePossible[y][x] = 0;
		}
	}
}

//-----------------------------------------------------------------------------
//盤面の範囲外か判定
bool BChara::CheckBoardOut(int boardX_, int boardY_)
{
	if (boardX_ < 0 || boardX_ >= 9) {
		return true;
	}

	if (boardY_ < 0 || boardY_ >= 9) {
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//盤上座標を画面座標に変換
ML::Vec2 BChara::BoardToScreen(int boardX_, int boardY_)
{
	ML::Vec2 rtv;

	const int boardLeft = 270;
	const int boardTop = 40;
	const int margin = 16;
	const int cellSize = 64;

	rtv.x =
		boardLeft +
		margin +
		boardX_ * cellSize;

	rtv.y =
		boardTop +
		margin +
		boardY_ * cellSize -
		5;

	return rtv;
}

//-----------------------------------------------------------------------------
//盤上の位置を設定
void BChara::SetBoardPosition(int boardX_, int boardY_)
{
	//盤上の位置を変更
	this->boardX = boardX_;
	this->boardY = boardY_;

	//画面座標を変更
	this->pos =
		this->BoardToScreen(
			this->boardX,
			this->boardY);
}

//-----------------------------------------------------------------------------
//移動先の位置を設定
void BChara::SetTargetPosition(int boardX_, int boardY_)
{
	this->targetBoardX = boardX_;
	this->targetBoardY = boardY_;
}