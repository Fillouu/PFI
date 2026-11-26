
#include "panneau/RegulationBassins.h"

#include <cassert>
#include <iostream>

int main()
{
    RegulationBassins regulation;

    auto cmd = regulation.calculer(
        10, 10, 0, 0, 1.0, false
    );

    assert(!cmd.pompe &&
           cmd.valveGB == 0 &&
           cmd.valvePB == 0);

    cmd = regulation.calculer(
        10, 10, 0, 0, 1.0, true
    );

    assert(cmd.pompe &&
           cmd.valveGB > 0 &&
           cmd.valvePB > 0);

    assert(cmd.valveGB <= 100 &&
           cmd.valvePB <= 100);

    cmd = regulation.calculer(
        10, 10, 50, 0, 1.0, true
    );

    assert(cmd.pompe &&
           cmd.valveGB == 0 &&
           cmd.valvePB > 0);

    cmd = regulation.calculer(
        10, 10, 50, 50, 1.0, true
    );

    // DEBUT DU CODE MODIFIE AVEC L'AIDE D'UNE IA
    assert(cmd.pompe && cmd.valveGB == 0 && cmd.valvePB == 0);

    cmd = regulation.calculer(
        10, 10, 0, 0, 1.0, true
    );

    assert(cmd.pompe);
    // FIN DU CODE MODIFIE AVEC L'AIDE D'UNE IA

    cmd = regulation.calculer(
        10, 10, 0, 0, 1.0, false
    );

    assert(!cmd.pompe &&
           cmd.valveGB == 0 &&
           cmd.valvePB == 0);

    cmd = regulation.calculer(
        10, 10, -8, 0, 1.0, true
    );

    assert(!cmd.pompe);

    std::cout << "Tests regulation deux bassins reussis\n";
}