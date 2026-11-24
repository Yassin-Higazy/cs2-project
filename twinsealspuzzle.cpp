#include "twinsealspuzzle.h"
#include "player.h"

const int GATE_OPEN_TIME = 300;   // 300 ticks = about 5 seconds
const int CRUSH_DAMAGE = 25;

TwinSealsPuzzle::TwinSealsPuzzle(QGraphicsScene* s, int level, QObject* parent)
    : Puzzle(s, level, parent)
{
    // plate A in the top left corner, plate B next to the vault in the bottom right
    plateA = new PressurePlate(36, 64, 44, 44);
    plateB = new PressurePlate(720, 336, 44, 44);
    scene->addItem(plateA);
    scene->addItem(plateB);

    // the vault: a wall plus the gate on top of the bottom right room
    scene->addItem(new Wall(496, 400, 208, 16));
    scene->addRect(704, 400, 80, 16, Qt::NoPen, QColor(90, 70, 50))->setZValue(0.5);   // floor under the gate
    gate = new VaultGate(704, 400, 80, 16);
    scene->addItem(gate);

    chest = new CluePickup(620, 500, 30, 24,
                           Clue("Vault Tablet", "The sun rests on the third stone.", level));
    scene->addItem(chest);

    reset();
}

void TwinSealsPuzzle::tick(Player* p1, Player* p2)
{
    plateA->checkPlayers(p1, p2);
    plateB->checkPlayers(p1, p2);

    // chest already taken, the puzzle is done
    if (chestTaken)
        return;

    if (plateA->isPressed() && plateB->isPressed()) {
        // both seals pressed: the gate stays open
        gate->open();
        held = true;
    } else {
        // they just let go: start the countdown once
        if (held) {
            gate->startCountdown(GATE_OPEN_TIME);
            held = false;
        }
        // softlock fix: if the gate slams on someone, push them out of the doorway
        if (gate->updateCountdown())
            damageDoorway(p1, p2);
    }

    // taking the chest solves the puzzle
    if (chest->checkPlayers(p1, p2)) {
        chestTaken = true;
        solved = true;
        gate->lockOpen();
        emit clueEarned(chest->getClue());
    }
}

void TwinSealsPuzzle::damageDoorway(Player* p1, Player* p2)
{
    QRectF doorway = gate->sceneBoundingRect();
    Player* players[2] = {p1, p2};

    for (int i = 0; i < 2; i++) {
        if (!players[i])
            continue;
        QRectF box = players[i]->shape().boundingRect();
        QRectF body = players[i]->mapRectToScene(box);
        if (body.intersects(doorway)) {
            // push them to the side of the gate they are closer to
            if (body.center().y() < doorway.center().y())
                players[i]->setY(doorway.top() - box.bottom() - 1);
            else
                players[i]->setY(doorway.bottom() - box.top() + 1);
            players[i]->takeDamage(CRUSH_DAMAGE);
        }
    }
}

void TwinSealsPuzzle::reset()
{
    solved = false;
    chestTaken = false;
    held = false;
    plateA->reset();
    plateB->reset();
    gate->reset();
    chest->reset();
    chest->setActive(true);   // the chest is always there inside the vault
}
