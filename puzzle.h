#ifndef PUZZLE_H
#define PUZZLE_H

#include <QObject>
#include <QGraphicsScene>
#include "clue.h"

class Player;

// base class for all puzzles
class Puzzle : public QObject
{
    Q_OBJECT

public:
    Puzzle(QGraphicsScene* s, int level, QObject* parent = nullptr);
    virtual ~Puzzle() = default;

    // called every tick, every puzzle checks what the players are doing
    virtual void tick(Player* p1, Player* p2) = 0;

    // back to how it was at the start of the level
    virtual void reset() = 0;

    bool isSolved() const;

signals:
    // sent when the players pick up the clue of this puzzle
    void clueEarned(const Clue& clue);

protected:
    bool solved;
    int levelNumber;
    QGraphicsScene* scene;
};

#endif
