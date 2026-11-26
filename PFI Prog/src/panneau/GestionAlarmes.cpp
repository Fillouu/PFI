#include "panneau/GestionAlarmes.h"


// Actualise l'etat de l'alarme selon le defaut,
// la reconnaissance et les temporisations.
bool GestionAlarme::actualiser(
    bool defaut,
    bool ack,
    int generation,
    int repetition,
    Horloge::time_point maintenant)
{
    // Reconnaissance de l'alarme.
    if (ack && emise)
    {
        emise = false;
        reconnue = true;
        debut = maintenant;
    }

    // Retour du niveau dans les limites normales.
    if (!defaut)
    {
        mesureActive = false;
        reconnue = false;
        return false;
    }

    // Debut de la condition d'alarme.
    if (!mesureActive)
    {
        mesureActive = true;
        debut = maintenant;
    }

    // Attend la reconnaissance de l'usager.
    if (emise)
        return false;

    // Selection de la temporisation.
    int delai = reconnue ? repetition : generation;

    if (maintenant - debut < std::chrono::seconds(delai))
        return false;

    // Generation ou regeneration de l'alarme.
    emise = true;
    reconnue = false;

    return true;
}

// Retourne l'etat de l'alarme.
bool GestionAlarme::estEmise() const
{
    return emise;
}

// Permet une nouvelle tentative de publication.
void GestionAlarme::publicationEchouee()
{
    emise = false;
}

