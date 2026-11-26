#pragma once
#include <chrono>


class GestionAlarme
{
    using Horloge = std::chrono::steady_clock;

    Horloge::time_point debut{};

    bool mesureActive = false;
    bool emise = false;
    bool reconnue = false;

public:
    // Retourne true seulement si une nouvelle notification doit etre envoyee.
    bool actualiser(
        bool defaut, 
        bool ack, 
        int generation, 
        int repetition,
        Horloge::time_point maintenant = Horloge::now()
    );
    bool estEmise() const;

    void publicationEchouee();
};