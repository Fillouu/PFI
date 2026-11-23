#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <csignal>

#include "panneau/ControlePanneau.h"
#include "panneau/InterfacePanneau.h"
#include "panneau/RegulationBassins.h"
#include "commun/ClientMqtt.h"
#include "commun/TopicsMqtt.h"

// -1 : aucune commande
//  0 : manuel
//  1 : automatique
std::atomic<int> demandeMode{-1};
std::atomic<int> demandeConsigneGB{-1};
std::atomic<int> demandeConsignePB{-1};

std::atomic<int> demandeValveManuelleGB{-1};
std::atomic<int> demandeValveManuellePB{-1};
std::atomic<int> demandePompeManuelle{-1};


volatile std::sig_atomic_t demandeArretUrgence = 0;



void signalArretUrgence(int)
{
    demandeArretUrgence = 1;
}


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

    if (topic == TopicsMqtt::CMD_VALVE_GB)
    {
        if (convertirConsigne(message, valeur) && valeur <= 25)
        {
            demandeValveManuelleGB.store(valeur);
            std::cout << "\nCommande valve GB : "
                      << valeur << " %" << std::endl;
        }
        else
        {
            std::cerr << "\nCommande manuelle GB invalide"
                      << std::endl;
        }
    }
    else if (topic == TopicsMqtt::CMD_VALVE_PB)
    {
        if (convertirConsigne(message, valeur) && valeur <= 25)
        {
            demandeValveManuellePB.store(valeur);
            std::cout << "\nCommande valve PB : "
                      << valeur << " %" << std::endl;
        }
        else
        {
            std::cerr << "\nCommande manuelle PB invalide"
                      << std::endl;
        }
    }
    else if (topic == TopicsMqtt::CMD_POMPE)
    {
        if (message == "on" || message == "1")
        {
            demandePompeManuelle.store(1);
        }
        else if (message == "off" || message == "0")
        {
            demandePompeManuelle.store(0);
        }
        else
        {
            std::cerr << "\nCommande pompe invalide"
                      << std::endl;
        }
    }
}


namespace
{
    volatile std::sig_atomic_t finProgrammeDemandee = 0;

    void demanderFinProgramme(int)
    {
        finProgrammeDemandee = 1;
    }
}




int main(int argc, char* argv[])
{
    std::cout << "PFI - Panneau RAM" << std::endl;

    std::signal(SIGUSR1, signalArretUrgence);
    std::signal(SIGINT, demanderFinProgramme);
    std::signal(SIGTERM, demanderFinProgramme);

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

    if (!mqtt.abonner(TopicsMqtt::CMD_VALVE_GB) ||
        !mqtt.abonner(TopicsMqtt::CMD_VALVE_PB) ||
        !mqtt.abonner(TopicsMqtt::CMD_POMPE))
    {
        std::cerr << "Erreur abonnement commandes manuelles"
                  << std::endl;

        panneau.mettreEnSecuriteSimulateur();
        mqtt.deconnecter();
        panneau.deconnecter();
        return 1;
    }

    std::cout << "Abonnements manuels demandes."
              << std::endl;

    ControlePanneau controle;

    RegulationBassins regulation;

    bool sortiesAutoActives = false;

    double valveManuelleGB = 0.0;
    double valveManuellePB = 0.0;
    bool pompeManuelle = false;
    bool commandesManuellesActives = false;

    const bool simulation =
        std::string(argv[1]) == "/tmp/ttyRAM";


    // Publier le mode initial
    if (!mqtt.publier(
            TopicsMqtt::MODE,
            controle.obtenirMode()))
    {
        std::cerr << "Erreur publication mode initial"
                << std::endl;
    }


    while (!finProgrammeDemandee)
    {
        if (demandeArretUrgence != 0 &&
            !controle.estArretUrgenceActif())
        {
            controle.declencherArretUrgence();
            demandeMode.store(-1);

            std::cerr << "\nARRET LOGICIEL DEMANDE"
                    << std::endl;

            if (panneau.mettreEnSecuriteSimulateur())
            {
                std::cout << "Commandes de mise en securite acceptees"
                        << std::endl;
            }
            else
            {
                std::cerr << "ERREUR : mise en securite non confirmee : "
                        << panneau.obtenirErreur()
                        << std::endl;
            }

            if (!mqtt.publier(TopicsMqtt::MODE,
                            controle.obtenirMode()))
            {
                std::cerr << "Erreur publication mode MQTT"
                        << std::endl;
            }
        }



        int commande = demandeMode.exchange(-1);


        if (commande != -1)
        {
            std::string nouveauMode =
                (commande == 1) ? "auto" : "manuel";

            if (controle.changerMode(nouveauMode))
            {
                regulation.reinitialiser();

                demandeValveManuelleGB.store(-1);
                demandeValveManuellePB.store(-1);
                demandePompeManuelle.store(-1);

                valveManuelleGB = 0.0;
                valveManuellePB = 0.0;
                pompeManuelle = false;
                commandesManuellesActives = false;

                if (!panneau.mettreEnSecuriteSimulateur())
                {
                    controle.declencherArretUrgence();

                    std::cerr
                        << "ERREUR : changement de mode non securise"
                        << std::endl;
                }

                sortiesAutoActives = false;

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
            const std::string erreurSerie =
                panneau.obtenirErreur();

            std::cerr << "\nALARME : PERTE COMMUNICATION SERIE"
                      << std::endl;

            std::cerr << "Detail : " << erreurSerie
                      << std::endl;

            controle.declencherArretUrgence();
            regulation.reinitialiser();
            demandeMode.store(-1);
            sortiesAutoActives = false;

            const bool securiteOK =
                panneau.mettreEnSecuriteSimulateur();

            if (!securiteOK)
            {
                std::cerr
                    << "ERREUR CRITIQUE : mise en securite "
                    << "non confirmee"
                    << std::endl;
            }

            if (!mqtt.publier(
                    TopicsMqtt::ALARMES,
                    "PERTE_COMMUNICATION_SERIE"))
            {
                std::cerr
                    << "Erreur publication alarme MQTT"
                    << std::endl;
            }

            mqtt.deconnecter();
            panneau.deconnecter();

            return 1;
        }


        const bool debordement =
            lecture.debordementGrosBassin ||
            lecture.debordementPetitBassin;

        const bool niveauInvalide =
            !std::isfinite(lecture.niveauGrosBassin) ||
            !std::isfinite(lecture.niveauPetitBassin) ||
            lecture.niveauGrosBassin < -2.0 ||
            lecture.niveauPetitBassin < -2.0 ||
            lecture.niveauGrosBassin >= 95.0 ||
            lecture.niveauPetitBassin >= 95.0;

        

        
        if ((debordement || niveauInvalide) && !controle.estArretUrgenceActif())
        {
            controle.declencherArretUrgence();
            regulation.reinitialiser();
            demandeMode.store(-1);

            std::string alarme;

            if (debordement)
            {
                alarme = "DEBORDEMENT";

                std::cerr
                    << "\nALARME : DEBORDEMENT DETECTE"
                    << std::endl;
            }
            else
            {
                alarme = "NIVEAU_INVALIDE";

                std::cerr
                    << "\nALARME : MESURE DE NIVEAU INVALIDE"
                    << std::endl;
            }

            std::cerr << "GB : "
                      << lecture.niveauGrosBassin
                      << " % | PB : "
                      << lecture.niveauPetitBassin
                      << " %" << std::endl;

            const bool securiteOK =
                panneau.mettreEnSecuriteSimulateur();

            if (!securiteOK)
            {
                std::cerr
                    << "ERREUR CRITIQUE : mise en securite "
                    << "non confirmee"
                    << std::endl;
            }

            if (!mqtt.publier(TopicsMqtt::ALARMES, alarme))
            {
                std::cerr
                    << "Erreur publication alarme MQTT"
                    << std::endl;
            }

            if (!mqtt.publier(
                    TopicsMqtt::MODE,
                    controle.obtenirMode()))
            {
                std::cerr
                    << "Erreur publication mode MQTT"
                    << std::endl;
            }
        }


        const bool modeAuto =
            controle.obtenirMode() == "auto";

        const bool consignesValides =
            controle.obtenirConsigneGB() > 0 &&
            controle.obtenirConsignePB() > 0;

        const bool regulationAutorisee =
            simulation &&
            modeAuto &&
            consignesValides &&
            !controle.estArretUrgenceActif() &&
            !debordement &&
            !niveauInvalide;

        if (regulationAutorisee)
        {
            CommandesBassins cmd = regulation.calculer(
                controle.obtenirConsigneGB(),
                controle.obtenirConsignePB(),
                lecture.niveauGrosBassin,
                lecture.niveauPetitBassin,
                1.0,
                true
            );

            bool succes = true;

            if (!cmd.pompe)
            {
                succes = panneau.commanderPompe(false);
            }

            if (succes)
            {
                succes = panneau.commanderValveGB(cmd.valveGB);
            }

            if (succes)
            {
                succes = panneau.commanderValvePB(cmd.valvePB);
            }

            if (succes && cmd.pompe)
            {
                succes = panneau.commanderPompe(true);
            }

            if (!succes)
            {
                controle.declencherArretUrgence();
                regulation.reinitialiser();

                std::cerr << "ARRET : erreur commande actionneur : "
                        << panneau.obtenirErreur()
                        << std::endl;

                if (!panneau.mettreEnSecuriteSimulateur())
                {
                    std::cerr << "ERREUR CRITIQUE : "
                            << "mise en securite non confirmee"
                            << std::endl;
                }

                sortiesAutoActives = false;
            }
            else
            {
                sortiesAutoActives = true;

                std::cout << "PID GB : " << cmd.valveGB
                        << " % | PID PB : " << cmd.valvePB
                        << " % | Pompe : "
                        << (cmd.pompe ? "ON" : "OFF")
                        << std::endl;
            }
        }
        else
        {
            regulation.reinitialiser();

            if (sortiesAutoActives ||
                controle.estArretUrgenceActif())
            {
                if (!panneau.mettreEnSecuriteSimulateur())
                {
                    std::cerr << "ERREUR CRITIQUE : "
                            << "arret des actionneurs non confirme"
                            << std::endl;

                    controle.declencherArretUrgence();
                }

                sortiesAutoActives = false;
            }
        }


        const int nouvelleValveGB =
            demandeValveManuelleGB.exchange(-1);

        const int nouvelleValvePB =
            demandeValveManuellePB.exchange(-1);

        const int nouvellePompe =
            demandePompeManuelle.exchange(-1);

        const bool manuelAutorise =
            simulation &&
            controle.obtenirMode() == "manuel" &&
            !controle.estArretUrgenceActif() &&
            !debordement &&
            !niveauInvalide;

        if (manuelAutorise)
        {
            bool modification = false;

            if (nouvelleValveGB >= 0)
            {
                valveManuelleGB = nouvelleValveGB;
                modification = true;
            }

            if (nouvelleValvePB >= 0)
            {
                valveManuellePB = nouvelleValvePB;
                modification = true;
            }

            if (nouvellePompe >= 0)
            {
                pompeManuelle = nouvellePompe == 1;
                modification = true;
            }

            if (modification || commandesManuellesActives)
            {
                const bool demandePompe =
                    pompeManuelle &&
                    (valveManuelleGB > 0.0 ||
                     valveManuellePB > 0.0);

                bool succes = true;

                if (!demandePompe)
                {
                    succes = panneau.commanderPompe(false);
                }

                if (succes)
                {
                    succes = panneau.commanderValveGB(
                        valveManuelleGB);
                }

                if (succes)
                {
                    succes = panneau.commanderValvePB(
                        valveManuellePB);
                }

                if (succes && demandePompe)
                {
                    succes = panneau.commanderPompe(true);
                }

                if (!succes)
                {
                    controle.declencherArretUrgence();

                    std::cerr
                        << "ARRET : erreur commande manuelle"
                        << std::endl;

                    if (!panneau.mettreEnSecuriteSimulateur())
                    {
                        std::cerr
                            << "ERREUR CRITIQUE : "
                            << "mise en securite non confirmee"
                            << std::endl;
                    }

                    commandesManuellesActives = false;
                    pompeManuelle = false;
                }
                else
                {
                    commandesManuellesActives = true;

                    std::cout
                        << "MANUEL GB : " << valveManuelleGB
                        << " % | PB : " << valveManuellePB
                        << " % | Pompe : "
                        << (demandePompe ? "ON" : "OFF")
                        << std::endl;
                }
            }
        }
        else if (commandesManuellesActives)
        {
            if (!panneau.mettreEnSecuriteSimulateur())
            {
                std::cerr
                    << "ERREUR CRITIQUE : "
                    << "arret manuel non confirme"
                    << std::endl;

                controle.declencherArretUrgence();
            }

            commandesManuellesActives = false;
            pompeManuelle = false;
            valveManuelleGB = 0.0;
            valveManuellePB = 0.0;
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

        std::cout << "Ouverture valve GB : "
          << lecture.ouvertureValve[1]
          << " %" << std::endl;

        std::cout << "Ouverture valve PB : "
                << lecture.ouvertureValve[2]
                << " %" << std::endl;

        std::cout << "Courant valve PB : "
                << lecture.courant[2]
                << " mA" << std::endl;          

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    
    
    std::cout << "\nARRET DU PROGRAMME DEMANDE"
              << std::endl;

    regulation.reinitialiser();

    const bool securiteOK =
        panneau.mettreEnSecuriteSimulateur();

    if (securiteOK)
    {
        std::cout << "Commandes de mise en securite acceptees"
                  << std::endl;
    }
    else
    {
        std::cerr << "ERREUR CRITIQUE : "
                  << "mise en securite non confirmee : "
                  << panneau.obtenirErreur()
                  << std::endl;
    }

    mqtt.deconnecter();
    panneau.deconnecter();

    std::cout << "Fermeture du programme Panneau RAM"
              << std::endl;

    return securiteOK ? 0 : 1;
}