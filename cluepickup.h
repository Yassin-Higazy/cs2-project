#ifndef CLUEPICKUP_H
#define CLUEPICKUP_H

#include <QGraphicsRectItem>
#include "clue.h"

// a chest or scroll with a clue inside, the player walks on it to take it
class CluePickup : public QGraphicsRectItem
{
public:
    CluePickup(qreal x, qreal y, qreal w, qreal h, Clue c);

    // show it (true) or hide it (false)
    void setActive(bool a);

    // returns true once, on the tick a player touches it
    bool checkPlayers(QGraphicsItem* p1, QGraphicsItem* p2);

    Clue getClue() const;
    bool isCollected() const;

    // not collected and hidden again
    void reset();

private:
    Clue clue;
    bool collected;
    bool active;
};

#endif
