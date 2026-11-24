#include "cluepickup.h"
#include <QBrush>
#include <QPen>

// chest or scroll, starts hidden until the puzzle calls setActive(true)
CluePickup::CluePickup(qreal x, qreal y, qreal w, qreal h, Clue c)
    : QGraphicsRectItem(0, 0, w, h)
{
    setPos(x, y);
    clue = c;
    collected = false;
    active = false;
    setBrush(QColor(240, 200, 60));   // gold
    setPen(QPen(QColor(110, 70, 20), 2));
    setZValue(2);
    setVisible(false);
}

// only visible if it's active and nobody took it yet
void CluePickup::setActive(bool a)
{
    active = a;
    setVisible(active && !collected);
}

// returns true once, on the tick a player touches it
bool CluePickup::checkPlayers(QGraphicsItem* p1, QGraphicsItem* p2)
{
    if (!active || collected)
        return false;

    if ((p1 && p1->collidesWithItem(this)) || (p2 && p2->collidesWithItem(this))) {
        collected = true;
        hide();
        return true;
    }
    return false;
}

Clue CluePickup::getClue() const
{
    return clue;
}

bool CluePickup::isCollected() const
{
    return collected;
}

// not collected and hidden again
void CluePickup::reset()
{
    collected = false;
    setActive(false);
}
