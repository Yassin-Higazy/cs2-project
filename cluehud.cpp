#include "cluehud.h"
#include <QBrush>
#include <QPen>
#include <QFont>

ClueHUD::ClueHUD(QGraphicsScene* scene, ClueTracker* t, QObject* parent) : QObject(parent)
{
    tracker = t;

    QFont bold;
    bold.setBold(true);
    bold.setPointSize(12);

    // counter in the top bar next to the health bars
    counterText = scene->addSimpleText("", bold);
    counterText->setBrush(QColor(255, 215, 90));
    counterText->setPos(505, 6);
    counterText->setZValue(100);

    // message box at the bottom (hidden at first)
    msgBox = scene->addRect(0, 0, 520, 34, QPen(QColor(255, 215, 90), 2), QBrush(QColor(20, 15, 10, 220)));
    msgBox->setPos(140, 520);
    msgBox->setZValue(110);
    msgBox->hide();
    msgText = new QGraphicsSimpleTextItem(msgBox);
    msgText->setBrush(Qt::white);
    msgText->setPos(15, 8);

    // clue list in the middle of the screen (hidden at first)
    listBox = scene->addRect(0, 0, 560, 320, QPen(QColor(255, 215, 90), 3), QBrush(QColor(30, 22, 12, 235)));
    listBox->setPos(120, 140);
    listBox->setZValue(120);
    listBox->hide();
    listText = new QGraphicsSimpleTextItem(listBox);
    listText->setBrush(Qt::white);
    listText->setPos(20, 16);

    connect(tracker, &ClueTracker::changed, this, &ClueHUD::refresh);
    connect(tracker, &ClueTracker::clueAdded, this, &ClueHUD::showMessage);
    refresh();
}

void ClueHUD::step(const QSet<int>& keys)
{
    // open/close the list when C is pressed (only once per press)
    bool cPressed = keys.contains(Qt::Key_C);
    if (cPressed && !cWasPressed)
        listBox->setVisible(!listBox->isVisible());
    cWasPressed = cPressed;

    if (msgTimer > 0) {
        msgTimer--;
        if (msgTimer == 0)
            msgBox->hide();
    }
}

void ClueHUD::refresh()
{
    counterText->setText("Clues: " + QString::number(tracker->count()) + "   [C]");

    QString s = "CLUES FOUND   (press C to close)\n\n";
    QList<Clue> clues = tracker->getClues();
    if (clues.isEmpty())
        s += "No clues yet.";
    for (int i = 0; i < clues.size(); i++)
        s += "Level " + QString::number(clues[i].level) + " - " + clues[i].title + "\n    " + clues[i].text + "\n\n";
    listText->setText(s);
}

void ClueHUD::showMessage(Clue c)
{
    msgText->setText("Clue found: " + c.title + "  (press C to read it)");
    msgBox->show();
    msgTimer = 180;   // about 3 seconds
}
