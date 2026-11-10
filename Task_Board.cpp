//-------------------------------------------------------------------
//将棋盤
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Board.h"
#include "Task_Game.h"
#include "sound.h"
#include "Task_AI.h"


namespace Board
{
    Resource::WP Resource::instance;

    //-------------------------------------------------------------------
    //リソースの初期化
    bool Resource::Initialize()
    {
        this->img = DG::Image::Create("./data/image/board.png");
        this->imgKoma = DG::Image::Create("./data/image/koma.png");
        this->bomImg = DG::Image::Create("./data/image/bom.png");
        this->tatami = DG::Image::Create("./data/image/tatami.png");
        this->komadai = DG::Image::Create("./data/image/komadai.png");
        this->sakura = DG::Image::Create("./data/image/komadai.png");
        this->imgRed = DG::Image::Create("./data/image/Red.png");
        //-----------------------------------------------------------
        //ランダムイベント画像
        this->imghenge = DG::Image::Create("./data/image/henge.png");
        this->imgNari = DG::Image::Create("./data/image/Nari.png");
        this->imgTimeMinus = DG::Image::Create("./data/image/TimeMinus.png");
        this->imgHandGet = DG::Image::Create("./data/image/HandGet.png");
        this->imgBombEvent = DG::Image::Create("./data/image/BombEvent.png");
        return true;
    }

    //-------------------------------------------------------------------
    //リソースの解放
    bool Resource::Finalize()
    {
        this->img.reset();
        this->imgKoma.reset();
        this->bomImg.reset();
        this->tatami.reset();
        this->komadai.reset();
        this->sakura.reset();
        this->imgRed.reset();
        //-----------------------------------------------------------
        //ランダムイベント画像を解放
        this->imghenge.reset();
        this->imgNari.reset();
        this->imgTimeMinus.reset();
        this->imgHandGet.reset();
        this->imgBombEvent.reset();
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


        //★データ初期化
        this->render2D_Priority[1] = 0.9f;

        this->boardX = 270;
        this->boardY = 40;
        this->cellSize = 64;

        //先手から開始
        this->turn = 0;


        //持ち駒を選択していない状態
        this->selectHandType = -1;

        //持ち駒表示用フォント
        this->handFont = DG::Font::Create("HG丸ｺﾞｼｯｸM-PRO", 20, 20);

        //駒を指すSE
        se::LoadFile("sasu", "./data/sound/se/将棋・駒を指す03改良版.wav");

        //★タスクの生成

        //盤面配列を初期化
        this->Board_Initialize();

        //持ち駒を初期化
        this->Hand_Initialize();

        //ランダムイベント表示を消す
        this->randomEventText = "";

        //ランダムイベントの表示時間を0にする
        this->randomEventCnt = 0;

        return true;
    }

    //-------------------------------------------------------------------
    //「終了」タスク消滅時に１回だけ行う処理
    bool Object::Finalize()
    {
        //★データ＆タスク解放

        if (!ge->QuitFlag() && this->nextTaskCreate) {
            //★引き継ぎタスクの生成
        }

        return true;
    }

    //-------------------------------------------------------------------
    //「更新」１フレーム毎に行う処理
    void Object::UpDate()
    {
        auto ms = ge->mouse->GetState();

        //NPC戦で後手の番なら、持ち駒の操作を禁止
      //NPCが存在する場合、後手の持ち駒操作を禁止
        auto ai = ge->GetTask<AI::Object>(
            AI::defGroupName,
            AI::defName);

        if (ai != nullptr && this->turn == 1) {
            return;
        }

        int ownerNo;

        //現在の手番
        if (this->turn == 0) {
            ownerNo = 0;
        }
        else {
            ownerNo = 1;
        }

        //持ち駒の表示開始位置
        int handLeft;
        int handTop;

        //先手
        if (ownerNo == 0) {
            handLeft = 900;
            handTop = 430;
        }
        //後手
        else {
            handLeft = 30;
            handTop = 180;
        }

        //持ち駒を選択
        if (ms.LB.down) {

            //盤面の駒が選択中か調べる
            bool boardKomaSelect = false;

            auto komas = ge->GetTasks<Koma::Object>(
                Koma::defGroupName);

            for (auto it = komas->begin();
                it != komas->end();
                ++it) {

                if ((*it)->motion == BChara::Motion::Select ||
                    (*it)->motion == BChara::Motion::Move) {

                    boardKomaSelect = true;
                    break;
                }
            }

            //盤面の駒を選択中なら持ち駒を選択しない
            if (boardKomaSelect == false) {

                for (int type = 1; type < 8; type++) {

                    int drawX =
                        handLeft + (type % 4) * 48;

                    int drawY =
                        handTop + (type / 4) * 48;

                    ML::Box2D handBox(drawX, drawY, 40, 40);

                    ML::Box2D mouseBox(ms.pos.x, ms.pos.y, 1, 1);

                    if (handBox.Hit(mouseBox)) {

                        if (this->handKoma
                            [ownerNo][type] > 0) {

                            //同じ持ち駒をもう一度クリックしたら選択解除
                            if (this->selectHandType == type) {
                                this->selectHandType = -1;
                            }
                            else {
                                //持ち駒を選択
                                this->selectHandType = type;
                            }

                            return;
                        }
                    }
                }
            }
        }

        //持ち駒を選択している
        if (this->selectHandType >= 0 &&
            ms.LB.down) {

            int mx = ms.pos.x - (this->boardX + 16);

            int my = ms.pos.y - (this->boardY + 16);

            //盤内をクリック
            if (mx >= 0 && mx < 64 * 9 &&
                my >= 0 && my < 64 * 9) {

                int x = mx / 64;
                int y = my / 64;

                Koma::Object::Owner owner;

                if (this->turn == 0) {
                    owner = Koma::Object::Owner::Sente;
                }
                else {
                    owner = Koma::Object::Owner::Gote;
                }

                this->PutHandKoma(
                    x,
                    y,
                    this->NoToType(
                        this->selectHandType),
                    owner);
            }
        }
        if (this->randomEventCnt > 0)
        {
            this->randomEventCnt--;
        }
    }
    //-------------------------------------------------------------------
    //「２Ｄ描画」１フレーム毎に行う処理
    void Object::Render2D_AF()
    {
        //たたみ背景画像
        {
            ML::Box2D  draw(-50, -60, 1380, 860);
            ML::Box2D  src(0, 0, 1536, 1024);

            draw.Offset(0, 0);
            this->res->tatami->Draw(draw, src);
        }
        //左上駒台画像
        {
            ML::Box2D  draw(-20, 100, 300, 300);
            ML::Box2D  src(0, 0, 1254, 1254);

            draw.Offset(0, 0);
            this->res->komadai->Draw(draw, src);
        }
        //右下駒台画像
        {
            ML::Box2D  draw1(845, 300, 300, 300);
            ML::Box2D  src1(0, 0, 1254, 1254);

            draw1.Offset(0, 0);
            this->res->komadai->Draw(draw1, src1);
        }

        ML::Box2D draw(this->boardX, this->boardY, 608, 608);

        ML::Box2D src(0, 0, 1254, 1254);

        this->res->img->Draw(draw, src);

        //---------------------------------------------------------------
       //爆弾の対象列を赤く表示
        if (Koma::Object::destroyColumn >= 0 &&
            Koma::Object::destroyTurn > 0)
        {
            int column = Koma::Object::destroyColumn;

            for (int y = 0; y < 9; y++)
            {
                //マスの左上座標
                float x = this->boardX + 16 + column * 64;
                float posY = this->boardY + 16 + y * 64;

                //マスの枠線より少し内側に表示
                ML::Box2D redDraw(
                    x + 2,
                    posY + 2,
                    60,
                    60
                );

                //Red.pngの実際の画像サイズ
                ML::Box2D redSrc(0, 0, 1263, 1245);

                this->res->imgRed->Draw(
                    redDraw,
                    redSrc,
                    ML::Color(1, 1, 1, 0.5f)
                );
            }
        }

        //---------------------------------------------------------------
        //先手・後手の持ち駒を両方表示
        for (int ownerNo = 0; ownerNo < 2; ownerNo++)
        {
            int handLeft;
            int handTop;

            //先手の持ち駒は右下
            if (ownerNo == 0)
            {
                handLeft = 900;
                handTop = 430;
            }
            //後手の持ち駒は左上
            else
            {
                handLeft = 30;
                handTop = 180;
            }

            //持ち駒を種類ごとに描画
            for (int type = 1; type < 8; type++)
            {
                //持っていない駒は表示しない
                if (this->handKoma[ownerNo][type] <= 0)
                {
                    continue;
                }

                int drawX = handLeft + (type % 4) * 48;
                int drawY = handTop + (type / 4) * 48;

                ML::Box2D draw(drawX, drawY, 40, 40);

                int imageTypeNo = type;

                //後手の駒画像
                if (ownerNo == 1)
                {
                    imageTypeNo += 16;
                }

                int srcX = (imageTypeNo % 8) * 64;
                int srcY = (imageTypeNo / 8) * 64;

                ML::Box2D src(srcX, srcY, 64, 64);

                ML::Color color(1, 1, 1, 1);

                //イベントで入手した持ち駒を緑色にする
                if (this->eventHandTurn > 0 &&
                    this->eventHandOwner == ownerNo &&
                    this->eventHandType == type)
                {
                    color = ML::Color(1.0f, 1.6f, 1.3f, 0.2f);
                }

                //現在の手番の持ち駒だけ選択色を付ける
                if (this->turn == ownerNo &&
                    this->selectHandType == type)
                {
                    color = ML::Color(1, 1, 0, 1);
                }

                //持ち駒を描画
                this->res->imgKoma->Draw(draw, src, color);

                //持ち駒の枚数を表示
                ML::Box2D countBox(
                    drawX + 28,
                    drawY + 20,
                    40,
                    20
                );

                char countText[16];

                sprintf_s(
                    countText,
                    "%d",
                    this->handKoma[ownerNo][type]
                );

                this->handFont->Draw(
                    countBox,
                    countText,
                    ML::Color(1, 1, 1, 1)
                );
            }
        }


        auto komas = ge->GetTasks<Koma::Object>(Koma::defGroupName);

        for (auto it = komas->begin(); it != komas->end(); ++it)
        {
            if ((*it)->motion != BChara::Motion::Select) {
                continue;
            }

            for (int y = 0; y < 9; y++) {
                for (int x = 0; x < 9; x++) {

                    if ((*it)->movePossible[y][x] == 0) {
                        continue;
                    }

                    ML::Box2D draw(270 + 16 + x * 64, 40 + 13 + y * 64, 64, 64);
                    ML::Box2D src(0, 0, 60, 60);

                    (*it)->res->moveImg->Draw(draw, src);
                }
            }

            break;
        }
        //-----------------------------------------------------------
        //ランダムイベント画像表示
        if (this->randomEventCnt > 0)
        {
            DG::Image::SP eventImg = nullptr;

            //駒変化
            if (this->randomEventText == "駒変化！")
            {
                eventImg = this->res->imghenge;
            }
            //成り進化
            else if (this->randomEventText == "成り進化！")
            {
                eventImg = this->res->imgNari;
            }
            //相手-5秒
            else if (this->randomEventText == "相手-5秒")
            {
                eventImg = this->res->imgTimeMinus;
            }
            //持ち駒GET
            else if (this->randomEventText == "持ち駒GET!")
            {
                eventImg = this->res->imgHandGet;
            }
            //2ターン後列破壊
            else if (this->randomEventText == "2ターン後列破壊")
            {
                eventImg = this->res->imgBombEvent;
            }

            //対応する画像を右上に表示
            if (eventImg != nullptr)
            {
                if (eventImg != nullptr)
                {
                    //盤面右側の畳部分に表示
                    ML::Box2D draw(900, 80, 350, 140);

                    ML::Box2D src(0, 0, 1950, 1024);

                    eventImg->Draw(draw, src);
                }
            }
        }
        //---------------------------------------------------------------
        //列破壊の予告を表示
        if (Koma::Object::destroyColumn >= 0 &&
            Koma::Object::destroyTurn > 0)
        {
            //爆弾の鼓動アニメーション
            static int bombAnimCnt = 0;
            bombAnimCnt++;

            //ドクン、ドクンと伸縮
            float pulse = sinf(bombAnimCnt * 0.15f);

            //拡大縮小率（90%～110%）
            float scale = 1.0f + pulse * 0.1f;

            //爆弾の中心座標
            float centerX =
                this->boardX + 16 +
                Koma::Object::destroyColumn * 64 + 40;

            float centerY = this->boardY - 40 + 24;

            //拡大縮小後のサイズ
            float size = 48.0f * scale;

            //中心を固定して拡大縮小
            ML::Box2D draw(
                centerX - size / 2,
                centerY - size / 2,
                size,
                size
            );

            ML::Box2D src(0, 0, 600, 600);

            this->res->bomImg->Draw(draw, src);
        }

    }
    //-------------------------------------------------------------------
    //盤面配列を初期化
    void Object::Board_Initialize()
    {
        for (int y = 0; y < 9; y++) {
            for (int x = 0; x < 9; x++) {
                this->board[y][x].reset();
            }
        }
    }

    //-------------------------------------------------------------------
    //指定したマスの駒を取得
    Koma::Object::SP Object::GetKoma(int boardX_, int boardY_)
    {
        //盤面の範囲外なら取得しない
        if (boardX_ < 0 || boardX_ >= 9) {
            return nullptr;
        }

        if (boardY_ < 0 || boardY_ >= 9) {
            return nullptr;
        }

        //指定したマスの駒を返す
        return this->board[boardY_][boardX_].lock();
    }

    //-------------------------------------------------------------------
    //指定したマスに駒を登録
    void Object::SetKoma(
        int boardX_,
        int boardY_,
        Koma::Object::SP koma_)
    {
        //盤面の範囲外なら登録しない
        if (boardX_ < 0 || boardX_ >= 9) {
            return;
        }

        if (boardY_ < 0 || boardY_ >= 9) {
            return;
        }

        //指定したマスに駒を登録
        this->board[boardY_][boardX_] = koma_;
    }

    //-------------------------------------------------------------------
    //指定したマスを空にする
    void Object::ClearKoma(int boardX_, int boardY_)
    {
        //盤面の範囲外なら処理しない
        if (boardX_ < 0 || boardX_ >= 9) {
            return;
        }

        if (boardY_ < 0 || boardY_ >= 9) {
            return;
        }

        //指定したマスを空にする
        this->board[boardY_][boardX_].reset();
    }
    //-------------------------------------------------------------------
    //手番を交代する
    void Object::ChangeTurn()
    {
        //先手から後手へ
        if (this->turn == 0) {
            this->turn = 1;
        }
        //後手から先手へ
        else {
            this->turn = 0;
        }

        //ゲーム本編を取得
        auto game = ge->GetTask<Game::Object>(
            Game::defGroupName,
            Game::defName);

        if (game != nullptr) {
            //残り時間を20秒に戻す
            game->timeLimit = 20 * 60;
        }

        //ランダムイベントのターン処理
        Koma::Object::RandomEventTurn();

        //-----------------------------------------------------------
        //イベントで入手した持ち駒の色を戻す
        if (this->eventHandTurn > 0)
        {
            this->eventHandTurn--;

            if (this->eventHandTurn == 0)
            {
                this->eventHandType = -1;
                this->eventHandOwner = -1;
            }
        }
    }
    //-------------------------------------------------------------------
    //持ち駒を初期化
    void Object::Hand_Initialize()
    {
        for (int owner = 0; owner < 2; owner++) {
            for (int type = 0; type < 8; type++) {
                this->handKoma[owner][type] = 0;
            }
        }
    }
    //-------------------------------------------------------------------
    //駒の種類を配列番号に変換
    int Object::TypeToNo(Koma::Object::Type type_)
    {
        switch (type_) {
        case Koma::Object::Type::Ou:
            return 0;

        case Koma::Object::Type::Hisya:
            return 1;

        case Koma::Object::Type::Kaku:
            return 2;

        case Koma::Object::Type::Kin:
            return 3;

        case Koma::Object::Type::Gin:
            return 4;

        case Koma::Object::Type::Kei:
            return 5;

        case Koma::Object::Type::Kyo:
            return 6;

        case Koma::Object::Type::Fu:
            return 7;
        }

        return -1;
    }

    //-------------------------------------------------------------------
    //配列番号を駒の種類に変換
    Koma::Object::Type Object::NoToType(int no_)
    {
        switch (no_) {
        case 0:
            return Koma::Object::Type::Ou;

        case 1:
            return Koma::Object::Type::Hisya;

        case 2:
            return Koma::Object::Type::Kaku;

        case 3:
            return Koma::Object::Type::Kin;

        case 4:
            return Koma::Object::Type::Gin;

        case 5:
            return Koma::Object::Type::Kei;

        case 6:
            return Koma::Object::Type::Kyo;

        case 7:
            return Koma::Object::Type::Fu;
        }

        return Koma::Object::Type::Fu;
    }
    //-------------------------------------------------------------------
    //持ち駒を増やす
    void Object::AddHand(
        Koma::Object::Type type_,
        Koma::Object::Owner owner_)
    {
        int ownerNo;
        int typeNo;

        if (owner_ == Koma::Object::Owner::Sente) {
            ownerNo = 0;
        }
        else {
            ownerNo = 1;
        }

        typeNo = this->TypeToNo(type_);

        //王は持ち駒にしない
        if (typeNo <= 0) {
            return;
        }

        this->handKoma[ownerNo][typeNo]++;
    }
    //-------------------------------------------------------------------
    //持ち駒を1枚使用する
    bool Object::UseHand(
        Koma::Object::Type type_,
        Koma::Object::Owner owner_)
    {
        int ownerNo;
        int typeNo;

        if (owner_ == Koma::Object::Owner::Sente) {
            ownerNo = 0;
        }
        else {
            ownerNo = 1;
        }

        typeNo = this->TypeToNo(type_);

        if (typeNo < 0) {
            return false;
        }

        //その駒を持っていない
        if (this->handKoma[ownerNo][typeNo] <= 0) {
            return false;
        }

        this->handKoma[ownerNo][typeNo]--;

        return true;
    }
    //-------------------------------------------------------------------
    //持ち駒を盤上に配置
    void Object::PutHandKoma(
        int boardX_,
        int boardY_,
        Koma::Object::Type type_,
        Koma::Object::Owner owner_)
    {
        //盤外なら配置しない
        if (boardX_ < 0 || boardX_ >= 9) {
            return;
        }

        if (boardY_ < 0 || boardY_ >= 9) {
            return;
        }

        //すでに駒がある
        if (this->GetKoma(boardX_, boardY_) != nullptr) {
            return;
        }

        //持ち駒を1枚使用
        if (this->UseHand(type_, owner_) == false) {
            return;
        }

        //駒タスクを生成
        auto koma = Koma::Object::Create(true);

        //盤上の位置
        koma->boardX = boardX_;
        koma->boardY = boardY_;

        //移動先の位置
        koma->targetBoardX = boardX_;
        koma->targetBoardY = boardY_;

        //駒の種類
        koma->type = type_;

        //所有者
        koma->owner = owner_;

        //持ち駒を打つ場合は成っていない
        koma->nari = false;

        //待機状態
        koma->motion = BChara::Motion::Stand;

        //画面座標
        koma->pos.x = 270 + 16 + boardX_ * 64;

        koma->pos.y = 40 + 16 + boardY_ * 64 - 5;

        //盤面に登録
        this->SetKoma(boardX_, boardY_, koma);

        //持ち駒の選択解除
        this->selectHandType = -1;

        //手番を交代
        this->ChangeTurn();
    }
    //★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    //以下は基本的に変更不要なメソッド
    //★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    //-------------------------------------------------------------------
    //タスク生成窓口
    Object::SP Object::Create(bool flagGameEnginePushBack_)
    {
        Object::SP ob = Object::SP(new Object());
        if (ob) {
            ob->me = ob;
            if (flagGameEnginePushBack_) {
                ge->PushBack(ob);
            }
            if (!ob->B_Initialize()) {
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
    Object::~Object() { this->B_Finalize(); }
    bool Object::B_Finalize()
    {
        auto rtv = this->Finalize();
        return rtv;
    }
    //-------------------------------------------------------------------
    Object::Object() {}
    //-------------------------------------------------------------------
   //-------------------------------------------------------------------
    //リソースクラスの生成
    Resource::SP Resource::Create()
    {
        if (auto sp = instance.lock()) {
            return sp;
        }
        else {
            sp = Resource::SP(new Resource());
            if (sp) {
                sp->Initialize();
                instance = sp;
            }
            return sp;
        }
    }
    //-------------------------------------------------------------------
    Resource::Resource() {}
    //-------------------------------------------------------------------
    Resource::~Resource() { this->Finalize(); }

}


