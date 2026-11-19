
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

#include "panneau/InterfacePanneau.h"
#include "commun/ClientMqtt.h"
#include "commun/TopicsMqtt.h"

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

    // Connexion au broker MQTT
    if (argc < 3)
    {
        std::cerr << "Adresse du broker MQTT manquante."
                << std::endl;
        return 1;
    }

    ClientMqtt mqtt;

    if (!mqtt.connecter(argv[2]))
    {
        std::cerr << "Impossible de lancer MQTT."
                << std::endl;
        return 1;
    }

    // Premier test de publication MQTT
    if (mqtt.publier("RAM/panneau/etats/NivGB", "50"))
    {
        std::cout << "Message MQTT envoye." << std::endl;
    }
    else
    {
        std::cerr << "Erreur de publication MQTT."
                  << std::endl;
    }

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

        // Publication des niveaux mesures
        int niveauGB = static_cast<int>(
            std::lround(lecture.niveauGrosBassin)
        );

        int niveauPB = static_cast<int>(
            std::lround(lecture.niveauPetitBassin)
        );

        // Limiter les valeurs MQTT entre 0 et 100 %
        niveauGB = std::clamp(niveauGB, 0, 100);
        niveauPB = std::clamp(niveauPB, 0, 100);

        if (!mqtt.publier(TopicsMqtt::NIV_GB,std::to_string(niveauGB)))
        {
            std::cerr << "Erreur publication niveau GB" << std::endl;
        }

        if (!mqtt.publier(TopicsMqtt::NIV_PB,std::to_string(niveauPB)))
        {
            std::cerr << "Erreur publication niveau PB" << std::endl;
        }

        // Publication de la temperature du petit bassin
        int temperaturePB = static_cast<int>(std::lround(lecture.temperaturePetitBassin));

        if (!mqtt.publier(TopicsMqtt::TMP_PB,std::to_string(temperaturePB)))
        {
            std::cerr << "Erreur publication temperature PB" << std::endl;
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

    mqtt.deconnecter();

    panneau.deconnecter();
    return 0;
}