#pragma warning(disable:4996)
#pragma once

#include "GameEngine_Ver3_83.h"
#include "BChara.h"

namespace Enemy
{
    const string defGroupName("本編");
    const string defName("勇者");

    // 勇者が使用する画像を管理するクラス
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

        // 歩行アニメーションの画像
        DG::Image::SP img;

        // 攻撃アニメーションの画像
        DG::Image::SP imgAttack;
    };

    // 勇者を管理するクラス
    class Object : public BChara
    {
    public:
        virtual ~Object();

        typedef shared_ptr<Object> SP;
        typedef weak_ptr<Object> WP;

        static Object::SP Create(bool flagGameEnginePushBack_);

        Resource::SP res;

    private:
        Object();

        bool B_Initialize();
        bool B_Finalize();

        bool Initialize();

        void UpDate() override;
        void Render2D_AF() override;

        bool Finalize();

    public:
        // AIによって勇者を操作するか
        bool aiControl;

        // 同じ攻撃で何度も当たらないようにする
        bool attackHit;

        // 攻撃を開始したときの向き
        Direction attackDirection;

        // 歩行アニメーション用
        int walkAnimCnt;
        int walkFrame;

        // 攻撃開始
        void StartEnemyAttack();

        // プレイヤーに攻撃が当たったか調べる
        bool CheckAttackHit(
            float playerX,
            float playerY,
            bool playerFlying
        );

        // 攻撃できるか調べる
        bool IsAttackReady();

        // 攻撃中か調べる
        bool IsAttacking();

        // 攻撃後の硬直中か調べる
        bool IsRecovering();

        // AI操作の設定
        void SetAIControl(bool flag);
    };
}
