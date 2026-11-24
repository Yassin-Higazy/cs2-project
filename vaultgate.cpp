#include "vaultgate.h"
#include <QGraphicsScene>

VaultGate::VaultGate(qreal x, qreal y, qreal w, qreal h) : Wall(x, y, w, h)
{
    setBrush(QColor(110, 75, 40));
    setPen(QPen(QColor(230, 190, 70), 2));
    isOpenFlag = false;
    lockedOpen = false;
    countdownTicks = 0;
    countdownMax = 300;   // 5 seconds

    // bar above the gate that shows the time left
    // it's a separate item (not a child) so it still shows when the gate is hidden
    countdownBar = new QGraphicsRectItem(0, 0, w, 5);
    countdownBar->setPos(x, y - 9);
    countdownBar->setBrush(QColor(240, 200, 70));
    countdownBar->setPen(Qt::NoPen);
    countdownBar->setZValue(7);
    countdownBar->hide();
}

void VaultGate::open()
{
    isOpenFlag = true;
    hide();
    countdownBar->hide();   // no countdown while it's being held open
    // the bar can only be added once the gate itself is in the scene
    if (scene() && !countdownBar->scene())
        scene()->addItem(countdownBar);
}

void VaultGate::close()
{
    if (lockedOpen)
        return;
    isOpenFlag = false;
    countdownTicks = 0;
    countdownBar->hide();
    show();
}

bool VaultGate::isOpen() const
{
    return isOpenFlag;
}

void VaultGate::startCountdown(int ticks)
{
    countdownTicks = ticks;
    countdownMax = ticks;
    countdownBar->setRect(0, 0, rect().width(), 5);   // full bar
    countdownBar->show();
}

bool VaultGate::updateCountdown()
{
    if (!isOpenFlag || lockedOpen || countdownTicks <= 0) {
        countdownBar->hide();
        return false;
    }

    countdownTicks--;
    double left = (double)countdownTicks / countdownMax;
    countdownBar->setRect(0, 0, rect().width() * left, 5);
    countdownBar->setBrush(left > 0.35 ? QColor(240, 200, 70) : QColor(220, 60, 50));
    countdownBar->show();

    if (countdownTicks == 0) {
        close();
        return true;
    }
    return false;
}

void VaultGate::lockOpen()
{
    open();
    lockedOpen = true;
    countdownBar->hide();
}

void VaultGate::reset()
{
    lockedOpen = false;
    close();
}
