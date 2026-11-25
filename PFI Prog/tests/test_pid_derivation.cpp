
// DEBUT DU CODE CREE AVEC L'AIDE D'UNE IA
#include "panneau/Pid.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main()
{
    Pid pid(0.0, 0.0, 2.0);
    assert(pid.definirLimites(0.0, 100.0));
    assert(pid.calculer(40, 10, 1.0) == 0.0);
    // Un changement de consigne n'engendre pas de pic derive.
    assert(pid.calculer(60, 10, 1.0) == 0.0);
    // En cas de baisse du niveau, la commande derivee doit ouvrir.
    assert(pid.calculer(60, 5, 1.0) == 10.0);
    assert(!pid.definirGains(-1, 0, 0));
    assert(pid.definirGains(1.0, 0.05, 0.0));
    std::cout << "Test derivee mesure reussi\n";
}
// FIN DU CODE CREE AVEC L'AIDE D'UNE IA
