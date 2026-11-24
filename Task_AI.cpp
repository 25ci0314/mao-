//-----------------------------------------------------------------------------
//勇者AI
//マップ全体の巡回・BFS追跡・攻撃判断
//-----------------------------------------------------------------------------
#include "MyPG.h"
#include "Task_AI.h"
#include "Task_Board.h"
#include "Task_Player.h"

namespace AI
{
    //-------------------------------------------------------------------------
    //初期化
    //-------------------------------------------------------------------------
    bool Object::Initialize()
    {
        BTask::Initialize("本編", "勇者AI", true);

        //勇者を取得
        this->enemy = ge->GetTask<Enemy::Object>(
            Enemy::defGroupName,
            Enemy::defName
        );

        //魔王の初期位置
        this->playerX = 0;
        this->playerY = 0;

        //追跡状態
        this->chasing = false;

        //AIカウンター
        this->aiCnt = 0;

        //探索位置
        this->startX = 0;
        this->startY = 0;

        //目標位置
        this->goalX = 0;
        this->goalY = 0;

        //次の移動先
        this->nextX = 0;
        this->nextY = 0;

        //巡回先
        this->patrolX = 0;
        this->patrolY = 0;

        this->patrolTarget = false;

        //訪問回数を初期化
        for (int y = 0; y < 15; y++)
        {
            for (int x = 0; x < 38; x++)
            {
                this->visitCount[y][x] = 0;
            }
        }

        return true;
    }

    //-------------------------------------------------------------------------
    //終了処理
    //-------------------------------------------------------------------------
    bool Object::Finalize()
    {
        this->enemy.reset();

        return true;
    }

    //-------------------------------------------------------------------------
    //魔王の位置を設定
    //-------------------------------------------------------------------------
    void Object::SetPlayerPosition(int x, int y)
    {
        this->playerX = x;
        this->playerY = y;
    }

    //-------------------------------------------------------------------------
    //通行可能か判定
    //-------------------------------------------------------------------------
    bool Object::CheckMove(int x, int y)
    {
        //マップ外
        if (x < 0 || x >= 38)
        {
            return false;
        }

        if (y < 0 || y >= 15)
        {
            return false;
        }

        //マップ取得
        Board::Object::SP board =
            ge->GetTask<Board::Object>(
                Board::defGroupName,
                Board::defName
            );

        if (board == nullptr)
        {
            return false;
        }

        //壁なら通れない
        if (board->IsWall(x, y) == true)
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------
    //BFSで最短経路を探索
    //-------------------------------------------------------------------------
    bool Object::SearchRoute()
    {
        if (this->enemy == nullptr)
        {
            return false;
        }

        //距離を初期化
        for (int y = 0; y < 15; y++)
        {
            for (int x = 0; x < 38; x++)
            {
                this->distance[y][x] = -1;
            }
        }

        //勇者の現在位置
        this->startX = this->enemy->mapX;
        this->startY = this->enemy->mapY;

        //開始位置を確認
        if (this->CheckMove(this->startX, this->startY) == false)
        {
            return false;
        }

        //目標位置を確認
        if (this->CheckMove(this->goalX, this->goalY) == false)
        {
            return false;
        }

        //同じマスの場合
        if (this->startX == this->goalX &&
            this->startY == this->goalY)
        {
            return false;
        }

        int head = 0;
        int tail = 0;

        //開始位置を登録
        this->queueX[tail] = this->startX;
        this->queueY[tail] = this->startY;

        tail++;

        this->distance[this->startY][this->startX] = 0;

        //上下左右
        int dx[4] = { 0, -1, 1, 0 };
        int dy[4] = { 1, 0, 0, -1 };

        //BFS探索
        while (head < tail)
        {
            int x = this->queueX[head];
            int y = this->queueY[head];

            head++;

            //目標に到着
            if (x == this->goalX &&
                y == this->goalY)
            {
                break;
            }

            //4方向を探索
            for (int i = 0; i < 4; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (this->CheckMove(nx, ny) == false)
                {
                    continue;
                }

                //探索済み
                if (this->distance[ny][nx] != -1)
                {
                    continue;
                }

                //距離を記録
                this->distance[ny][nx] =
                    this->distance[y][x] + 1;

                //キューに追加
                if (tail < 570)
                {
                    this->queueX[tail] = nx;
                    this->queueY[tail] = ny;

                    tail++;
                }
            }
        }

        //経路が見つからない場合
        if (this->distance[this->goalY][this->goalX] == -1)
        {
            return false;
        }

        //目標から逆にたどる
        int x = this->goalX;
        int y = this->goalY;

        while (this->distance[y][x] > 1)
        {
            bool found = false;

            for (int i = 0; i < 4; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (this->CheckMove(nx, ny) == false)
                {
                    continue;
                }

                if (this->distance[ny][nx] ==
                    this->distance[y][x] - 1)
                {
                    x = nx;
                    y = ny;

                    found = true;
                    break;
                }
            }

            if (found == false)
            {
                return false;
            }
        }

        //次の移動先
        this->nextX = x;
        this->nextY = y;

        return true;
    }

    //-------------------------------------------------------------------------
    //巡回先を決定
    //訪問回数が少ないマスを優先する
    //-------------------------------------------------------------------------
    void Object::SetPatrolTarget()
    {
        if (this->enemy == nullptr)
        {
            return;
        }

        //現在位置
        int nowX = this->enemy->mapX;
        int nowY = this->enemy->mapY;

        //現在位置から各マスへの距離をBFSで調べる
        for (int y = 0; y < 15; y++)
        {
            for (int x = 0; x < 38; x++)
            {
                this->distance[y][x] = -1;
            }
        }

        if (this->CheckMove(nowX, nowY) == false)
        {
            return;
        }

        int head = 0;
        int tail = 0;

        this->queueX[tail] = nowX;
        this->queueY[tail] = nowY;
        tail++;

        this->distance[nowY][nowX] = 0;

        int dx[4] = { 0, -1, 1, 0 };
        int dy[4] = { 1, 0, 0, -1 };

        while (head < tail)
        {
            int x = this->queueX[head];
            int y = this->queueY[head];

            head++;

            for (int i = 0; i < 4; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (this->CheckMove(nx, ny) == false)
                {
                    continue;
                }

                if (this->distance[ny][nx] != -1)
                {
                    continue;
                }

                this->distance[ny][nx] =
                    this->distance[y][x] + 1;

                if (tail < 570)
                {
                    this->queueX[tail] = nx;
                    this->queueY[tail] = ny;
                    tail++;
                }
            }
        }

        //-------------------------------------------------------------
        //訪問回数が少なく、遠くにあるマスを選ぶ
        //-------------------------------------------------------------
        int bestCount = 1000000;
        int bestDistance = -1;

        int bestX = nowX;
        int bestY = nowY;

        bool found = false;

        for (int y = 0; y < 15; y++)
        {
            for (int x = 0; x < 38; x++)
            {
                //到達できないマスは除外
                if (this->distance[y][x] == -1)
                {
                    continue;
                }

                //現在位置は除外
                if (x == nowX && y == nowY)
                {
                    continue;
                }

                int count = this->visitCount[y][x];
                int dist = this->distance[y][x];

                //訪問回数が少ないマスを優先
                if (count < bestCount)
                {
                    bestCount = count;
                    bestDistance = dist;

                    bestX = x;
                    bestY = y;

                    found = true;
                }
                //訪問回数が同じなら遠いマス
                else if (count == bestCount &&
                    dist > bestDistance)
                {
                    bestDistance = dist;

                    bestX = x;
                    bestY = y;

                    found = true;
                }
            }
        }

        //巡回先を設定
        if (found == true)
        {
            this->patrolX = bestX;
            this->patrolY = bestY;

            this->patrolTarget = true;
        }
        else
        {
            this->patrolTarget = false;
        }
    }

    //-------------------------------------------------------------------------
    //勇者の移動方向を設定
    //-------------------------------------------------------------------------
    void Object::SetNextMove(int x, int y)
    {
        if (this->enemy == nullptr)
        {
            return;
        }

        //移動中は変更しない
        if (this->enemy->moving == true)
        {
            return;
        }

        //移動できない状態
        if (this->enemy->CanMove() == false)
        {
            return;
        }

        //現在位置
        int nowX = this->enemy->mapX;
        int nowY = this->enemy->mapY;

        //移動方向
        int dx = x - nowX;
        int dy = y - nowY;

        //上下左右の1マスだけ
        if (dx == 1 || dx == -1)
        {
            if (dy != 0)
            {
                return;
            }
        }
        else if (dy == 1 || dy == -1)
        {
            if (dx != 0)
            {
                return;
            }
        }
        else
        {
            return;
        }

        //BCharaの移動処理
        this->enemy->SetMoveDirection(dx, dy);
    }

    //-------------------------------------------------------------------------
    //巡回処理
    //-------------------------------------------------------------------------
    void Object::Patrol()
    {
        if (this->enemy == nullptr)
        {
            return;
        }

        if (this->enemy->moving == true)
        {
            return;
        }

        //現在位置
        int x = this->enemy->mapX;
        int y = this->enemy->mapY;

        //現在のマスの訪問回数を増やす
        this->visitCount[y][x]++;

        //巡回先に到着した場合
        if (this->patrolTarget == true)
        {
            if (x == this->patrolX &&
                y == this->patrolY)
            {
                this->patrolTarget = false;
            }
        }

        //巡回先がない場合は決める
        if (this->patrolTarget == false)
        {
            this->SetPatrolTarget();
        }

        if (this->patrolTarget == false)
        {
            return;
        }

        //巡回先をBFSの目標にする
        this->goalX = this->patrolX;
        this->goalY = this->patrolY;

        //最短経路で巡回先へ進む
        if (this->SearchRoute() == true)
        {
            this->SetNextMove(
                this->nextX,
                this->nextY
            );
        }
        else
        {
            //経路が見つからなければ別の場所を選ぶ
            this->patrolTarget = false;
        }
    }

    //-------------------------------------------------------------------------
    //魔王を追跡
    //-------------------------------------------------------------------------
    void Object::Chase()
    {
        if (this->enemy == nullptr)
        {
            return;
        }

        if (this->enemy->moving == true)
        {
            return;
        }

        //魔王の位置を目標にする
        this->goalX = this->playerX;
        this->goalY = this->playerY;

        //BFS探索
        if (this->SearchRoute() == true)
        {
            this->SetNextMove(
                this->nextX,
                this->nextY
            );
        }
    }

    //-------------------------------------------------------------------------
    //攻撃判断
    //-------------------------------------------------------------------------
    void Object::CheckAttack()
    {
        if (this->enemy == nullptr)
        {
            return;
        }

        if (this->enemy->IsAttackReady() == false)
        {
            return;
        }

        int dx = this->playerX - this->enemy->mapX;
        int dy = this->playerY - this->enemy->mapY;

        //下
        if (dx == 0 && dy == 1)
        {
            this->enemy->direction = BChara::Direction::Down;
            this->enemy->StartEnemyAttack();
        }
        //上
        else if (dx == 0 && dy == -1)
        {
            this->enemy->direction = BChara::Direction::Up;
            this->enemy->StartEnemyAttack();
        }
        //左
        else if (dx == -1 && dy == 0)
        {
            this->enemy->direction = BChara::Direction::Left;
            this->enemy->StartEnemyAttack();
        }
        //右
        else if (dx == 1 && dy == 0)
        {
            this->enemy->direction = BChara::Direction::Right;
            this->enemy->StartEnemyAttack();
        }
    }

    //-------------------------------------------------------------------------
    //毎フレーム更新
    //-------------------------------------------------------------------------
    void Object::UpDate()
    {
        //勇者を取得
        if (this->enemy == nullptr)
        {
            this->enemy = ge->GetTask<Enemy::Object>(
                Enemy::defGroupName,
                Enemy::defName
            );
        }

        if (this->enemy == nullptr)
        {
            return;
        }

        //AI操作が無効なら停止
        if (this->enemy->aiControl == false)
        {
            return;
        }

        //-------------------------------------------------------------
        //魔王の位置を取得
        //-------------------------------------------------------------
        Player::Object::SP player =
            ge->GetTask<Player::Object>(
                Player::defGroupName,
                Player::defName
            );

        if (player == nullptr)
        {
            return;
        }

        this->SetPlayerPosition(
            player->mapX,
            player->mapY
        );

        //-------------------------------------------------------------
        //攻撃判断
        //-------------------------------------------------------------
        this->CheckAttack();

        //攻撃中・硬直中は移動しない
        if (this->enemy->CanMove() == false)
        {
            return;
        }

        //移動中は次の指示を出さない
        if (this->enemy->moving == true)
        {
            return;
        }

        //-------------------------------------------------------------
        //魔王との距離
        //-------------------------------------------------------------
        int dx = this->playerX - this->enemy->mapX;
        int dy = this->playerY - this->enemy->mapY;

        if (dx < 0)
        {
            dx = -dx;
        }

        if (dy < 0)
        {
            dy = -dy;
        }

        int dist = dx + dy;

        //-------------------------------------------------------------
        //6マス以内なら追跡
        //-------------------------------------------------------------
        if (dist <= 6)
        {
            this->chasing = true;

            //巡回先を解除
            this->patrolTarget = false;
        }
        else
        {
            this->chasing = false;
        }

        //-------------------------------------------------------------
        //行動を決定
        //-------------------------------------------------------------
        if (this->chasing == true)
        {
            this->Chase();
        }
        else
        {
            this->Patrol();
        }

        this->aiCnt++;
    }

    //-------------------------------------------------------------------------
    //タスク生成
    //-------------------------------------------------------------------------
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

    //-------------------------------------------------------------------------
    //初期化呼び出し
    //-------------------------------------------------------------------------
    bool Object::B_Initialize()
    {
        return this->Initialize();
    }

    //-------------------------------------------------------------------------
    //デストラクタ
    //-------------------------------------------------------------------------
    Object::~Object()
    {
        this->B_Finalize();
    }

    //-------------------------------------------------------------------------
    //終了処理呼び出し
    //-------------------------------------------------------------------------
    bool Object::B_Finalize()
    {
        return this->Finalize();
    }

    //-------------------------------------------------------------------------
    //コンストラクタ
    //-------------------------------------------------------------------------
    Object::Object()
    {
    }
}