#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPixmap>
#include <QSet>
#include <QVector>
#include <QList>
#include <algorithm>
#include "wall.h"

// one of the two explorers (moved here from main.cpp without changes)
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

#endif
