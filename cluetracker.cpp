#include "cluetracker.h"

ClueTracker::ClueTracker(QObject* parent) : QObject(parent)
{
}

void ClueTracker::addClue(Clue c)
{
    if (hasClue(c.title))
        return;
    clues.append(c);
    emit clueAdded(c);
    emit changed();
}

void ClueTracker::removeLevelClues(int level)
{
    for (int i = clues.size() - 1; i >= 0; i--) {
        if (clues[i].level == level)
            clues.removeAt(i);
    }
    emit changed();
}

bool ClueTracker::hasClue(QString title)
{
    for (int i = 0; i < clues.size(); i++) {
        if (clues[i].title == title)
            return true;
    }
    return false;
}

int ClueTracker::count()
{
    return clues.size();
}

QList<Clue> ClueTracker::getClues()
{
    return clues;
}
