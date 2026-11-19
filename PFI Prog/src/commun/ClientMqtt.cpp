
#include "commun/ClientMqtt.h"

#include <iostream>


ClientMqtt::ClientMqtt()
    : client(nullptr), connecte(false)
{
    mosquitto_lib_init();

    client = mosquitto_new("PFI_Panneau_RAM", true, nullptr);

    if (client == nullptr)
    {
        std::cerr << "Erreur creation client MQTT." << std::endl;
    }
}

ClientMqtt::~ClientMqtt()
{
    deconnecter();

    if (client != nullptr)
    {
        mosquitto_destroy(client);
    }

    mosquitto_lib_cleanup();
}

bool ClientMqtt::connecter(const std::string& adresse, int port)
{
    if (client == nullptr)
    {
        return false;
    }

    int resultat = mosquitto_connect(
        client,
        adresse.c_str(),
        port,
        60
    );

    if (resultat != MOSQ_ERR_SUCCESS)
    {
        std::cerr << "Erreur MQTT : "
                  << mosquitto_strerror(resultat)
                  << std::endl;

        return false;
    }

    resultat = mosquitto_loop_start(client);

    if (resultat != MOSQ_ERR_SUCCESS)
    {
        std::cerr << "Erreur boucle MQTT : "
                  << mosquitto_strerror(resultat)
                  << std::endl;

        mosquitto_disconnect(client);
        return false;
    }

    connecte = true;

    std::cout << "Connexion MQTT demarree." << std::endl;
    return true;
}

bool ClientMqtt::publier(const std::string& topic,
                         const std::string& message)
{
    if (!connecte || client == nullptr)
    {
        return false;
    }

    int resultat = mosquitto_publish(
        client,
        nullptr,
        topic.c_str(),
        static_cast<int>(message.size()),
        message.c_str(),
        0,
        false
    );

    return resultat == MOSQ_ERR_SUCCESS;
}

void ClientMqtt::deconnecter()
{
    if (connecte && client != nullptr)
    {
        mosquitto_disconnect(client);
        mosquitto_loop_stop(client, false);
        connecte = false;
    }
}

bool ClientMqtt::estConnecte() const
{
    return connecte;
}