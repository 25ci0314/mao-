#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//魔王プレイヤー
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Player
{
    const string defGroupName("本編");
    const string defName("魔王");

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

        DG::Image::SP img;
    };

    class Object : public BTask
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
        //現在のマス
        int mapX;
        int mapY;

        //現在の画面座標
        float posX;
        float posY;

        //移動先のマス
        int targetX;
        int targetY;

        //移動速度
        float speed;

        //移動中かどうか
        bool moving;

        //入力された方向
        int inputX;
        int inputY;

        // 魔王が向いている方向
        // 0 = 下、1 = 左、2 = 右、3 = 上
        int direction;

        int animFrame;  // アニメーションのコマ番号（0～3）
        int animCnt;    // アニメーションの時間管理

    };
}

