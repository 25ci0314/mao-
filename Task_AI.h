#pragma warning(disable:4996)
#pragma once

//-----------------------------------------------------------------------------
//勇者AI
//巡回・BFS追跡・攻撃判断
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "Task_Enemy.h"

namespace AI
{
    class Object : public BTask
    {
    public:
        virtual ~Object();

        typedef shared_ptr<Object> SP;
        typedef weak_ptr<Object> WP;

        static Object::SP Create(bool flagGameEnginePushBack_);

    private:
        Object();

        bool B_Initialize();
        bool B_Finalize();

        bool Initialize();
        void UpDate() override;
        bool Finalize();

    public:

        //勇者
        Enemy::Object::SP enemy;

        //魔王のマス座標
        int playerX;
        int playerY;

        //追跡中かどうか
        bool chasing;

        //BFS用配列
        int distance[15][38];

        //探索用配列
        int queueX[570];
        int queueY[570];

        //探索開始位置
        int startX;
        int startY;

        //探索目標位置
        int goalX;
        int goalY;

        //次の移動先
        int nextX;
        int nextY;

        //AI更新カウンター
        int aiCnt;

        //各マスを訪れた回数
        int visitCount[15][38];

        //巡回先
        int patrolX;
        int patrolY;

        //巡回先が決まっているか
        bool patrolTarget;

        //魔王の位置を設定
        void SetPlayerPosition(int x, int y);

        //BFS探索
        bool SearchRoute();

        //巡回
        void Patrol();

        //巡回先を決める
        void SetPatrolTarget();

        //追跡
        void Chase();

        //攻撃判断
        void CheckAttack();

        //移動先を設定
        void SetNextMove(int x, int y);

        //通行可能か判定
        bool CheckMove(int x, int y);
    };
}