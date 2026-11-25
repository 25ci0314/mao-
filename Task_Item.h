#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//アイテム・荷物
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Item
{
    const string defGroupName("本編");
    const string defName("アイテム");

    //アイテムの種類
    enum ItemType
    {
        Item_Bomb,       //爆弾
        Item_Stop,       //動きを止める
        Item_Speed,      //スピードアップ
        Item_Power,      //攻撃力アップ
        Item_Package     //荷物
    };

    //アイテムの情報
    struct ItemData
    {
        ItemType type;

        //現在のマス
        int mapX;
        int mapY;

        //アイテムが存在するか
        bool active;

        //荷物の価値
        int value;

        //荷物の番号（1～5）
        int number;

        //荷物の配達先
        int deliveryX;
        int deliveryY;
    };

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

        //荷物の画像
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

        //アイテムの最大数
        static const int MAX_ITEMS = 100;

        //アイテムの情報
        ItemData items[MAX_ITEMS];

        //なくなった荷物の数
        int removedPackageCount;

        //補充する荷物の数
        int pendingPackageCount;

        //アイテム生成タイマー
        int spawnTimer;

        //勇者がアイテムを持っているか
        bool heroHasItem;

        //勇者が持っているアイテム
        ItemType heroItem;

        //動きを止める時間
        int stopTimer;

        //スピードアップ時間
        int speedTimer;

        //攻撃力アップ時間
        int powerTimer;

        //爆弾が設置されているか
        bool bombActive;

        //爆弾のマス座標
        int bombX;
        int bombY;

        //爆弾の爆発タイマー
        int bombTimer;

        //荷物を生成
        bool CreatePackage();

        //勇者専用アイテムを生成
        void CreateRandomItem();

        //荷物が配達または消滅したとき
        void RemovePackage();

        //勇者がアイテムを拾う
        void PickupHeroItem(int x, int y);

        //勇者がアイテムを使用する
        void UseHeroItem(int x, int y);

        //爆弾の爆発判定
        bool CheckBombExplosion(int x, int y);

        //魔王が停止しているか
        bool IsStopped();

        //勇者がスピードアップ中か
        bool IsSpeedUp();

        //勇者が攻撃力アップ中か
        bool IsPowerUp();

        //アイテムを生成できる場所か
        bool CanSpawn(int x, int y);

        //通路かどうか
        bool IsRoad(int x, int y);

        //ランダムな生成場所を探す
        bool FindSpawnPoint(int& x, int& y);

        //荷物の最大数
        static const int MAX_PACKAGE = 5;

        //指定した番号の荷物が存在するか
        bool IsPackageNumber(int number_);

        //指定した配達先が使用されているか
        bool IsDeliveryUsed(int x, int y);
    };
}