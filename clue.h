#ifndef CLUE_H
#define CLUE_H

#include <QString>

// a clue the players get from solving a puzzle
class Clue
{
public:
    Clue() {}
    Clue(QString t, QString txt, int lvl)
    {
        title = t;
        text = txt;
        level = lvl;
    }

    QString title;
    QString text;
    int level = 0;   // level where it was found
};

#endif
