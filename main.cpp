#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QPainterPath>
#include <QPixmap>
#include <QTimer>
#include <QSet>
#include <QVector>
#include <QList>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <algorithm>

class Wall : public QGraphicsRectItem
{
public:
    Wall(qreal x, qreal y, qreal w, qreal h)
        : QGraphicsRectItem(0, 0, w, h)
    {
        setPos(x, y);
        setBrush(QBrush(QColor(95, 80, 60)));
        setPen(QPen(QColor(60, 50, 35), 2));
        setZValue(5);
    }
};

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    enum Facing { Down = 0, Left = 1, Right = 2, Up = 3 };

    Player(int id, QPointF spawn, const QString& spritePath, QObject* parent = nullptr)
        : QObject(parent), m_id(id), m_spawn(spawn)
    {
        loadFrames(spritePath);
        setPixmap(m_frames[m_facing][0]);
        setPos(m_spawn);
        setZValue(10);
    }

    void setKeys(int up, int down, int left, int right)
    {
        m_keyUp = up;  m_keyDown = down;  m_keyLeft = left;  m_keyRight = right;
    }
    void setSpawn(QPointF spawn) { m_spawn = spawn; }

    int  id() const        { return m_id; }
    int  health() const    { return m_health; }
    int  maxHealth() const { return m_maxHealth; }

    void update(const QSet<int>& keys)
    {
        qreal dx = 0, dy = 0;
        if (keys.contains(m_keyLeft))  { dx -= m_speed; m_facing = Left;  }
        if (keys.contains(m_keyRight)) { dx += m_speed; m_facing = Right; }
        if (keys.contains(m_keyUp))    { dy -= m_speed; m_facing = Up;    }
        if (keys.contains(m_keyDown))  { dy += m_speed; m_facing = Down;  }

        bool moving = (dx != 0 || dy != 0);

        if (moving) {
            tryMove(dx, 0);
            tryMove(0, dy);
        }

        if (m_damageCooldown > 0)
            --m_damageCooldown;

        advanceAnimation(moving);
        updateBlink();
    }

    void takeDamage(int amount)
    {
        if (amount <= 0 || m_health <= 0)
            return;
        if (m_damageCooldown > 0)
            return;

        m_health = std::max(0, m_health - amount);
        m_damageCooldown = kDamageCooldownTicks;
        emit healthChanged(m_health);

        if (m_health == 0)
            emit died();
    }

    void reset()
    {
        m_health = m_maxHealth;
        m_damageCooldown = 0;

        setPos(m_spawn);

        m_facing = Down;
        m_frameIndex = 0;
        m_frameCounter = 0;
        setPixmap(m_frames[m_facing][0]);
        setOpacity(1.0);

        emit healthChanged(m_health);
    }

    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(8, 4, 16, 26);
        return path;
    }

signals:
    void healthChanged(int newHealth);
    void died();

private:
    int m_id;

    int     m_keyUp = 0, m_keyDown = 0, m_keyLeft = 0, m_keyRight = 0;
    qreal   m_speed = 3;
    QPointF m_spawn;

    int m_health = 100;
    int m_maxHealth = 100;
    int m_damageCooldown = 0;
    static constexpr int kDamageCooldownTicks = 30;

    QVector<QPixmap> m_frames[4];
    Facing m_facing = Down;
    int    m_frameIndex = 0;
    int    m_frameCounter = 0;
    static constexpr int kTicksPerFrame = 6;
    static constexpr int kFrameW = 32, kFrameH = 32, kFramesPerRow = 3;

    void loadFrames(const QString& spritePath)
    {
        QPixmap sheet(spritePath);
        for (int dir = 0; dir < 4; ++dir) {
            for (int f = 0; f < kFramesPerRow; ++f) {
                if (!sheet.isNull()) {
                    m_frames[dir].push_back(
                        sheet.copy(f * kFrameW, dir * kFrameH, kFrameW, kFrameH));
                } else {
                    QPixmap px(kFrameW, kFrameH);
                    px.fill(m_id == 1 ? Qt::red : Qt::blue);
                    m_frames[dir].push_back(px);
                }
            }
        }
    }

    bool tryMove(qreal dx, qreal dy)
    {
        if (dx == 0 && dy == 0)
            return false;

        const QPointF old = pos();
        moveBy(dx, dy);

        const QList<QGraphicsItem*> hits = collidingItems();
        for (QGraphicsItem* item : hits) {
            if (dynamic_cast<Wall*>(item)) {
                setPos(old);
                return false;
            }
        }

        if (scene()) {
            const QRectF bounds = scene()->sceneRect();
            const QRectF box = mapRectToScene(shape().boundingRect());
            if (!bounds.contains(box)) {
                setPos(old);
                return false;
            }
        }
        return true;
    }

    void advanceAnimation(bool moving)
    {
        if (!moving) {
            m_frameIndex = 0;
            m_frameCounter = 0;
            setPixmap(m_frames[m_facing][0]);
            return;
        }
        if (++m_frameCounter % kTicksPerFrame == 0)
            m_frameIndex = (m_frameIndex == 1) ? 2 : 1;
        if (m_frameIndex == 0)
            m_frameIndex = 1;
        setPixmap(m_frames[m_facing][m_frameIndex]);
    }

    void updateBlink()
    {
        if (m_damageCooldown > 0)
            setOpacity((m_damageCooldown / 4) % 2 == 0 ? 1.0 : 0.4);
        else
            setOpacity(1.0);
    }
};

class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GameView(QGraphicsScene* scene, QWidget* parent = nullptr)
        : QGraphicsView(scene, parent)
    {
        setFocusPolicy(Qt::StrongFocus);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    const QSet<int>& pressedKeys() const { return m_pressedKeys; }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* e) override
    {
        emit clicked();
        QGraphicsView::mousePressEvent(e);
    }

    void keyPressEvent(QKeyEvent* e) override
    {
        if (e->isAutoRepeat())
            return;
        m_pressedKeys.insert(e->key());
    }

    void keyReleaseEvent(QKeyEvent* e) override
    {
        if (e->isAutoRepeat())
            return;
        m_pressedKeys.remove(e->key());
    }

    void focusOutEvent(QFocusEvent* e) override
    {
        m_pressedKeys.clear();
        QGraphicsView::focusOutEvent(e);
    }

private:
    QSet<int> m_pressedKeys;
};

class HUD : public QObject
{
    Q_OBJECT

public:
    HUD(QGraphicsScene* scene, Player* p1, Player* p2, QObject* parent = nullptr)
        : QObject(parent)
    {
        const QRectF r = scene->sceneRect();

        m_bar1 = makeBar(scene, r.left() + 50,  r.top() + 8, "P1");
        m_bar2 = makeBar(scene, r.left() + 290, r.top() + 8, "P2");

        m_levelText = scene->addSimpleText("Level 1");
        QFont f = m_levelText->font();
        f.setPointSize(14);
        f.setBold(true);
        m_levelText->setFont(f);
        m_levelText->setBrush(Qt::white);
        m_levelText->setZValue(kHudZ);
        m_levelText->setPos(r.right() - 110, r.top() + 5);

        connect(p1, &Player::healthChanged, this, [this, p1](int hp) {
            updateBar(m_bar1, hp, p1->maxHealth());
        });
        connect(p2, &Player::healthChanged, this, [this, p2](int hp) {
            updateBar(m_bar2, hp, p2->maxHealth());
        });

        updateBar(m_bar1, p1->health(), p1->maxHealth());
        updateBar(m_bar2, p2->health(), p2->maxHealth());
    }

    void setLevel(int n) { m_levelText->setText(QString("Level %1").arg(n)); }

private:
    struct Bar {
        QGraphicsRectItem*       background = nullptr;
        QGraphicsRectItem*       fill       = nullptr;
        QGraphicsSimpleTextItem* label      = nullptr;
        QGraphicsSimpleTextItem* value      = nullptr;
        qreal x = 0, y = 0;
    };

    static constexpr qreal kBarWidth  = 150;
    static constexpr qreal kBarHeight = 14;
    static constexpr qreal kHudZ      = 100;

    Bar m_bar1;
    Bar m_bar2;
    QGraphicsSimpleTextItem* m_levelText = nullptr;

    Bar makeBar(QGraphicsScene* scene, qreal x, qreal y, const QString& name)
    {
        Bar b;
        b.x = x;
        b.y = y;

        QFont font;
        font.setBold(true);
        font.setPointSize(12);
        b.label = scene->addSimpleText(name, font);
        b.label->setBrush(Qt::white);
        b.label->setPos(x - 30, y - 2);
        b.label->setZValue(kHudZ);

        b.background = scene->addRect(x, y, kBarWidth, kBarHeight,
                                      QPen(Qt::black, 2), QBrush(QColor(40, 40, 40)));
        b.background->setZValue(kHudZ);

        b.fill = scene->addRect(x, y, kBarWidth, kBarHeight,
                                QPen(Qt::NoPen), QBrush(Qt::green));
        b.fill->setZValue(kHudZ + 1);

        QFont small;
        small.setPointSize(10);
        b.value = scene->addSimpleText("100", small);
        b.value->setBrush(Qt::white);
        b.value->setPos(x + kBarWidth + 6, y);
        b.value->setZValue(kHudZ);
        return b;
    }

    void updateBar(Bar& bar, int hp, int maxHp)
    {
        const qreal ratio = maxHp > 0 ? qreal(hp) / maxHp : 0;
        bar.fill->setRect(bar.x, bar.y, kBarWidth * ratio, kBarHeight);
        if (ratio > 0.6)      bar.fill->setBrush(QColor(60, 200, 80));
        else if (ratio > 0.3) bar.fill->setBrush(QColor(230, 200, 40));
        else                  bar.fill->setBrush(QColor(220, 50, 50));
        bar.value->setText(QString::number(hp));
    }
};

class Level : public QObject
{
    Q_OBJECT

public:
    Level(QGraphicsScene* scene, int number, QObject* parent = nullptr)
        : QObject(parent), m_scene(scene), m_number(number) {}

    void build()
    {
        struct R { qreal x, y, w, h; };
        const R rects[] = {
            {0,   32,  800, 16}, {0, 584, 800, 16},
            {0,   32,  16, 568}, {784, 32, 16, 568},
            {160, 120, 16, 360}, {160, 120, 240, 16},
            {320, 260, 300, 16}, {480, 380, 16, 204},
            {620, 120, 16, 200}, {260, 440, 140, 16},
        };
        for (const R& r : rects)
            m_scene->addItem(new Wall(r.x, r.y, r.w, r.h));
        m_exit = m_scene->addRect(700, 60, 60, 60, QPen(QColor(30, 110, 40), 2), QBrush(QColor(80, 200, 100)));
        m_exit->setZValue(1);
    }

    void setPlayers(Player* p1, Player* p2) { m_p1 = p1; m_p2 = p2; }

    int     number() const       { return m_number; }
    QPointF spawnP1() const      { return m_spawn1; }
    QPointF spawnP2() const      { return m_spawn2; }
    int     restartCount() const { return m_restartCount; }

    void setClueTotal(int n) { m_cluesTotal = n; }
    void clueFound()         { ++m_cluesFound; }
    bool allCluesFound() const { return m_cluesFound >= m_cluesTotal; }

    bool bothAtExit() const
    {
        if (!m_p1 || !m_p2 || !m_exit)
            return false;
        return m_p1->collidesWithItem(m_exit) && m_p2->collidesWithItem(m_exit);
    }

    void checkExit()
    {
        if (allCluesFound() && bothAtExit())
            emit levelComplete();
    }

public slots:
    void restart()
    {
        ++m_restartCount;
        if (m_p1) m_p1->reset();
        if (m_p2) m_p2->reset();
        emit restarted();
    }

signals:
    void restarted();
    void levelComplete();

private:
    QGraphicsScene* m_scene;
    int m_number;
    Player* m_p1 = nullptr;
    Player* m_p2 = nullptr;
    QPointF m_spawn1{60, 520};
    QPointF m_spawn2{110, 520};
    int m_restartCount = 0;
    QGraphicsRectItem* m_exit = nullptr;
    int m_cluesFound = 0;
    int m_cluesTotal = 0;
};

class Game : public QObject
{
    Q_OBJECT

public:
    explicit Game(QObject* parent = nullptr)
        : QObject(parent)
    {
        m_scene = new QGraphicsScene(0, 0, 800, 600, this);
        m_scene->setBackgroundBrush(QColor(194, 170, 120));
        m_view = new GameView(m_scene);
        m_view->setFixedSize(804, 604);
        m_view->setWindowTitle("The Lost Expedition");

        m_level = new Level(m_scene, 1, this);
        m_level->build();

        m_player1 = new Player(1, m_level->spawnP1(), ":/sprites/explorer1.png", this);
        m_player1->setKeys(Qt::Key_W, Qt::Key_S, Qt::Key_A, Qt::Key_D);
        m_scene->addItem(m_player1);

        m_player2 = new Player(2, m_level->spawnP2(), ":/sprites/explorer2.png", this);
        m_player2->setKeys(Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right);
        m_scene->addItem(m_player2);

        m_level->setPlayers(m_player1, m_player2);

        connect(m_player1, &Player::died, m_level, &Level::restart, Qt::QueuedConnection);
        connect(m_player2, &Player::died, m_level, &Level::restart, Qt::QueuedConnection);

        m_hud = new HUD(m_scene, m_player1, m_player2, this);
        m_hud->setLevel(m_level->number());

        connect(&m_timer, &QTimer::timeout, this, &Game::tick);
        m_startText = m_scene->addSimpleText("Click to start");
        m_startText->setBrush(QColor(60, 40, 20));
        m_startText->setFont(QFont("Arial", 32, QFont::Bold));
        m_startText->setZValue(50);
        m_startText->setPos((800 - m_startText->boundingRect().width()) / 2, 270);

        connect(m_view, &GameView::clicked, this, &Game::beginGame);
        connect(m_level, &Level::levelComplete, this, &Game::finishLevel);
    }

    ~Game() override { delete m_view; }

    void start()
    {
        m_view->show();
        m_view->setFocus();
    }

    Player* player1() const { return m_player1; }
    Player* player2() const { return m_player2; }
    Level*  level()   const { return m_level; }

public slots:
    void beginGame()
    {
        if (m_started)
            return;
        m_started = true;
        m_scene->removeItem(m_startText);
        delete m_startText;
        m_startText = nullptr;
        m_timer.start(16);
        m_view->setFocus();
    }
    void tick()
    {
        const QSet<int>& keys = m_view->pressedKeys();
        m_player1->update(keys);
        m_player2->update(keys);
        m_level->checkExit();
    }

    void finishLevel()
    {
        m_timer.stop();
        QString msg = "Level " + QString::number(m_level->number()) + " Complete!";
        m_doneText = m_scene->addSimpleText(msg);
        m_doneText->setBrush(QColor(60, 40, 20));
        m_doneText->setFont(QFont("Arial", 32, QFont::Bold));
        m_doneText->setZValue(50);
        m_doneText->setPos((800 - m_doneText->boundingRect().width()) / 2, 270);
    }

private:
    QGraphicsScene* m_scene   = nullptr;
    GameView*       m_view    = nullptr;
    Player*         m_player1 = nullptr;
    Player*         m_player2 = nullptr;
    Level*          m_level   = nullptr;
    HUD*            m_hud     = nullptr;
    QTimer          m_timer;
    QGraphicsSimpleTextItem* m_startText = nullptr;
    bool            m_started = false;
    QGraphicsSimpleTextItem* m_doneText = nullptr;
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Game game;
    game.start();

    return app.exec();
}

#include "main.moc"
