#ifndef WALL_H
#define WALL_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

// solid block, players can't walk through it (checked in Player::tryMove)
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

#endif
