#ifndef GLYPHFLOORPUZZLE_H
#define GLYPHFLOORPUZZLE_H

#include <QList>
#include "puzzle.h"
#include "pressureplate.h"
#include "cluepickup.h"

// Puzzle 2: The Glyph Floor
// a picture on the floor shows 4 symbols with some marks under each one.
// players step on the symbol tiles in order, fewest marks first.
// wrong tile = that player loses health and the order starts again.
// when it's done a scroll with the clue shows up.
class GlyphFloorPuzzle : public Puzzle
{
    Q_OBJECT

public:
    GlyphFloorPuzzle(QGraphicsScene* s, int level, QObject* parent = nullptr);

    void tick(Player* p1, Player* p2) override;
    void reset() override;

private:
    // a wrong tile was stepped on by "who"
    void wrongStep(Player* who);

    // gives every tile the same colour
    void setTilesColor(QColor c);

    // draws the picture with the symbols and marks
    void makeHintPanel();

    QList<PressurePlate*> tiles;
    QList<QString> correctOrder;
    int currentStep;
    CluePickup* scroll;
    int flashTicks;
    QGraphicsRectItem* hintPanel;
};

#endif
