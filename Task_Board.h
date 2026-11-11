#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//迷路マップ
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Board
{
    const string defGroupName("本編");
    const string defName("マップ");

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

        DG::Image::SP img;
    };

    //-------------------------------------------------------------------
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
        //マップの表示位置
        int boardX;
        int boardY;

        //1マスの大きさ
        int cellSize;

        //マップサイズ
        static const int MAP_W = 20;
        static const int MAP_H = 15;

        //マップデータ
        //0：通路
        //1：壁
        int map[MAP_H][MAP_W];

        //マップ初期化
        void Board_Initialize();

        //指定したマスが壁かどうか
        bool IsWall(int x, int y);
    };
}