// DEBUT DU FICHIER CONTENANT DES MODIFICATIONS AVEC L'AIDE D'UNE IA

#pragma once

#include <string>
#include <atomic>
#include <mosquitto.h>

class ClientMqtt
{
public:
    ClientMqtt();
    ~ClientMqtt();

    bool connecter(const std::string& adresse, int port = 1883);

    bool publier(const std::string& topic,
                 const std::string& message,
                 bool retained = false);

    void deconnecter();

    bool estConnecte() const;

    using GestionnaireMessage = void (*)(
        const std::string& topic,
        const std::string& message
    );

    bool abonner(const std::string& topic);
    void definirGestionnaire(GestionnaireMessage gestionnaire);

private:
    struct mosquitto* client;
    bool connecte;

    std::atomic<bool> liaisonPerdue{false};

    static void perteConnexion(
        struct mosquitto*,
        void*,
        int
    );

    GestionnaireMessage gestionnaireMessage = nullptr;

    static void receptionMessage(
        struct mosquitto* client,
        void* contexte,
        const struct mosquitto_message* message
    );
};

// FIN DU FICHIER CONTENANT DES MODIFICATIONS AVEC L'AIDE D'UNE IA