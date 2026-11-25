
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
        std::cerr << panneau.lastError() << std::endl;
        return 1;
    }

    auto arreterActionneurs = [&]()
    {
        bool ok = panneau.setPump(false);
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

    Pid pidGB(5.0, 0.9, 2.0);

    if (!pidGB.definirLimites(0.0, 50.0))
    {
        std::cerr << "Erreur limites PID" << std::endl;
        panneau.close();
        return 1;
    }

    bool pompeActive = false;
    bool erreur = false;

    std::cout << "TEST PID GRAND BASSIN - SIMULATEUR"
              << std::endl;

    std::cout << "Consigne : " << consigne
              << " % | Duree max : 35 secondes"
              << std::endl;

    for (int i = 0; i < 35 && !arretDemande; ++i)
    {
        PanelReading lecture{};

        if (!panneau.read(lecture))
        {
            std::cerr << "Erreur de lecture : "
                      << panneau.lastError() << std::endl;
            erreur = true;
            break;
        }

        if (lecture.debordementGrosBassin ||
            lecture.debordementPetitBassin)
        {
            std::cerr << "ARRET : debordement detecte"
                      << std::endl;
            erreur = true;
            break;
        }

        double niveau = lecture.niveauGrosBassin;

        if (!std::isfinite(niveau) ||
            niveau < -2.0 ||
            niveau > 95.0)
        {
            std::cerr << "ARRET : niveau hors limites : "
                      << niveau << std::endl;
            erreur = true;
            break;
        }

        double niveauPID = std::max(0.0, niveau);

        double commande =
            pidGB.calculer(consigne, niveauPID, 1.0);

        if (!std::isfinite(commande))
        {
            std::cerr << "Erreur calcul PID" << std::endl;
            erreur = true;
            break;
        }

        // Hysteresis de la pompe
        // Arret lorsque la commande <= 4 %
        // Demarrage lorsque la commande >= 10 %

        if (pompeActive && commande <= 4.0)
        {
            if (!panneau.setPump(false))
            {
                std::cerr << "Erreur arret pompe : "
                          << panneau.lastError()
                          << std::endl;
                erreur = true;
                break;
            }

            pompeActive = false;
        }

        bool demarrerPompe =
            !pompeActive && commande >= 10.0;

        double ouvertureAppliquee =
            (pompeActive || demarrerPompe)
                ? commande
                : 0.0;

        if (arretDemande)
        {
            break;
        }

        // Appliquer l'ouverture de la valve
        if (!panneau.setValve(
                Valve::LargeTank,
                ouvertureAppliquee))
        {
            std::cerr << "Erreur commande valve : "
                      << panneau.lastError()
                      << std::endl;
            erreur = true;
            break;
        }

        // Demarrer la pompe seulement si necessaire
        if (demarrerPompe && !arretDemande)
        {
            if (!panneau.setPump(true))
            {
                std::cerr << "Erreur demarrage pompe : "
                          << panneau.lastError()
                          << std::endl;
                erreur = true;
                break;
            }

            pompeActive = true;
        }

        std::cout << std::fixed << std::setprecision(2)
                  << "Temps : " << i << " s"
                  << " | Niveau : " << niveau << " %"
                  << " | Consigne : " << consigne << " %"
                  << " | Valve GB : " << ouvertureAppliquee << " %"
                  << " | Pompe : "
                  << (pompeActive ? "ON" : "OFF")
                  << std::endl;

        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }

    bool arretOK = arreterActionneurs();
    panneau.close();

    std::cout << "Fin du test PID" << std::endl;

    return (erreur || !arretOK) ? 1 : 0;
}
