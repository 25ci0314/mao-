#pragma warning(disable:4996)
//-------------------------------------------------------------------
//魔王プレイヤー
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Player.h"
#include "Task_Board.h"
#include "Task_Item.h"
#include "Task_Game.h"

namespace Player
{
    Resource::WP Resource::instance;

    //-------------------------------------------------------------------
    //リソース初期化
    //-------------------------------------------------------------------
    bool Resource::Initialize()
    {
        this->img = DG::Image::Create("./data/image/player.png");

        return true;
    }

    //-------------------------------------------------------------------
    //リソース解放
    //-------------------------------------------------------------------
    bool Resource::Finalize()
    {
        this->img.reset();

        return true;
    }

    //-------------------------------------------------------------------
    //初期化
    //-------------------------------------------------------------------
    bool Object::Initialize()
    {
        __super::Initialize(defGroupName, defName, true);

        this->res = Resource::Create();

        //初期位置
        this->mapX = 3;
        this->mapY = 2;

        this->posX = 32 + mapX * 32;
        this->posY = 144 + mapY * 32;

        //移動先
        this->targetX = mapX;
        this->targetY = mapY;

        //移動速度
        this->speed = 3.0f;

        //移動状態
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

        //---------------------------------------------------------------
        //荷物の初期化
        //---------------------------------------------------------------

        //最初は荷物を持っていない
        this->hasPackage = false;

        //荷物の価値
        this->packageValue = 0;

        //荷物の番号
        this->packageNumber = 0;

        //荷物の配達先
        this->packageDeliveryX = -1;
        this->packageDeliveryY = -1;

        //---------------------------------------------------------------
        //荷物の追従関係の初期化
        //---------------------------------------------------------------

        //荷物の描画座標
        this->packagePosX = posX;
        this->packagePosY = posY;

        //魔王の移動経路を初期化
        for (int i = 0; i < FOLLOW_MAX; i++)
        {
            followX[i] = posX;
            followY[i] = posY;
        }

        //---------------------------------------------------------------
        //荷物の再取得防止
        //---------------------------------------------------------------

        this->dropLock = false;

        this->dropX = -1;
        this->dropY = -1;

        //描画優先度
        this->render2D_Priority[1] = 1.0f;

        return true;
    }

    //-------------------------------------------------------------------
    //更新
    //-------------------------------------------------------------------
    void Object::UpDate()
    {
        //マップタスクを取得
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
        //---------------------------------------------------------------
        auto inp = ge->in1->GetState();

        //---------------------------------------------------------------
        //荷物操作
        //---------------------------------------------------------------

        //荷物を持っている場合
        if (hasPackage)
        {
            //B1ボタンで荷物を置く、または配達する
            if (inp.B1.down && !moving)
            {
                PackageControl();
            }
        }

        //---------------------------------------------------------------
        //移動入力
        //---------------------------------------------------------------

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
        //---------------------------------------------------------------
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
        //---------------------------------------------------------------
        if (moving == true)
        {
            //移動先の画面座標
            float goalX =
                board->boardX + targetX * board->cellSize;

            float goalY =
                board->boardY + targetY * board->cellSize;

            //-----------------------------------------------------------
            //X方向移動
            //-----------------------------------------------------------
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
            //-----------------------------------------------------------
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
            //-----------------------------------------------------------
            if (posX == goalX && posY == goalY)
            {
                mapX = targetX;
                mapY = targetY;

                moving = false;
            }
        }

        //---------------------------------------------------------------
        //荷物の再取得防止
        //---------------------------------------------------------------

        //荷物を置いた直後の場合
        if (dropLock)
        {
            //荷物を置いたマスから離れた場合
            if (mapX != dropX || mapY != dropY)
            {
                //再び荷物を拾えるようにする
                dropLock = false;

                dropX = -1;
                dropY = -1;
            }
        }

        //---------------------------------------------------------------
        //荷物の自動取得
        //---------------------------------------------------------------

        //荷物を持っていない場合
        if (!hasPackage && !dropLock)
        {
            //荷物との接触を確認する
            PickupPackage();
        }

        //---------------------------------------------------------------
        //荷物の追従処理
        //---------------------------------------------------------------
        PackageFollow();

        //---------------------------------------------------------------
        //歩行アニメーション
        //---------------------------------------------------------------

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
    //荷物の追従処理
    //-------------------------------------------------------------------
    void Object::PackageFollow()
    {
        //魔王が移動した場合だけ経路を記録する
        //停止中に荷物が魔王と重ならないようにする
        if (followX[0] != posX ||
            followY[0] != posY)
        {
            //-----------------------------------------------------------
            //過去の移動座標を後ろへずらす
            //-----------------------------------------------------------
            for (int i = FOLLOW_MAX - 1; i > 0; i--)
            {
                followX[i] = followX[i - 1];
                followY[i] = followY[i - 1];
            }

            //-----------------------------------------------------------
            //現在の魔王の座標を記録
            //-----------------------------------------------------------
            followX[0] = posX;
            followY[0] = posY;
        }

        //---------------------------------------------------------------
        //荷物を持っている場合
        //---------------------------------------------------------------
        if (hasPackage)
        {
            //魔王が少し前に通った座標を荷物の座標にする
            packagePosX = followX[FOLLOW_MAX - 1];
            packagePosY = followY[FOLLOW_MAX - 1];
        }
    }

    //-------------------------------------------------------------------
    //荷物操作
    //-------------------------------------------------------------------
    void Object::PackageControl()
    {
        //荷物を持っていない場合は何もしない
        if (!hasPackage)
        {
            return;
        }

        //正しい配達先にいる場合
        if (IsDeliveryPoint())
        {
            //荷物を配達する
            DeliveryPackage();
        }
        else
        {
            //荷物を足元に置く
            DropPackage();
        }
    }

    //-------------------------------------------------------------------
    //荷物を拾う
    //-------------------------------------------------------------------
    void Object::PickupPackage()
    {
        //すでに荷物を持っている場合は拾わない
        if (hasPackage)
        {
            return;
        }

        //荷物を置いた直後は拾わない
        if (dropLock)
        {
            return;
        }

        //アイテムタスクを取得
        auto item = ge->GetTask<Item::Object>(
            Item::defGroupName,
            Item::defName
        );

        //マップタスクを取得
        auto board = ge->GetTask<Board::Object>(
            Board::defGroupName,
            Board::defName
        );

        if (item == nullptr || board == nullptr)
        {
            return;
        }

        //---------------------------------------------------------------
        //魔王の中心座標
        //---------------------------------------------------------------
        float playerCenterX = posX + board->cellSize / 2.0f;
        float playerCenterY = posY + board->cellSize / 2.0f;

        //---------------------------------------------------------------
        //マップ上の荷物を探す
        //---------------------------------------------------------------
        for (int i = 0; i < Item::Object::MAX_ITEMS; i++)
        {
            //存在している荷物か確認
            //勇者専用アイテムは拾わない
            if (item->items[i].active &&
                item->items[i].type == Item::Item_Package)
            {
                //-------------------------------------------------------
                //荷物の画面座標
                //-------------------------------------------------------
                float itemX =
                    board->boardX +
                    item->items[i].mapX * board->cellSize;

                float itemY =
                    board->boardY +
                    item->items[i].mapY * board->cellSize;

                //荷物の中心座標
                float itemCenterX =
                    itemX + board->cellSize / 2.0f;

                float itemCenterY =
                    itemY + board->cellSize / 2.0f;

                //-------------------------------------------------------
                //魔王と荷物の距離
                //-------------------------------------------------------
                float dx = playerCenterX - itemCenterX;
                float dy = playerCenterY - itemCenterY;

                //接触判定の範囲
                float hitRange = 20.0f;

                //-------------------------------------------------------
                //魔王と荷物が接触した場合
                //-------------------------------------------------------
                if (dx * dx + dy * dy <= hitRange * hitRange)
                {
                    //荷物を持つ
                    hasPackage = true;

                    //荷物の価値を取得
                    packageValue = item->items[i].value;

                    //荷物の番号を取得
                    packageNumber = item->items[i].number;

                    //荷物の配達先を取得
                    packageDeliveryX =
                        item->items[i].deliveryX;

                    packageDeliveryY =
                        item->items[i].deliveryY;

                    //---------------------------------------------------
                    //荷物の追従座標を初期化
                    //---------------------------------------------------

                    //魔王の現在位置から追従を開始する
                    packagePosX = posX;
                    packagePosY = posY;

                    //移動経路を初期化
                    for (int j = 0; j < FOLLOW_MAX; j++)
                    {
                        followX[j] = posX;
                        followY[j] = posY;
                    }

                    //マップ上から荷物を消す
                    item->items[i].active = false;

                    //荷物は1個しか持てないため終了
                    return;
                }
            }
        }
    }

    //-------------------------------------------------------------------
    //荷物を置く
    //-------------------------------------------------------------------
    void Object::DropPackage()
    {
        //荷物を持っていない場合
        if (!hasPackage)
        {
            return;
        }

        //アイテムタスクを取得
        auto item = ge->GetTask<Item::Object>(
            Item::defGroupName,
            Item::defName
        );

        if (item == nullptr)
        {
            return;
        }

        //現在のマスに荷物を置けるか確認
        //ほかのアイテムがある場合は置かない
        if (!item->CanSpawn(mapX, mapY))
        {
            return;
        }

        //---------------------------------------------------------------
        //空いているアイテム配列を探す
        //---------------------------------------------------------------
        for (int i = 0; i < Item::Object::MAX_ITEMS; i++)
        {
            if (!item->items[i].active)
            {
                //アイテムの種類を荷物にする
                item->items[i].type = Item::Item_Package;

                //魔王の現在位置に荷物を置く
                item->items[i].mapX = mapX;
                item->items[i].mapY = mapY;

                //荷物の価値を維持する
                item->items[i].value = packageValue;

                //荷物の番号を維持する
                item->items[i].number = packageNumber;

                //荷物の配達先を維持する
                item->items[i].deliveryX = packageDeliveryX;
                item->items[i].deliveryY = packageDeliveryY;

                //マップ上に荷物を表示する
                item->items[i].active = true;

                //-------------------------------------------------------
                //荷物の再取得防止
                //-------------------------------------------------------

                //置いた直後に拾わないようにする
                dropLock = true;

                //荷物を置いたマスを記録
                dropX = mapX;
                dropY = mapY;

                //-------------------------------------------------------
                //魔王が持っている荷物の情報をリセット
                //-------------------------------------------------------

                //荷物を手放す
                hasPackage = false;

                //荷物の価値をリセット
                packageValue = 0;

                //荷物の番号をリセット
                packageNumber = 0;

                //荷物の配達先をリセット
                packageDeliveryX = -1;
                packageDeliveryY = -1;

                return;
            }
        }
    }

    //-------------------------------------------------------------------
    //正しい配達先か確認する
    //-------------------------------------------------------------------
    bool Object::IsDeliveryPoint()
    {
        return hasPackage &&
            mapX == packageDeliveryX &&
            mapY == packageDeliveryY;
    }

    //-------------------------------------------------------------------
    //荷物を配達する
    //-------------------------------------------------------------------
    void Object::DeliveryPackage()
    {
        //正しい配達先でなければ配達しない
        if (!IsDeliveryPoint())
        {
            return;
        }

        //ゲーム本編タスクを取得
        auto game = ge->GetTask<Game::Object>(
            Game::defGroupName,
            Game::defName
        );

        //アイテムタスクを取得
        auto item = ge->GetTask<Item::Object>(
            Item::defGroupName,
            Item::defName
        );

        if (game == nullptr || item == nullptr)
        {
            return;
        }

        //---------------------------------------------------------------
        //配達報酬
        //---------------------------------------------------------------

        //残りの価値を所持金に加算する
        game->money += packageValue;

        //---------------------------------------------------------------
        //配達した荷物の情報をリセット
        //---------------------------------------------------------------

        hasPackage = false;

        packageNumber = 0;
        packageValue = 0;

        packageDeliveryX = -1;
        packageDeliveryY = -1;

        //配達された荷物の数を加算する
        item->RemovePackage();
    }

    //-------------------------------------------------------------------
    //攻撃によって荷物の価値を減らす
    //-------------------------------------------------------------------
    void Object::DamagePackage(bool powerUp)
    {
        //荷物を持っていない場合
        if (!hasPackage)
        {
            return;
        }

        //通常攻撃は500G減少
        int damage = 500;

        //攻撃力アップ中は1000G減少
        if (powerUp)
        {
            damage = 1000;
        }

        //荷物の価値を減らす
        packageValue -= damage;

        //---------------------------------------------------------------
        //荷物の価値が0以下になった場合
        //---------------------------------------------------------------
        if (packageValue <= 0)
        {
            //価値が0になった荷物を消滅させる
            hasPackage = false;

            packageNumber = 0;
            packageValue = 0;

            packageDeliveryX = -1;
            packageDeliveryY = -1;

            //アイテムタスクを取得
            auto item = ge->GetTask<Item::Object>(
                Item::defGroupName,
                Item::defName
            );

            if (item != nullptr)
            {
                //破壊された荷物の数を加算する
                item->RemovePackage();
            }
        }
    }

    //-------------------------------------------------------------------
    //「２Ｄ描画」１フレーム毎に行う処理
    //-------------------------------------------------------------------
    void Object::Render2D_AF()
    {
        //---------------------------------------------------------------
        //マップ取得
        //---------------------------------------------------------------
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
        //---------------------------------------------------------------
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
        //---------------------------------------------------------------
        this->res->img->Draw(draw, src);
    }

    //-------------------------------------------------------------------
    //終了
    //-------------------------------------------------------------------
    bool Object::Finalize()
    {
        return true;
    }

    //★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    //以下は基本的に変更不要なメソッド
    //★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

    //-------------------------------------------------------------------
    //タスク生成
    //-------------------------------------------------------------------
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
    //-------------------------------------------------------------------
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