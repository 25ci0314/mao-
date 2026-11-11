//-------------------------------------------------------------------
//魔王プレイヤー
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Player.h"
#include "Task_Board.h"
#include <cmath>

namespace Player
{
    Resource::WP Resource::instance;

    //-------------------------------------------------------------------
    //リソース初期化
    bool Resource::Initialize()
    {
        this->img = DG::Image::Create("./data/image/player.png");

        return true;
    }

    //-------------------------------------------------------------------
    //リソース解放
    bool Resource::Finalize()
    {
        this->img.reset();

        return true;
    }

    //-------------------------------------------------------------------
    //初期化
    bool Object::Initialize()
    {
        __super::Initialize(defGroupName, defName, true);

        this->res = Resource::Create();

        //初期位置
        this->mapX = 1;
        this->mapY = 1;

        this->posX = 100 + mapX * 32;
        this->posY = 50 + mapY * 32;

        this->targetX = mapX;
        this->targetY = mapY;

        //移動速度
        this->speed = 3.0f;

        this->moving = false;

        //入力方向
        this->inputX = 0;
        this->inputY = 0;

        //魔王の向き
        //0=下 1=左 2=右 3=上
        this->direction = 0;

        //アニメーション
        this->animFrame = 0;
        this->animCnt = 0;

        //描画優先度
        this->render2D_Priority[1] = 1.0f;

        return true;
    }

    //-------------------------------------------------------------------
    //更新
    void Object::UpDate()
    {
        auto board = ge->GetTask<Board::Object>(
            Board::defGroupName,
            Board::defName
        );

        if (board == nullptr)
        {
            return;
        }

        //---------------------------------------------------------------
        //入力取得
        auto inp = ge->in1->GetState();

        //左
        if (inp.LStick.BL.on)
        {
            inputX = -1;
            inputY = 0;
        }
        //右
        else if (inp.LStick.BR.on)
        {
            inputX = 1;
            inputY = 0;
        }
        //上
        else if (inp.LStick.BU.on)
        {
            inputX = 0;
            inputY = -1;
        }
        //下
        else if (inp.LStick.BD.on)
        {
            inputX = 0;
            inputY = 1;
        }

        //---------------------------------------------------------------
        //移動していない場合
        if (moving == false)
        {
            //向きを変更
            if (inputX == -1)
            {
                direction = 1;
            }
            else if (inputX == 1)
            {
                direction = 2;
            }
            else if (inputY == -1)
            {
                direction = 3;
            }
            else if (inputY == 1)
            {
                direction = 0;
            }

            //次のマス
            int nextX = mapX + inputX;
            int nextY = mapY + inputY;

            //入力があり、壁がなければ移動
            if ((inputX != 0 || inputY != 0) &&
                board->IsWall(nextX, nextY) == false)
            {
                targetX = nextX;
                targetY = nextY;

                moving = true;
            }
        }

        //---------------------------------------------------------------
        //移動中
        if (moving == true)
        {
            float goalX =
                board->boardX + targetX * board->cellSize;

            float goalY =
                board->boardY + targetY * board->cellSize;

            //-----------------------------------------------------------
            //X方向移動
            if (posX < goalX)
            {
                posX += speed;

                if (posX > goalX)
                {
                    posX = goalX;
                }
            }
            else if (posX > goalX)
            {
                posX -= speed;

                if (posX < goalX)
                {
                    posX = goalX;
                }
            }

            //-----------------------------------------------------------
            //Y方向移動
            if (posY < goalY)
            {
                posY += speed;

                if (posY > goalY)
                {
                    posY = goalY;
                }
            }
            else if (posY > goalY)
            {
                posY -= speed;

                if (posY < goalY)
                {
                    posY = goalY;
                }
            }

            //-----------------------------------------------------------
            //目的地に到着
            if (posX == goalX && posY == goalY)
            {
                mapX = targetX;
                mapY = targetY;

                moving = false;
            }
        }

        //---------------------------------------------------------------
        //歩行アニメーション

        //次のマスへ進めるか確認
        bool canContinue = false;

        if (inputX != 0 || inputY != 0)
        {
            int nextX = mapX + inputX;
            int nextY = mapY + inputY;

            canContinue = !board->IsWall(nextX, nextY);
        }

        //移動中、または次のマスへ進める場合
        if (moving || canContinue)
        {
            animCnt++;

            //3フレームごとに切り替え
            if (animCnt >= 3)
            {
                animCnt = 0;

                animFrame++;

                if (animFrame >= 4)
                {
                    animFrame = 0;
                }
            }
        }
        else
        {
            animFrame = 0;
            animCnt = 0;
        }
    }
    //-------------------------------------------------------------------
    //「２Ｄ描画」１フレーム毎に行う処理
    void Object::Render2D_AF()
    {
        //---------------------------------------------------------------
        //マップ取得
        auto board = ge->GetTask<Board::Object>(
            Board::defGroupName,
            Board::defName
        );

        if (board == nullptr)
        {
            return;
        }

        //画像がない場合は描画しない
        if (this->res == nullptr || this->res->img == nullptr)
        {
            return;
        }

        //---------------------------------------------------------------
        //魔王の描画位置
        ML::Box2D draw(
            this->posX,
            this->posY,
            board->cellSize,
            board->cellSize
        );

        //---------------------------------------------------------------
        //画像サイズ：2172 × 724
        //
        // direction
        // 0 = 下
        // 1 = 左
        // 2 = 右
        // 3 = 上
        //
        // animFrame
        // 0 ～ 3

        //---------------------------------------------------------------
        //各コマのX座標
        const int srcX[4][4] =
        {
            { 599, 851, 1111, 1369 },  //下
            { 606, 868, 1128, 1385 },  //左
            { 602, 859, 1123, 1384 },  //右
            { 611, 869, 1129, 1389 }   //上
        };

        //---------------------------------------------------------------
        //各コマのY座標
        const int srcY[4][4] =
        {
            { 15, 15, 15, 16 },         //下
            { 194, 194, 194, 196 },     //左
            { 370, 370, 370, 372 },     //右
            { 547, 547, 547, 547 }      //上
        };

        //---------------------------------------------------------------
        //各コマの横幅
        const int srcW[4][4] =
        {
            { 182, 184, 188, 182 },    //下
            { 189, 188, 191, 184 },    //左
            { 185, 187, 185, 187 },    //右
            { 175, 175, 174, 178 }     //上
        };

        //---------------------------------------------------------------
        //各コマの高さ
        const int srcH[4][4] =
        {
            { 163, 163, 163, 162 },    //下
            { 161, 161, 161, 160 },    //左
            { 160, 160, 160, 158 },    //右
            { 165, 165, 165, 165 }     //上
        };

        //---------------------------------------------------------------
        //現在の方向とアニメーション番号
        int dir = this->direction;
        int frame = this->animFrame;

        //範囲外防止
        if (dir < 0 || dir >= 4)
        {
            dir = 0;
        }

        if (frame < 0 || frame >= 4)
        {
            frame = 0;
        }

        //---------------------------------------------------------------
        //切り取り範囲
        //画像の端が欠けないように少し余白を追加したかったけどむり
        const int margin = 3;

        ML::Box2D src(
            srcX[dir][frame] - margin,
            srcY[dir][frame] - margin,
            srcW[dir][frame] + margin * 2,
            srcH[dir][frame] + margin * 2
        );

        //---------------------------------------------------------------
        //魔王描画
        this->res->img->Draw(draw, src);
    }
    //-------------------------------------------------------------------
    //終了
    bool Object::Finalize()
    {
        return true;
    }

    //-------------------------------------------------------------------
    //タスク生成
    Object::SP Object::Create(bool flagGameEnginePushBack_)
    {
        Object::SP ob = Object::SP(new Object());

        if (ob)
        {
            ob->me = ob;

            if (flagGameEnginePushBack_)
            {
                ge->PushBack(ob);
            }

            if (!ob->B_Initialize())
            {
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

    //-------------------------------------------------------------------
    bool Object::B_Finalize()
    {
        return this->Finalize();
    }

    //-------------------------------------------------------------------
    Object::Object()
    {
    }

    //-------------------------------------------------------------------
    //リソース生成
    Resource::SP Resource::Create()
    {
        if (auto sp = instance.lock())
        {
            return sp;
        }
        else
        {
            sp = Resource::SP(new Resource());

            if (sp)
            {
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