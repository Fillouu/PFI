
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <atomic>

#include "panneau/ControlePanneau.h"
#include "panneau/InterfacePanneau.h"
#include "commun/ClientMqtt.h"
#include "commun/TopicsMqtt.h"

// -1 : aucune commande
//  0 : manuel
//  1 : automatique
std::atomic<int> demandeMode{-1};
std::atomic<int> demandeConsigneGB{-1};
std::atomic<int> demandeConsignePB{-1};



bool convertirConsigne(const std::string& texte, int& valeur)
{
    if (texte.empty() || texte.size() > 3)
    {
        return false;
    }

    int resultat = 0;

    for (char caractere : texte)
    {
        if (caractere < '0' || caractere > '9')
        {
            return false;
        }

        resultat = resultat * 10 + (caractere - '0');
    }

    if (resultat > 100)
    {
        return false;
    }

    valeur = resultat;
    return true;
}



void traiterCommandeMqtt(
    const std::string& topic,
    const std::string& message)
{
    if (topic == TopicsMqtt::CMD_MODE)
    {
        if (message == "manuel")
        {
            demandeMode.store(0);
            std::cout << "\nCommande recue : manuel"
                      << std::endl;
        }
        else if (message == "auto")
        {
            demandeMode.store(1);
            std::cout << "\nCommande recue : auto"
                      << std::endl;
        }
        else
        {
            std::cerr << "\nMode MQTT invalide : "
                      << message << std::endl;
        }

        return;
    }

    int valeur = 0;

    if (topic == TopicsMqtt::CMD_NIV_GB)
    {
        if (convertirConsigne(message, valeur))
        {
            demandeConsigneGB.store(valeur);
        }
        else
        {
            std::cerr << "\nConsigne GB invalide"
                      << std::endl;
        }
    }
    else if (topic == TopicsMqtt::CMD_NIV_PB)
    {
        if (convertirConsigne(message, valeur))
        {
            demandeConsignePB.store(valeur);
        }
        else
        {
            std::cerr << "\nConsigne PB invalide"
                      << std::endl;
        }
    }
}




int main(int argc, char* argv[])
{
    std::cout << "PFI - Panneau RAM" << std::endl;

    if (argc < 3)
    {
        std::cerr << "Utilisation : "
                  << argv[0]
                  << " <port serie> <adresse broker MQTT>"
                  << std::endl;
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

    ClientMqtt mqtt;

    if (!mqtt.connecter(argv[2]))
    {
        std::cerr << "Impossible de lancer MQTT."
                << std::endl;
        return 1;
    }


    mqtt.definirGestionnaire(traiterCommandeMqtt);

    if (!mqtt.abonner(TopicsMqtt::CMD_MODE))
    {
        std::cerr << "Erreur abonnement MQTT au mode."
                << std::endl;

        mqtt.deconnecter();
        panneau.deconnecter();
        return 1;
    }

    std::cout << "Abonnement MQTT au mode demande." << std::endl;

    // Abonnement aux consignes des deux bassins
    if (!mqtt.abonner(TopicsMqtt::CMD_NIV_GB) ||
        !mqtt.abonner(TopicsMqtt::CMD_NIV_PB))
    {
        std::cerr << "Erreur abonnement consignes MQTT"
                  << std::endl;

        mqtt.deconnecter();
        panneau.deconnecter();
        return 1;
    }

    std::cout << "Abonnements aux consignes demandes."<< std::endl;


    ControlePanneau controle;

    // Publier le mode initial
    if (!mqtt.publier(
            TopicsMqtt::MODE,
            controle.obtenirMode()))
    {
        std::cerr << "Erreur publication mode initial"
                << std::endl;
    }



    while (true)
    {
        int commande = demandeMode.exchange(-1);

        if (commande != -1)
        {
            std::string nouveauMode =
                (commande == 1) ? "auto" : "manuel";

            if (controle.changerMode(nouveauMode))
            {
                std::cout << "\nMode actuel : "
                        << controle.obtenirMode()
                        << std::endl;

                if (!mqtt.publier(
                        TopicsMqtt::MODE,
                        controle.obtenirMode()))
                {
                    std::cerr << "Erreur publication du mode"
                            << std::endl;
                }
            }
        }


        int nouvelleConsigneGB = demandeConsigneGB.exchange(-1);
        int nouvelleConsignePB = demandeConsignePB.exchange(-1);

        if (nouvelleConsigneGB != -1)
        {
            if (controle.definirConsigneGB(nouvelleConsigneGB))
            {
                std::cout << "\nNouvelle consigne GB : "
                        << controle.obtenirConsigneGB()
                        << " %" << std::endl;
            }
        }

        if (nouvelleConsignePB != -1)
        {
            if (controle.definirConsignePB(nouvelleConsignePB))
            {
                std::cout << "\nNouvelle consigne PB : "
                        << controle.obtenirConsignePB()
                        << " %" << std::endl;
            }
        }

        PanelReading lecture{};

        if (!panneau.lireCapteurs(lecture))
        {
            std::cerr << "Erreur de lecture : "
                      << panneau.obtenirErreur()
                      << std::endl;

            mqtt.deconnecter();
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