#include "puzzle.h"

// stores the scene and the level, nothing is solved at the start
Puzzle::Puzzle(QGraphicsScene* s, int level, QObject* parent)
    : QObject(parent), solved(false), levelNumber(level), scene(s)
{
}

bool Puzzle::isSolved() const
{
    return solved;
}
