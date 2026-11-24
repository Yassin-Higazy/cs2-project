#ifndef VAULTGATE_H
#define VAULTGATE_H

#include "wall.h"

// gate of the vault, it is a Wall so players can't pass while it's closed
// to open it we hide it (hidden items don't collide with anything in Qt)
class VaultGate : public Wall
{
public:
    VaultGate(qreal x, qreal y, qreal w, qreal h);

    void open();
    void close();
    bool isOpen() const;

    // start counting down before it closes (60 ticks = about 1 second)
    void startCountdown(int ticks);

    // call every tick, returns true on the tick the gate slams shut
    bool updateCountdown();

    // stays open for good (after the chest is taken)
    void lockOpen();

    // closed again with no countdown
    void reset();

private:
    bool isOpenFlag;
    bool lockedOpen;
    int countdownTicks;
    int countdownMax;
    QGraphicsRectItem* countdownBar;
};

#endif
