#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace Game
{
    //タスクに割り当てるグループ名と固有名
    const string defGroupName("本編");
    const string defName("統括");

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

        //共有する変数はここに追加する
        DG::Image::SP img, imgSE, imgGO, imgTE, imgNO,
            imgBA, imgSU, imgNKR, imgBY;
    };

    //-------------------------------------------------------------------
    class Object : public BTask
    {
        //変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
    public:
        virtual ~Object();

        typedef shared_ptr<Object> SP;
        typedef weak_ptr<Object> WP;

        //生成窓口 引数はtrueでタスクシステムへ自動登録
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

        //変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
    public:

        //追加したい変数・メソッドはここに追加する
        DG::Font::SP TestFont;

        //制限時間（フレーム）
        const int limit = 60 * 60 * 3;

        //残り時間（フレーム）
        int time;

        //ゲーム終了フラグ
        bool gameEnd;

        bool cpuBattle;

        //---------------------------------------------------------------
        //所持金・借金関係
        //---------------------------------------------------------------

        //魔王の所持金
        int money;

        //魔王の借金
        int debt;

        //初期借金額
        static const int INITIAL_DEBT = 10000;

        //荷物を持っていない状態で被弾した際の治療費
        static const int MEDICAL_COST = 1000;

        //残りの返済額を取得
        int GetRemainingDebt();

        //借金を増やす
        void AddDebt(int amount_);

        //---------------------------------------------------------------
        //勝敗判定
        //---------------------------------------------------------------

        //勝敗を確認する
        void CheckWinner();

        //ゲーム終了処理
        void GameFinish(int winner_);

        //---------------------------------------------------------------
        //エンディングへの引き継ぎ
        //---------------------------------------------------------------

        //終了時の残り時間（秒）
        static int resultTime;

        //終了時の所持金
        static int resultMoney;

        //終了時の借金
        static int resultDebt;

        //終了時の残り返済額
        static int resultRemainingDebt;
    };
}