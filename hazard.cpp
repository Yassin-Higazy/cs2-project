#include "hazard.h"
#include "player.h"
#include <QBrush>
#include <QPen>

Hazard::Hazard(qreal x, qreal y, qreal w, qreal h, int dmg)
    : QGraphicsRectItem(0, 0, w, h)
{
    setPos(x, y);
    setZValue(3);   // above the floor, under the players (10)
    damage = dmg;
    setBrush(QColor(170, 40, 40));
    setPen(QPen(QColor(90, 20, 20), 2));
}

void Hazard::tick(Player* p1, Player* p2)
{
    // takeDamage has a cooldown so this doesn't hurt every single tick
    if (p1 && p1->collidesWithItem(this))
        p1->takeDamage(damage);
    if (p2 && p2->collidesWithItem(this))
        p2->takeDamage(damage);
}

void Hazard::reset()
{
}
