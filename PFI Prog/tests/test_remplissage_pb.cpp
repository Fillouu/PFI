
#include "Panneau.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <csignal>

volatile std::sig_atomic_t arretDemande = 0;

void demanderArret(int)
{
    arretDemande = 1;
}

int main()
{
    std::signal(SIGINT, demanderArret);
    std::signal(SIGTERM, demanderArret);
    std::signal(SIGUSR1, demanderArret);

    Panel panneau;

    if (!panneau.open("/tmp/ttyRAM"))
    {
        std::cerr << "Erreur connexion : "
                  << panneau.lastError() << std::endl;
        return 1;
    }

    auto arreterActionneurs = [&]()
    {
        bool succes = panneau.setPump(false);

        if (!panneau.setValve(Valve::SmallTank, 0.0))
            succes = false;

        if (!panneau.setValve(Valve::LargeTank, 0.0))
            succes = false;

        if (!succes)
        {
            std::cerr << "ERREUR : arret non confirme : "
                      << panneau.lastError() << std::endl;
        }

        return succes;
    };

    if (!arreterActionneurs())
    {
        panneau.close();
        return 1;
    }

    if (!panneau.setValve(Valve::SmallTank, 40.0))
    {
        std::cerr << "Erreur valve PB : "
                  << panneau.lastError() << std::endl;

        arreterActionneurs();
        panneau.close();
        return 1;
    }

    if (!panneau.setPump(true))
    {
        std::cerr << "Erreur pompe : "
                  << panneau.lastError() << std::endl;

        arreterActionneurs();
        panneau.close();
        return 1;
    }

    std::cout << "TEST REMPLISSAGE PETIT BASSIN"
              << std::endl;

    std::cout << "Valve PB : 40 %"
              << " | Duree max : 15 secondes"
              << std::endl;

    bool erreur = false;

    for (int i = 0; i < 15 && !arretDemande; ++i)
    {
        PanelReading lecture{};

        if (!panneau.read(lecture))
        {
            std::cerr << "Erreur lecture : "
                      << panneau.lastError() << std::endl;
            erreur = true;
            break;
        }

        if (!std::isfinite(lecture.niveauPetitBassin) ||
            !std::isfinite(lecture.niveauGrosBassin) ||
            lecture.niveauPetitBassin < -2.0 ||
            lecture.niveauPetitBassin >= 20.0 ||
            lecture.niveauGrosBassin >= 95.0 ||
            lecture.debordementPetitBassin ||
            lecture.debordementGrosBassin)
        {
            std::cerr << "ARRET : niveau invalide ou limite"
                      << std::endl;
            erreur = true;
            break;
        }

        std::cout << std::fixed << std::setprecision(2)
                  << "Temps : " << i << " s"
                  << " | Niveau PB : "
                  << lecture.niveauPetitBassin << " %"
                  << " | Valve PB : "
                  << lecture.ouvertureValve[2] << " %"
                  << " | Courant PB : "
                  << lecture.courant[2] << " mA"
                  << " | Niveau GB : "
                  << lecture.niveauGrosBassin << " %"
                  << std::endl;

        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }

    bool arretOK = arreterActionneurs();

    panneau.close();

    std::cout << "Fin du test PB" << std::endl;

    return (erreur || !arretOK) ? 1 : 0;
}
