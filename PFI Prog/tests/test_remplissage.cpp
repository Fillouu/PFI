
#include "Panneau.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

volatile std::sig_atomic_t arreter = 0;

void signalArret(int)
{
    arreter = 1;
}

int main()
{
    std::signal(SIGINT, signalArret);

    Panel panneau;

    if (!panneau.open("/tmp/ttyRAM"))
    {
        std::cerr << panneau.lastError() << std::endl;
        return 1;
    }

    auto toutArreter = [&]()
    {
        bool ok = panneau.setPump(false);
        ok = panneau.setValve(Valve::LargeTank, 0.0) && ok;
        if (!ok)
            std::cerr << "Arret non confirme : "
                      << panneau.lastError() << std::endl;
    };

    toutArreter();

    if (!panneau.setValve(Valve::LargeTank, 50.0))
    {
        std::cerr << panneau.lastError() << std::endl;
        toutArreter();
        return 1;
    }

    if (!panneau.setPump(true))
    {
        std::cerr << panneau.lastError() << std::endl;
        toutArreter();
        return 1;
    }

    std::cout << "Pompe active - essai de 8 secondes"
              << std::endl;

    for (int i = 0; i < 8 && !arreter; ++i)
    {
        PanelReading lecture{};

        if (!panneau.read(lecture) ||
            lecture.debordementGrosBassin ||
            lecture.debordementPetitBassin)
        {
            std::cerr << "Lecture invalide ou debordement"
                      << std::endl;
            break;
        }

        
        std::cout << "Niveau GB : "
                << lecture.niveauGrosBassin
                << " % | Niveau PB : "
                << lecture.niveauPetitBassin
                << " %" << std::endl;

        std::cout << "Valve GB : "
                << lecture.ouvertureValve[1]
                << " % | Courant valve GB : "
                << lecture.courant[1]
                << " mA" << std::endl;


        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }

    toutArreter();
    panneau.close();

    std::cout << "Fin du test" << std::endl;
    return 0;
}
