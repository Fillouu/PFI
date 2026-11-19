
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

private:
    struct mosquitto* client;
    bool connecte;
};