
#pragma once

#include <string>
#include <mosquitto.h>


class ClientMqtt
{
public:
    ClientMqtt();
    ~ClientMqtt();

    bool connecter(const std::string& adresse, int port = 1883);

    bool publier(const std::string& topic,
                 const std::string& message);

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

    GestionnaireMessage gestionnaireMessage = nullptr;

    static void receptionMessage(
        struct mosquitto* client,
        void* contexte,
        const struct mosquitto_message* message
    );
};