#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲームUI
//-------------------------------------------------------------------
#include "BChara.h"
#include "Task_StartTimer.h"

namespace UI
{
    //タスクに割り当てるグループ名と固有名
    const string defGroupName("UI"); //グループ名
    const string defName("NoName");  //タスク名

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

        //変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
        //共有する変数はここに追加する

        //フォント画像
        DG::Image::SP font;

        //配達先の吹き出し画像
        DG::Image::SP deliveryImg;

        //残り返済額の文字画像
        DG::Image::SP debtImg;
    };

    //-------------------------------------------------------------------
    class Object : public BChara
    {
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

    public:
        //変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
        //追加したい変数・メソッドはここに追加する
        //BCharaに含まれないモノのみここに追加する

        //タイマー位置調整
        ML::Vec2 tPos;

        //描画サイズ
        const int size = 4;

        //残り時間受け取り変数
        int time;

        //文字変換
        BChara::DrawInfo font_ward(int ward_);

        //数字変換
        BChara::DrawInfo font_number(int num_);

        //残り時間描画
        void Timer_Render(int px_, int py_, int time_);

        //---------------------------------------------------------------
        //所持金・借金UI
        //---------------------------------------------------------------

        //所持金の表示位置
        ML::Vec2 moneyPos;

        //残り返済額の表示位置
        ML::Vec2 debtPos;

        //所持金と残り返済額を描画
        void Money_Render();

        //指定した金額を描画
        void Number_Render(int px_, int py_, int number_);

        //---------------------------------------------------------------
        //配達先UI
        //---------------------------------------------------------------

        //すべての配達先を描画
        void Delivery_Render();

        //指定した配達先に吹き出しを描画
        void DeliveryIcon_Render(int x_, int y_, int number_);
    };
}