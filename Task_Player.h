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

    //-------------------------------------------------------------------
    //リソースクラス
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

        //魔王の画像
        DG::Image::SP img;
    };

    //-------------------------------------------------------------------
    //オブジェクトクラス
    //-------------------------------------------------------------------
    class Object : public BTask
    {
    public:
        virtual ~Object();

        typedef shared_ptr<Object> SP;
        typedef weak_ptr<Object> WP;

        //タスク生成窓口
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

        //---------------------------------------------------------------
        //魔王の移動関係
        //---------------------------------------------------------------

        //現在のマス座標
        int mapX;
        int mapY;

        //現在の描画座標
        float posX;
        float posY;

        //移動先のマス座標
        int targetX;
        int targetY;

        //移動速度
        float speed;

        //移動中かどうか
        bool moving;

        //入力方向
        int inputX;
        int inputY;

        //魔王の向き
        //0=下 1=左 2=右 3=上
        int direction;

        //アニメーション
        int animFrame;
        int animCnt;

        //---------------------------------------------------------------
        //荷物関係
        //---------------------------------------------------------------

        //荷物を持っているか
        bool hasPackage;

        //荷物の価値
        int packageValue;

        //荷物の番号
        int packageNumber;

        //荷物の配達先
        int packageDeliveryX;
        int packageDeliveryY;

        //---------------------------------------------------------------
        //荷物の追従関係
        //---------------------------------------------------------------

        //荷物の描画座標
        float packagePosX;
        float packagePosY;

        //移動経路を記録する最大数
        static const int FOLLOW_MAX = 12;

        //魔王が通った座標を記録する配列
        float followX[FOLLOW_MAX];
        float followY[FOLLOW_MAX];

        //---------------------------------------------------------------
        //荷物の再取得防止
        //---------------------------------------------------------------

        //荷物を置いた直後かどうか
        bool dropLock;

        //荷物を置いたマス
        int dropX;
        int dropY;

        //---------------------------------------------------------------
        //荷物関係の関数
        //---------------------------------------------------------------

        //荷物操作
        void PackageControl();

        //荷物を拾う
        void PickupPackage();

        //荷物を置く
        void DropPackage();

        //荷物を配達する
        void DeliveryPackage();

        //荷物のダメージ処理
        void DamagePackage(bool powerUp);

        //正しい配達先か確認
        bool IsDeliveryPoint();

        //荷物の追従処理
        void PackageFollow();
    };
}