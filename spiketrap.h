#ifndef SPIKETRAP_H
#define SPIKETRAP_H

#include "hazard.h"

// spikes that go down, warn (red tips), then come up for a while
// they only hurt while they are raised
class SpikeTrap : public Hazard
{
public:
    // offset = where in the cycle it starts, so two traps aren't in sync
    SpikeTrap(qreal x, qreal y, qreal w, qreal h, int damage, int offset);

    // move the cycle forward and hurt players if raised
    void tick(Player* p1, Player* p2) override;

    // back to the starting point of the cycle
    void reset() override;

private:
    // colour for down / warning / raised
    void updateLook();

    bool raised;
    bool warning;
    int tickCounter;
    int offset;
    int downTicks;
    int warningTicks;
    int upTicks;
};

#endif
