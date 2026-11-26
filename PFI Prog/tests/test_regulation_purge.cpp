
#include "panneau/RegulationBassins.h"

#include <cassert>
#include <cmath>
#include <iostream>

int main()
{
    RegulationBassins regulation;

    // Remplissage initial
    auto sortie = regulation.calculer(
        50, 75, 0, 0, 1.0, true, 0, 0
    );

    assert(sortie.pompe);
    assert(sortie.valveGB > 0);
    assert(sortie.valvePB > 0);

    // Le PID reste actif pres de la consigne, sans arret de la pompe
    sortie = regulation.calculer(
        50, 75, 49.7, 74.7, 1.0, true, 0, 0
    );

    assert(sortie.pompe);
    assert(sortie.valveGB >= 0 && sortie.valveGB <= 100);
    assert(sortie.valvePB >= 0 && sortie.valvePB <= 100);

    // Regulation independante des mesures de purge
    sortie = regulation.calculer(
        50, 75, 49.0, 74.0, 1.0, true, 40, 30
    );

    assert(sortie.pompe);
    assert(sortie.valveGB >= 0 && sortie.valveGB <= 100);
    assert(sortie.valvePB >= 0 && sortie.valvePB <= 100);

    // Nouvelles consignes independantes
    sortie = regulation.calculer(
        60, 80, 50, 75, 1.0, true, 0, 0
    );

    assert(sortie.pompe);
    assert(sortie.valveGB >= 0 && sortie.valveGB <= 100);
    assert(sortie.valvePB >= 0 && sortie.valvePB <= 100);

    // GB a 0, PB conserve son PID
    regulation.reinitialiser();

    sortie = regulation.calculer(
        0, 75, 0, 50, 1.0, true, 0, 0
    );

    assert(sortie.pompe);
    assert(sortie.valveGB == 0);
    assert(sortie.valvePB > 0);

    // PB a 0, GB conserve son PID
    regulation.reinitialiser();

    sortie = regulation.calculer(
        50, 0, 20, 0, 1.0, true, 0, 0
    );

    assert(sortie.pompe);
    assert(sortie.valveGB > 0);
    assert(sortie.valvePB == 0);

    // Deux consignes nulles : pompe toujours autorisee
    regulation.reinitialiser();

    sortie = regulation.calculer(
        0, 0, 0, 0, 1.0, true, 0, 0
    );

    assert(sortie.pompe);
    assert(sortie.valveGB == 0);
    assert(sortie.valvePB == 0);

    // Protection de niveau maximum
    sortie = regulation.calculer(
        50, 75, 100, 30, 1.0, true, 0, 0
    );

    assert(!sortie.pompe);
    assert(sortie.valveGB == 0);
    assert(sortie.valvePB == 0);

    // Modification des gains PID
    assert(regulation.reglerGainsGB(3.0, 0.02, 0.0));
    assert(regulation.reglerGainsPB(1.5, 0.02, 0.0));
    assert(!regulation.reglerGainsGB(-2.0, 0.01, 0.0));

    std::cout << "Tests niveaux/purges reussis\n";

    return 0;
}
