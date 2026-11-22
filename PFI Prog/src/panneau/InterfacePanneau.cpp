#include "panneau/InterfacePanneau.h"
#include <cmath>

InterfacePanneau::InterfacePanneau()
{
}

InterfacePanneau::~InterfacePanneau()
{
    deconnecter();
}

bool InterfacePanneau::connecter(const std::string& port)
{
    return panneau.open(port.c_str());
}

bool InterfacePanneau::lireCapteurs(PanelReading& lecture)
{
    return panneau.read(lecture);
}

bool InterfacePanneau::commanderValveGB(double ouverture)
{
    if (!std::isfinite(ouverture) ||
        ouverture < 0.0 || ouverture > 100.0)
    {
        return false;
    }

    return panneau.setValve(
        Valve::LargeTank, ouverture
    );
}

bool InterfacePanneau::commanderValvePB(double ouverture)
{
    if (!std::isfinite(ouverture) ||
        ouverture < 0.0 || ouverture > 100.0)
    {
        return false;
    }

    return panneau.setValve(
        Valve::SmallTank, ouverture
    );
}

bool InterfacePanneau::commanderPompe(bool marche)
{
    return panneau.setPump(marche);
}

void InterfacePanneau::deconnecter()
{
    panneau.close();
}

std::string InterfacePanneau::obtenirErreur() const
{
    return panneau.lastError();
}


bool InterfacePanneau::mettreEnSecuriteSimulateur()
{
    bool succes = true;

    // Arreter la pompe en priorite
    if (!panneau.setPump(false))
        succes = false;

    // Couper les arrivees d'eau
    if (!panneau.setHotWater(false))
        succes = false;

    if (!panneau.setColdWater(false))
        succes = false;

    // Fermer les valves proportionnelles
    if (!panneau.setValve(Valve::HotWater, 0.0))
        succes = false;

    if (!panneau.setValve(Valve::ColdWater, 0.0))
        succes = false;

    if (!panneau.setValve(Valve::LargeTank, 0.0))
        succes = false;

    if (!panneau.setValve(Valve::SmallTank, 0.0))
        succes = false;

    return succes;
}
