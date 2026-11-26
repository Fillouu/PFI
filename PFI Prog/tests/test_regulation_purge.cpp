
#include "panneau/RegulationBassins.h"
#include "panneau/GestionAlarmes.h"

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

    // DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
    RegulationBassins sansPurge, avecPurge;
    const auto a = sansPurge.calculer(10, 10, 9.8, 10.1, 1.0, true);
    const auto b = avecPurge.calculer(10, 10, 9.8, 10.1, 1.0, true, 0, 100);
    assert(a.pompe == b.pompe && a.valveGB == b.valveGB && a.valvePB == b.valvePB);

    GestionAlarme alarme;
    const auto t = std::chrono::steady_clock::time_point{};
    using S = std::chrono::seconds;
    assert(!alarme.actualiser(true, false, 3, 10, t));
    assert(!alarme.actualiser(true, false, 3, 10, t + S(2)));
    assert(alarme.actualiser(true, false, 3, 10, t + S(3)));
    assert(alarme.estEmise());
    assert(!alarme.actualiser(true, true, 3, 10, t + S(4)));
    assert(!alarme.estEmise());
    assert(!alarme.actualiser(true, false, 3, 10, t + S(13)));
    assert(alarme.actualiser(true, false, 3, 10, t + S(14)));
    assert(!alarme.actualiser(false, true, 3, 10, t + S(15)));
    assert(!alarme.actualiser(false, false, 3, 10, t + S(30)));
    // FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA

    std::cout << "Tests niveaux/purges reussis\n";

    return 0;
}