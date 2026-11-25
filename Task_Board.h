// マップ管理：外壁・家・迷路の描画と当たり判定
#pragma warning(disable:4996)
#pragma once
#include "GameEngine_Ver3_83.h"

namespace Board
{
    const string defGroupName("本編");
    const string defName("マップ");

    // マップと家の画像を読み込んで共有するクラス
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

        // 壁タイルの画像
        DG::Image::SP img;

        // 家の画像：ドアが向く方向ごとに1枚ずつ用意
        // Down=上辺、Up=下辺、Right=左辺、Left=右辺
        DG::Image::SP houseDown;
        DG::Image::SP houseUp;
        DG::Image::SP houseLeft;
        DG::Image::SP houseRight;
    };

    // マップ本体：初期化・描画・通行判定を担当
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

        // 画面上のマップ左上座標（ピクセル）
        int boardX;
        int boardY;

        // 1マスの縦横サイズ（ピクセル）
        int cellSize;

        // マップの横・縦のマス数
        static const int MAP_W = 38;
        static const int MAP_H = 15;

        // マップ配列：0=通れる道、1=通れない壁・家
        int map[MAP_H][MAP_W];

        // 外壁・家・迷路の当たり判定を設定
        void Board_Initialize();

        // 指定マスが通行不可か判定（範囲外も不可）
        bool IsWall(int x, int y);

        //---------------------------------------------------------------
        // 配達先
        //---------------------------------------------------------------

        // 配達先の最大数
        static const int MAX_DELIVERY = 14;

        // 配達先の座標
        int deliveryX[MAX_DELIVERY];
        int deliveryY[MAX_DELIVERY];

        // 配達先の初期化
        void Delivery_Initialize();

        // 配達先のマスかどうか
        bool IsDeliveryPoint(int x, int y);
    };
}