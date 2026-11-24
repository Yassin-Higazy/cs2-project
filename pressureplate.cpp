#include "pressureplate.h"
#include <QFont>

PressurePlate::PressurePlate(qreal x, qreal y, qreal w, qreal h, QString sym)
    : QGraphicsRectItem(0, 0, w, h)
{
    setPos(x, y);
    setZValue(1);   // under the players
    symbol = sym;
    pressed = false;
    wasPressed = false;
    normalColor = QColor(130, 120, 105);
    pressedColor = QColor(240, 200, 70);
    label = nullptr;

    // show the symbol in the middle of the plate
    if (symbol != "") {
        label = new QGraphicsSimpleTextItem(symbol, this);
        QFont f;
        f.setPointSize(18);
        f.setBold(true);
        label->setFont(f);
        label->setBrush(QColor(60, 45, 30));
        label->setPos((w - label->boundingRect().width()) / 2, (h - label->boundingRect().height()) / 2);
    }
    updateColor();
}

void PressurePlate::checkPlayers(QGraphicsItem* p1, QGraphicsItem* p2)
{
    wasPressed = pressed;
    pressed = (p1 && p1->collidesWithItem(this)) || (p2 && p2->collidesWithItem(this));
    if (pressed != wasPressed)
        updateColor();
}

bool PressurePlate::isPressed() const
{
    return pressed;
}

bool PressurePlate::justPressed() const
{
    return pressed && !wasPressed;
}

QString PressurePlate::getSymbol() const
{
    return symbol;
}

void PressurePlate::setColors(QColor normal, QColor press)
{
    normalColor = normal;
    pressedColor = press;
    updateColor();
}

void PressurePlate::reset()
{
    pressed = false;
    wasPressed = false;
    updateColor();
}

void PressurePlate::updateColor()
{
    if (pressed) {
        setBrush(pressedColor);
        setPen(QPen(pressedColor.lighter(130), 3));
    } else {
        setBrush(normalColor);
        setPen(QPen(normalColor.darker(150), 3));
    }
}
