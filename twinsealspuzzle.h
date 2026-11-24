#ifndef TWINSEALSPUZZLE_H
#define TWINSEALSPUZZLE_H

#include "puzzle.h"
#include "pressureplate.h"
#include "vaultgate.h"
#include "cluepickup.h"

// Puzzle 1: The Twin Seals (co-op)
// two plates far apart have to be pressed at the same time to open the vault gate.
// after they step off the gate closes again in 5 seconds.
// taking the chest inside keeps the gate open.
// if the gate closes on someone they lose health.
class TwinSealsPuzzle : public Puzzle
{
    Q_OBJECT

public:
    TwinSealsPuzzle(QGraphicsScene* s, int level, QObject* parent = nullptr);

    void tick(Player* p1, Player* p2) override;
    void reset() override;

private:
    // hurts anyone standing in the doorway when the gate shuts and pushes them out
    // (softlock fix: otherwise they'd be inside a wall and couldn't move)
    void damageDoorway(Player* p1, Player* p2);

    PressurePlate* plateA;
    PressurePlate* plateB;
    VaultGate* gate;
    CluePickup* chest;
    bool chestTaken;
    bool held;   // both plates were pressed last tick
};

#endif
