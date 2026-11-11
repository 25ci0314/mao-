//-------------------------------------------------------------------
//迷路マップ
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Board.h"

namespace Board
{
    Resource::WP Resource::instance;

    //-------------------------------------------------------------------
    //リソースの初期化
    bool Resource::Initialize()
    {
        //マップ画像は後から設定する
        this->img = DG::Image::Create("./data/image/MapChip.png");
        return true;
    }

    //-------------------------------------------------------------------
    //リソースの解放
    bool Resource::Finalize()
    {
        this->img.reset();

        return true;
    }

    //-------------------------------------------------------------------
    //「初期化」タスク生成時に１回だけ行う処理
    bool Object::Initialize()
    {
        //スーパークラス初期化
        __super::Initialize(defGroupName, defName, true);

        //リソースクラス生成orリソース共有
        this->res = Resource::Create();

        //描画優先度
        this->render2D_Priority[1] = 0.9f;

        //マップの左上座標
        this->boardX = 100;
        this->boardY = 50;

        //1マスの大きさ
        this->cellSize = 32;

        //マップデータを初期化
        this->Board_Initialize();

        return true;
    }

    //-------------------------------------------------------------------
    //「終了」タスク消滅時に１回だけ行う処理
    bool Object::Finalize()
    {
        //★データ＆タスク解放

        if (!ge->QuitFlag() && this->nextTaskCreate)
        {
            //★引き継ぎタスクの生成
        }

        return true;
    }

    //-------------------------------------------------------------------
    //「更新」１フレーム毎に行う処理
    void Object::UpDate()
    {
        //マップは現在、更新処理なし
    }

    //-------------------------------------------------------------------
    //「２Ｄ描画」１フレーム毎に行う処理
    void Object::Render2D_AF()
    {
        for (int y = 0; y < MAP_H; y++)
        {
            for (int x = 0; x < MAP_W; x++)
            {
                int drawX = this->boardX + x * this->cellSize;
                int drawY = this->boardY + y * this->cellSize;

                ML::Box2D draw(
                    drawX,
                    drawY,
                    this->cellSize,
                    this->cellSize
                );

                //壁だけ描画
                if (this->map[y][x] == 1)
                {
                    ML::Box2D src(12, 74, 128, 135);
                    this->res->img->Draw(draw, src);
                }
            }
        }
    }

    //-------------------------------------------------------------------
    //マップデータを初期化
    void Object::Board_Initialize()
    {
        int stage[MAP_H][MAP_W] =
        {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
            {1,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1},
            {1,0,1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1},
            {1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
            {1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
            {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
            {1,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        };

        for (int y = 0; y < MAP_H; y++)
        {
            for (int x = 0; x < MAP_W; x++)
            {
                this->map[y][x] = stage[y][x];
            }
        }
    }

    //-------------------------------------------------------------------
    //指定したマスが壁かどうか
    bool Object::IsWall(int x, int y)
    {
        //マップの範囲外
        if (x < 0 || x >= MAP_W ||
            y < 0 || y >= MAP_H)
        {
            return true;
        }

        //壁の場合
        if (this->map[y][x] == 1)
        {
            return true;
        }

        //通路の場合
        return false;
    }

    //★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    //以下は基本的に変更不要なメソッド
    //★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

    //-------------------------------------------------------------------
    //タスク生成窓口
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

    //-------------------------------------------------------------------
    bool Object::B_Initialize()
    {
        return this->Initialize();
    }

    //-------------------------------------------------------------------
    Object::~Object()
    {
        this->B_Finalize();
    }

    //-------------------------------------------------------------------
    bool Object::B_Finalize()
    {
        auto rtv = this->Finalize();

        return rtv;
    }

    //-------------------------------------------------------------------
    Object::Object()
    {
    }

    //-------------------------------------------------------------------
    //リソースクラスの生成
    Resource::SP Resource::Create()
    {
        if (auto sp = instance.lock())
        {
            return sp;
        }
        else
        {
            sp = Resource::SP(new Resource());

            if (sp)
            {
                sp->Initialize();
                instance = sp;
            }

            return sp;
        }
    }

    //-------------------------------------------------------------------
    Resource::Resource()
    {
    }

    //-------------------------------------------------------------------
    Resource::~Resource()
    {
        this->Finalize();
    }
}