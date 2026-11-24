#ifndef CLUEHUD_H
#define CLUEHUD_H

#include <QObject>
#include <QSet>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include "cluetracker.h"

// shows the clue counter at the top, a message when a clue is found,
// and a list of all clues when C is pressed
class ClueHUD : public QObject
{
    Q_OBJECT

public:
    ClueHUD(QGraphicsScene* scene, ClueTracker* tracker, QObject* parent = nullptr);
    void step(const QSet<int>& keys);   // called every tick

private slots:
    void refresh();
    void showMessage(Clue c);

private:
    ClueTracker* tracker;

    QGraphicsSimpleTextItem* counterText;

    QGraphicsRectItem* msgBox;
    QGraphicsSimpleTextItem* msgText;
    int msgTimer = 0;

    QGraphicsRectItem* listBox;
    QGraphicsSimpleTextItem* listText;
    bool cWasPressed = false;
};

#endif
