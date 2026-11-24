#ifndef CLUETRACKER_H
#define CLUETRACKER_H

#include <QObject>
#include <QList>
#include "clue.h"

// keeps all clues collected so far
// it belongs to Game so the clues stay when we go to the next level
class ClueTracker : public QObject
{
    Q_OBJECT

public:
    ClueTracker(QObject* parent = nullptr);

    void addClue(Clue c);
    void removeLevelClues(int level);   // used when the level restarts
    bool hasClue(QString title);
    int count();
    QList<Clue> getClues();

signals:
    void clueAdded(Clue c);
    void changed();

private:
    QList<Clue> clues;
};

#endif
