
#include "panneau/Pid.h"
#include <cassert>
#include <cmath>
#include <iostream>


int main()
{
    Pid proportionnel(2.0, 0.0, 0.0);

    assert(std::abs(
        proportionnel.calculer(60.0, 40.0, 1.0) - 40.0
    ) < 1e-9);

    assert(proportionnel.calculer(100.0, 0.0, 1.0) == 100.0);
    assert(proportionnel.calculer(0.0, 100.0, 1.0) == 0.0);
    assert(proportionnel.calculer(50.0, 40.0, 0.0) == 0.0);

    assert(proportionnel.definirLimites(10.0, 80.0));
    assert(proportionnel.calculer(100.0, 0.0, 1.0) == 80.0);
    assert(!proportionnel.definirLimites(100.0, 0.0));

    Pid integral(0.0, 1.0, 0.0);

    for (int i = 0; i < 500; ++i)
    {
        assert(integral.calculer(100.0, 0.0, 1.0) <= 100.0);
    }

    assert(integral.calculer(0.0, 100.0, 1.0) == 0.0);

    integral.reinitialiser();

    assert(integral.calculer(10.0, 0.0, 1.0) == 10.0);

    std::cout << "Tests PID reussis" << std::endl;
    return 0;
}
