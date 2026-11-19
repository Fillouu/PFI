
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


bool ControlePanneau::definirConsigneGB(int consigne)
{
    if (consigne < 0 || consigne > 100)
    {
        return false;
    }

    consigneGB = consigne;
    return true;
}

bool ControlePanneau::definirConsignePB(int consigne)
{
    if (consigne < 0 || consigne > 100)
    {
        return false;
    }

    consignePB = consigne;
    return true;
}

int ControlePanneau::obtenirConsigneGB() const
{
    return consigneGB;
}

int ControlePanneau::obtenirConsignePB() const
{
    return consignePB;
}