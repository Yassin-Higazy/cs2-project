#ifndef PRESSUREPLATE_H
#define PRESSUREPLATE_H

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen>
#include <QString>

// a plate on the floor that players step on
// used for the two seals and for the glyph tiles (a glyph tile has a symbol)
class PressurePlate : public QGraphicsRectItem
{
public:
    // symbol is empty for a normal plate
    PressurePlate(qreal x, qreal y, qreal w, qreal h, QString symbol = "");

    // called every tick, checks if a player is on it and changes the colour
    void checkPlayers(QGraphicsItem* p1, QGraphicsItem* p2);

    // true while someone is standing on it
    bool isPressed() const;

    // true only on the tick someone steps onto it
    bool justPressed() const;

    QString getSymbol() const;

    // change the colours (glyph puzzle uses this for gold / red)
    void setColors(QColor normal, QColor pressed);

    // back to not pressed
    void reset();

private:
    void updateColor();

    bool pressed;
    bool wasPressed;
    QString symbol;
    QColor normalColor;
    QColor pressedColor;
    QGraphicsSimpleTextItem* label;
};

#endif
