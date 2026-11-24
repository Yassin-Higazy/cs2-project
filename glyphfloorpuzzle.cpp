#include "glyphfloorpuzzle.h"
#include "player.h"
#include <QGraphicsSimpleTextItem>
#include <QFont>

const int WRONG_DAMAGE = 15;
const QColor STONE(150, 135, 110);
const QColor STONE_PRESSED(175, 160, 130);
const QColor GOLD(245, 205, 80);
const QColor RED(200, 60, 50);

GlyphFloorPuzzle::GlyphFloorPuzzle(QGraphicsScene* s, int level, QObject* parent)
    : Puzzle(s, level, parent)
{
    correctOrder << "★" << "●" << "▲" << "◆";

    // 6 tiles in 2 rows, ■ and ✚ are not on the picture (fake ones)
    QList<QString> symbols;
    symbols << "●" << "■" << "▲" << "✚" << "★" << "◆";
    for (int i = 0; i < 6; i++) {
        qreal x = 196 + (i % 3) * 56;
        qreal y = 300 + (i / 3) * 56;
        PressurePlate* tile = new PressurePlate(x, y, 44, 44, symbols[i]);
        tile->setColors(STONE, STONE_PRESSED);
        scene->addItem(tile);
        tiles.append(tile);
    }

    makeHintPanel();

    scroll = new CluePickup(368, 330, 26, 22,
                            Clue("Glyph Scroll", "The old order is  ★ ● ▲ ◆", level));
    scroll->setBrush(QColor(240, 225, 180));   // parchment colour
    scene->addItem(scroll);

    reset();   // scroll hidden until the puzzle is solved
}

// the picture shows the symbols in the wrong order with marks under each one
void GlyphFloorPuzzle::makeHintPanel()
{
    hintPanel = scene->addRect(0, 0, 200, 92, QPen(QColor(80, 60, 40), 3), QColor(175, 150, 110));
    hintPanel->setPos(190, 148);
    hintPanel->setZValue(1);

    QFont small;
    small.setPointSize(8);
    small.setBold(true);
    QGraphicsSimpleTextItem* hint = new QGraphicsSimpleTextItem("Fewest marks walk first", hintPanel);
    hint->setFont(small);
    hint->setBrush(QColor(70, 45, 25));
    hint->setPos(35, 4);

    QString syms[4] = {"▲", "◆", "★", "●"};
    int marks[4] = {3, 4, 1, 2};

    QFont big;
    big.setPointSize(16);
    big.setBold(true);
    for (int i = 0; i < 4; i++) {
        int cx = 25 + i * 50;
        QGraphicsSimpleTextItem* g = new QGraphicsSimpleTextItem(syms[i], hintPanel);
        g->setFont(big);
        g->setBrush(QColor(60, 40, 20));
        g->setPos(cx - g->boundingRect().width() / 2, 24);

        // little lines under the symbol
        for (int m = 0; m < marks[i]; m++) {
            int x = cx - (marks[i] - 1) * 4 + m * 8;
            QGraphicsRectItem* line = new QGraphicsRectItem(x - 1, 62, 3, 14, hintPanel);
            line->setBrush(QColor(60, 40, 20));
            line->setPen(Qt::NoPen);
        }
    }
}

void GlyphFloorPuzzle::tick(Player* p1, Player* p2)
{
    // after a mistake the tiles stay red for a bit
    if (flashTicks > 0) {
        flashTicks--;
        if (flashTicks == 0)
            setTilesColor(STONE);
    }

    // once solved, only the scroll is left to pick up
    if (solved) {
        if (scroll->checkPlayers(p1, p2))
            emit clueEarned(scroll->getClue());
        return;
    }

    for (int i = 0; i < tiles.size(); i++) {
        tiles[i]->checkPlayers(p1, p2);

        // only count it on the tick someone steps onto it, and not while flashing red
        if (!tiles[i]->justPressed() || flashTicks > 0)
            continue;

        // walking back over a tile that's already right is fine
        int place = correctOrder.indexOf(tiles[i]->getSymbol());
        if (place != -1 && place < currentStep)
            continue;

        if (tiles[i]->getSymbol() == correctOrder[currentStep]) {
            tiles[i]->setColors(GOLD, GOLD);
            currentStep++;
            if (currentStep == correctOrder.size()) {
                solved = true;
                scroll->setActive(true);
            }
        } else {
            // find who stepped on it
            if (p1 && p1->collidesWithItem(tiles[i]))
                wrongStep(p1);
            else if (p2)
                wrongStep(p2);
        }
    }
}

void GlyphFloorPuzzle::wrongStep(Player* who)
{
    who->takeDamage(WRONG_DAMAGE);
    currentStep = 0;
    setTilesColor(RED);
    flashTicks = 30;
}

void GlyphFloorPuzzle::setTilesColor(QColor c)
{
    for (int i = 0; i < tiles.size(); i++) {
        if (c == STONE)
            tiles[i]->setColors(STONE, STONE_PRESSED);
        else
            tiles[i]->setColors(c, c);
    }
}

void GlyphFloorPuzzle::reset()
{
    solved = false;
    currentStep = 0;
    flashTicks = 0;
    for (int i = 0; i < tiles.size(); i++)
        tiles[i]->reset();
    setTilesColor(STONE);
    scroll->reset();   // reset() also hides it
}
