#ifndef HAZARD_H
#define HAZARD_H

#include <QGraphicsRectItem>

class Player;

// base class for all traps
// moving hazards in later levels will inherit from this too
class Hazard : public QGraphicsRectItem
{
public:
    Hazard(qreal x, qreal y, qreal w, qreal h, int damage);
    virtual ~Hazard() = default;

    // called every tick, by default it damages whoever touches it
    virtual void tick(Player* p1, Player* p2);

    // back to the start of the level
    virtual void reset();

protected:
    int damage;
};

#endif
