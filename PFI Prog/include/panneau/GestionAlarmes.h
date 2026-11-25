#pragma once

#include <chrono>

// DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA

class GestionAlarme
{
    using Horloge = std::chrono::steady_clock;

    Horloge::time_point debut{};

    bool mesureActive = false;
    bool emise = false;
    bool reconnue = false;

public:

    // Retourne true seulement si une nouvelle
    // notification doit etre envoyee.

    bool actualiser(
        bool defaut,
        bool ack,
        int generation,
        int repetition,
        Horloge::time_point maintenant = Horloge::now())
    {
        if (ack && emise)
        {
            emise = false;
            reconnue = true;
            debut = maintenant;
        }

        if (!defaut)
        {
            mesureActive = false;
            reconnue = false;
            return false;
        }

        if (!mesureActive)
        {
            mesureActive = true;
            debut = maintenant;
        }

        if (emise)
        {
            return false;
        }

        if (maintenant - debut <
            std::chrono::seconds(
                reconnue ? repetition : generation))
        {
            return false;
        }

        emise = true;
        reconnue = false;

        return true;
    }

    void publicationEchouee()
    {
        emise = false;
    }
};

// FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA