
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "panneau/InterfacePanneau.h"


int main(int argc, char* argv[])
{
    std::cout << "PFI - Panneau RAM" << std::endl;

    if (argc < 2)
    {
        std::cerr << "Utilisation : "
                  << argv[0]
                  << " <port serie>" << std::endl;
        return 1;
    }

    InterfacePanneau panneau;

    // Connexion au panneau
    if (!panneau.connecter(argv[1]))
    {
        std::cerr << "Erreur de connexion : "
                  << panneau.obtenirErreur()
                  << std::endl;
        return 1;
    }

    std::cout << "Connexion ouverte." << std::endl;

    while (true)
    {
        PanelReading lecture{};

        if (!panneau.lireCapteurs(lecture))
        {
            std::cerr << "Erreur de lecture : "
                      << panneau.obtenirErreur()
                      << std::endl;

            panneau.deconnecter();
            return 1;
        }

        std::cout << "\n===== PANNEAU RAM =====" << std::endl;

        std::cout << "Grand bassin : "
                  << lecture.niveauGrosBassin
                  << " %" << std::endl;

        std::cout << "Petit bassin : "
                  << lecture.niveauPetitBassin
                  << " %" << std::endl;

        std::cout << "Temperature : "
                  << lecture.temperaturePetitBassin
                  << " C" << std::endl;

        std::cout << "Debordement GB : "
                  << (lecture.debordementGrosBassin ? "OUI" : "NON")
                  << std::endl;

        std::cout << "Debordement PB : "
                  << (lecture.debordementPetitBassin ? "OUI" : "NON")
                  << std::endl;

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    panneau.deconnecter();
    return 0;
}