#include "spiketrap.h"
#include "player.h"
#include <QBrush>
#include <QPen>

SpikeTrap::SpikeTrap(qreal x, qreal y, qreal w, qreal h, int dmg, int off)
    : Hazard(x, y, w, h, dmg)
{
    offset = off;
    downTicks = 65;      // a tick is 16ms, so 60 ticks is about 1 second
    warningTicks = 25;
    upTicks = 60;
    reset();
}

void SpikeTrap::tick(Player* p1, Player* p2)
{
    tickCounter++;
    int t = (tickCounter + offset) % (downTicks + warningTicks + upTicks);
    warning = (t >= downTicks && t < downTicks + warningTicks);
    raised = (t >= downTicks + warningTicks);
    updateLook();

    if (raised)
        Hazard::tick(p1, p2);
}

void SpikeTrap::reset()
{
    tickCounter = 0;
    raised = false;
    warning = false;
    updateLook();
}

void SpikeTrap::updateLook()
{
    if (raised) {
        setBrush(QBrush(QColor(215, 215, 230), Qt::DiagCrossPattern));   // silver spikes
        setPen(QPen(QColor(60, 60, 70), 2));
    } else if (warning) {
        setBrush(QBrush(QColor(200, 70, 60), Qt::Dense5Pattern));        // red tips
        setPen(QPen(QColor(120, 30, 30), 2));
    } else {
        setBrush(QBrush(QColor(55, 45, 35), Qt::Dense6Pattern));         // holes in the floor
        setPen(QPen(QColor(70, 60, 50), 2));
    }
}
