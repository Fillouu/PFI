
#include "panneau/ControlePanneau.h"

// DEBUT DU CODE GENERE AVEC L'AIDE D'UNE IA

ControlePanneau::ControlePanneau()
    : modeActuel(ModePanneau::MANUEL)
{
}

bool ControlePanneau::changerMode(
    const std::string& nouveauMode)
{
    if (nouveauMode == "manuel")
    {
        modeActuel = ModePanneau::MANUEL;
        return true;
    }

    if (nouveauMode == "auto")
    {
        modeActuel = ModePanneau::AUTOMATIQUE;
        return true;
    }

    return false;
}

std::string ControlePanneau::obtenirMode() const
{
    if (modeActuel == ModePanneau::AUTOMATIQUE)
    {
        return "auto";
    }

    return "manuel";
}

// FIN DU CODE GENERE AVEC L'AIDE D'UNE IA
