#include "Panneau.h"
#include "panneau/Pid.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>
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
        bool ok = panneau.setPump(false);
        ok = panneau.setValve(Valve::SmallTank, 0.0) && ok;
        ok = panneau.setValve(Valve::LargeTank, 0.0) && ok;

        if (!ok)
        {
            std::cerr << "ERREUR : arret non confirme : "
                      << panneau.lastError() << std::endl;
        }
        return ok;
    };

    if (!arreterActionneurs())
    {
        panneau.close();
        return 1;
    }

    const double consigne = 10.0;
    Pid pidPB(2.0, 0.02, 0.0);

    if (!pidPB.definirLimites(0.0, 25.0))
    {
        std::cerr << "Erreur limites PID" << std::endl;
        arreterActionneurs();
        panneau.close();
        return 1;
    }

    bool pompeActive = false;
    bool erreur = false;

    std::cout << "TEST PID PETIT BASSIN - SIMULATEUR" << std::endl;
    std::cout << "Consigne : " << consigne
              << " % | Duree max : 35 secondes" << std::endl;

    for (int i = 0; i < 35 && !arretDemande; ++i)
    {
        PanelReading lecture{};

        if (!panneau.read(lecture))
        {
            std::cerr << "Erreur lecture : "
                      << panneau.lastError() << std::endl;
            erreur = true;
            break;
        }

        if (lecture.debordementGrosBassin ||
            lecture.debordementPetitBassin)
        {
            std::cerr << "ARRET : debordement detecte" << std::endl;
            erreur = true;
            break;
        }

        const double niveau = lecture.niveauPetitBassin;

        if (!std::isfinite(niveau) ||
            !std::isfinite(lecture.niveauGrosBassin) ||
            niveau < -2.0 || niveau >= 15.0 ||
            lecture.niveauGrosBassin >= 95.0)
        {
            std::cerr << "ARRET : niveau invalide ou hors limites : "
                      << niveau << std::endl;
            erreur = true;
            break;
        }

        const double niveauPID = std::max(0.0, niveau);
        const double commande = pidPB.calculer(consigne, niveauPID, 1.0);

        if (!std::isfinite(commande))
        {
            std::cerr << "Erreur calcul PID" << std::endl;
            erreur = true;
            break;
        }

        if (arretDemande)
            break;

        if (pompeActive && commande <= 4.0)
        {
            if (!panneau.setPump(false))
            {
                std::cerr << "Erreur arret pompe : "
                          << panneau.lastError() << std::endl;
                erreur = true;
                break;
            }
            pompeActive = false;
        }

        const bool demarrerPompe = !pompeActive && commande >= 10.0;
        const double ouvertureAppliquee =
            (pompeActive || demarrerPompe) ? commande : 0.0;

        if (arretDemande)
            break;

        if (!panneau.setValve(Valve::SmallTank, ouvertureAppliquee))
        {
            std::cerr << "Erreur commande valve PB : "
                      << panneau.lastError() << std::endl;
            erreur = true;
            break;
        }

        if (demarrerPompe && !arretDemande)
        {
            if (!panneau.setPump(true))
            {
                std::cerr << "Erreur demarrage pompe : "
                          << panneau.lastError() << std::endl;
                erreur = true;
                break;
            }
            pompeActive = true;
        }

        std::cout << std::fixed << std::setprecision(2)
                  << "Temps : " << i << " s"
                  << " | Niveau PB : " << niveau << " %"
                  << " | Consigne : " << consigne << " %"
                  << " | Valve PB : " << ouvertureAppliquee << " %"
                  << " | Pompe : " << (pompeActive ? "ON" : "OFF")
                  << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    const bool arretOK = arreterActionneurs();
    panneau.close();

    std::cout << "Fin du test PID PB" << std::endl;
    return (erreur || !arretOK) ? 1 : 0;
}
