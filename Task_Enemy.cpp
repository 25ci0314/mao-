#include "MyPG.h"
#include "Task_Enemy.h"

namespace Enemy
{
    Resource::WP Resource::instance;

    // ==============================
    // リソースのコンストラクタ
    // ==============================
    Resource::Resource()
    {
    }

    // ==============================
    // リソースのデストラクタ
    // ==============================
    Resource::~Resource()
    {
        Finalize();
    }

    // ==============================
    // リソースの作成
    // ==============================
    Resource::SP Resource::Create()
    {
        Resource::SP sp = instance.lock();

        if (sp == nullptr)
        {
            sp = Resource::SP(new Resource());

            if (sp->Initialize() == false)
            {
                return nullptr;
            }

            instance = sp;
        }

        return sp;
    }

    // ==============================
    // 画像の読み込み
    // ==============================
    bool Resource::Initialize()
    {
        // 勇者の歩行画像を読み込む
        img = DG::Image::Create("./data/image/Hero.png");

        // 勇者の攻撃画像を読み込む
        imgAttack = DG::Image::Create("./data/image/Hero_Attack.png");

        // 歩行画像が読み込めなかった場合
        if (img == nullptr)
        {
            return false;
        }

        // 攻撃画像が読み込めなかった場合
        if (imgAttack == nullptr)
        {
            return false;
        }

        return true;
    }

    // ==============================
    // 画像の解放
    // ==============================
    bool Resource::Finalize()
    {
        img.reset();
        imgAttack.reset();

        return true;
    }

    // ==============================
    // 勇者のコンストラクタ
    // ==============================
    Object::Object()
    {
    }

    // ==============================
    // 勇者のデストラクタ
    // ==============================
    Object::~Object()
    {
        B_Finalize();
    }

    // ==============================
    // 勇者の作成
    // ==============================
    Object::SP Object::Create(bool flagGameEnginePushBack_)
    {
        Object::SP ob = Object::SP(new Object());

        ob->me = ob;

        if (flagGameEnginePushBack_)
        {
            ge->PushBack(ob);
        }

        if (ob->B_Initialize() == false)
        {
            ob->Kill();

            return nullptr;
        }

        return ob;
    }

    // ==============================
    // 初期化処理の呼び出し
    // ==============================
    bool Object::B_Initialize()
    {
        return Initialize();
    }

    // ==============================
    // 終了処理の呼び出し
    // ==============================
    bool Object::B_Finalize()
    {
        return Finalize();
    }

    // ==============================
    // 勇者の初期化
    // ==============================
    bool Object::Initialize()
    {
        // 勇者のタスクを初期化する
        if (BTask::Initialize(defGroupName, defName, true) == false)
        {
            return false;
        }

        // 勇者が使用する画像を取得する
        res = Resource::Create();

        if (res == nullptr)
        {
            return false;
        }

        // 勇者の初期位置
        SetPosition(10, 5);

        // 勇者の移動速度
        SetMoveSpeed(2.0f);

        // 最初は下を向く
        direction = Direction::Down;

        // 最初の攻撃方向も下にする
        attackDirection = Direction::Down;

        // 攻撃にかかる時間
        attackTime = 18;

        // 攻撃後に動けなくなる時間
        recoveryTime = 120;

        // 次の攻撃までの時間
        coolTime = 30;

        // 最初は通常の移動状態にする
        ChangeActionState(ActionState::Move, 0);

        // 攻撃が当たったかどうか
        attackHit = false;

        // 歩行アニメーションの初期化
        walkAnimCnt = 0;
        walkFrame = 0;

        // 移動とアニメーションのカウント
        moveCnt = 0;
        animCnt = 0;

        // 最初は待機モーション
        UpdateMotion(Motion::Idle);

        // AIで勇者を操作する
        aiControl = true;

        return true;
    }

    // ==============================
    // 勇者の更新処理
    // ==============================
    void Object::UpDate()
    {
        // 攻撃状態や硬直時間を更新する
        UpdateActionState();

        // 移動していたかを保存する
        bool wasMoving = moving;

        // 移動できる状態なら移動する
        if (CanMove())
        {
            Move();
        }

        // ==============================
        // 歩行アニメーションの更新
        // ==============================
        if (wasMoving)
        {
            // 移動している間はカウントを増やす
            walkAnimCnt++;

            // 4フレームごとに画像を切り替える
            if (walkAnimCnt >= 4)
            {
                walkAnimCnt = 0;

                walkFrame++;

                // 歩行画像は横に4コマある
                if (walkFrame >= 4)
                {
                    walkFrame = 0;
                }
            }
        }
        else
        {
            // 止まっているときは最初の画像に戻す
            walkAnimCnt = 0;
            walkFrame = 0;
        }

        // ==============================
        // 勇者のモーションを決める
        // ==============================

        // 攻撃している場合
        if (actionState == ActionState::Attack)
        {
            UpdateMotion(Motion::Attack);
        }
        // 移動している場合
        else if (moving)
        {
            UpdateMotion(Motion::Walk);
        }
        // 何もしていない場合
        else
        {
            UpdateMotion(Motion::Idle);
        }
    }

    // ==============================
    // 勇者の描画処理
    // ==============================
    void Object::Render2D_AF()
    {
        // 画像の何行目を使用するか
        int dir = 0;

        // 下向きは1行目
        if (direction == Direction::Down)
        {
            dir = 0;
        }
        // 左向きは2行目
        else if (direction == Direction::Left)
        {
            dir = 1;
        }
        // 右向きは3行目
        else if (direction == Direction::Right)
        {
            dir = 2;
        }
        // 上向きは4行目
        else if (direction == Direction::Up)
        {
            dir = 3;
        }

        // ==============================
        // 攻撃中の描画
        // ==============================
        if (actionState == ActionState::Attack)
        {
            // 攻撃中は向きを変更しない
            if (attackDirection == Direction::Down)
            {
                dir = 0;
            }
            else if (attackDirection == Direction::Left)
            {
                dir = 1;
            }
            else if (attackDirection == Direction::Right)
            {
                dir = 2;
            }
            else if (attackDirection == Direction::Up)
            {
                dir = 3;
            }

            // 攻撃開始から何フレーム経過したか
            int elapsed = attackTime - actionTimer;

            // 攻撃画像の何コマ目を表示するか
            int frame = 0;

            // 攻撃時間を4コマに分ける
            if (attackTime > 0)
            {
                frame = elapsed * 4 / attackTime;
            }

            // 画像の範囲外にならないようにする
            if (frame < 0)
            {
                frame = 0;
            }

            if (frame > 3)
            {
                frame = 3;
            }

            // ==============================
            // 攻撃画像の切り取り位置
            // ==============================

            // 攻撃画像全体は640×680ピクセル
            // 横4コマ、縦4コマ
            // 斬撃が見切れないように個別に指定する

            // X座標、Y座標、横幅、縦幅
            ML::Box2D attackSrc[4][4] =
            {
                // ==========================
                // 下向きの攻撃
                // ==========================
                {
                    ML::Box2D(0,   0, 150, 165),
                    ML::Box2D(150, 0, 155, 165),
                    ML::Box2D(305, 0, 160, 165),
                    ML::Box2D(465, 0, 155, 165)
                },

                // ==========================
                // 左向きの攻撃
                // ==========================
                {
                    ML::Box2D(0,   165, 150, 165),
                    ML::Box2D(150, 165, 145, 165),
                    ML::Box2D(280, 165, 180, 165),
                    ML::Box2D(465, 165, 155, 165)
                },

                // ==========================
                // 右向きの攻撃
                // ==========================
                {
                     ML::Box2D(0,   340, 145, 130),
                     ML::Box2D(145, 340, 150, 130),
                     ML::Box2D(295, 340, 185, 130),
                     ML::Box2D(480, 340, 140, 130)
                },

                // ==========================
                // 上向きの攻撃
                // ==========================
                {
                    ML::Box2D(0,   490, 150, 170),
                    ML::Box2D(150, 490, 155, 170),
                    ML::Box2D(305, 470, 160, 190),
                    ML::Box2D(465, 490, 155, 170)
                }
            };

            // 方向とアニメーション番号から
            // 切り取り範囲を取得する
            ML::Box2D src = attackSrc[dir][frame];

            // ==============================
            // 攻撃中の表示サイズ
            // ==============================

            // 通常時と同じ32×32で表示する
            float attackSize = 32.0f;

            // 通常時の中央に合わせる
            float offset = (32.0f - attackSize) / 2.0f;

            // 攻撃中の表示位置と大きさ
            ML::Box2D attackDraw(
                posX + offset,
                posY + offset,
                attackSize,
                attackSize
            );

            // 攻撃画像を描画する
            res->imgAttack->Draw(
                attackDraw,
                src
            );
        }

        // ==============================
        // 通常時の描画
        // ==============================
        else
        {
            // 止まっているときは0コマ目
            int frame = 0;

            // 移動している場合は歩行アニメーション
            if (moving)
            {
                frame = walkFrame;
            }

            // ==============================
            // 歩行画像の切り取り位置
            // ==============================

            // 歩行画像全体は640×680ピクセル
            // 横4コマ、縦4コマ
            // 1コマは160×170ピクセル

            ML::Box2D src(
                frame * 160,
                dir * 170,
                160,
                170
            );

            // 通常時の表示サイズは32×32
            ML::Box2D draw(
                posX,
                posY,
                32,
                32
            );

            // 歩行画像を描画する
            res->img->Draw(
                draw,
                src
            );
        }
    }

    // ==============================
    // 勇者の攻撃開始
    // ==============================
    void Object::StartEnemyAttack()
    {
        // 攻撃できない状態なら何もしない
        if (IsAttackReady() == false)
        {
            return;
        }

        // 攻撃開始時の方向を保存する
        attackDirection = direction;

        // 新しい攻撃なので当たり判定を戻す
        attackHit = false;

        // 攻撃アニメーションを開始する
        StartAttack(attackTime);
    }

    // ==============================
    // プレイヤーへの攻撃判定
    // ==============================
    bool Object::CheckAttackHit(
        float playerX,
        float playerY,
        bool playerFlying
    )
    {
        // 攻撃中でなければ当たらない
        if (IsAttacking() == false)
        {
            return false;
        }

        // プレイヤーが飛行中なら攻撃は当たらない
        if (playerFlying)
        {
            return false;
        }

        // 同じ攻撃で一度当たっていたら判定しない
        if (attackHit)
        {
            return false;
        }

        // 攻撃開始から何フレーム経過したか
        int attackFrame = attackTime - actionTimer;

        // 攻撃開始直後はまだ当たらない
        if (attackFrame < 5)
        {
            return false;
        }

        // 攻撃を振り終わったら当たらない
        if (attackFrame > 10)
        {
            return false;
        }

        // 勇者から見たプレイヤーの位置
        float dx = playerX - posX;
        float dy = playerY - posY;

        // 攻撃が当たったかどうか
        bool hit = false;

        // ==============================
        // 下向きの攻撃判定
        // ==============================
        if (attackDirection == Direction::Down)
        {
            // 勇者より下側40ピクセル以内
            if (dy >= 0.0f && dy <= 40.0f)
            {
                // 左右24ピクセル以内
                if (dx >= -24.0f && dx <= 24.0f)
                {
                    hit = true;
                }
            }
        }

        // ==============================
        // 上向きの攻撃判定
        // ==============================
        else if (attackDirection == Direction::Up)
        {
            // 勇者より上側40ピクセル以内
            if (dy <= 0.0f && dy >= -40.0f)
            {
                // 左右24ピクセル以内
                if (dx >= -24.0f && dx <= 24.0f)
                {
                    hit = true;
                }
            }
        }

        // ==============================
        // 左向きの攻撃判定
        // ==============================
        else if (attackDirection == Direction::Left)
        {
            // 勇者より左側40ピクセル以内
            if (dx <= 0.0f && dx >= -40.0f)
            {
                // 上下24ピクセル以内
                if (dy >= -24.0f && dy <= 24.0f)
                {
                    hit = true;
                }
            }
        }

        // ==============================
        // 右向きの攻撃判定
        // ==============================
        else if (attackDirection == Direction::Right)
        {
            // 勇者より右側40ピクセル以内
            if (dx >= 0.0f && dx <= 40.0f)
            {
                // 上下24ピクセル以内
                if (dy >= -24.0f && dy <= 24.0f)
                {
                    hit = true;
                }
            }
        }

        // 攻撃が当たった場合
        if (hit)
        {
            // 同じ攻撃で再び当たらないようにする
            attackHit = true;

            return true;
        }

        // 攻撃が当たらなかった場合
        return false;
    }

    // ==============================
    // 攻撃できるか調べる
    // ==============================
    bool Object::IsAttackReady()
    {
        // 移動中は攻撃しない
        if (moving)
        {
            return false;
        }

        // 攻撃可能な状態か調べる
        return CanAttack();
    }

    // ==============================
    // 攻撃中か調べる
    // ==============================
    bool Object::IsAttacking()
    {
        return actionState == ActionState::Attack;
    }

    // ==============================
    // 攻撃後の硬直中か調べる
    // ==============================
    bool Object::IsRecovering()
    {
        return actionState == ActionState::Recovery;
    }

    // ==============================
    // AI操作の設定
    // ==============================
    void Object::SetAIControl(bool flag)
    {
        aiControl = flag;
    }

    // ==============================
    // 勇者の終了処理
    // ==============================
    bool Object::Finalize()
    {
        // 使用していた画像リソースを解放する
        res.reset();

        return true;
    }
}