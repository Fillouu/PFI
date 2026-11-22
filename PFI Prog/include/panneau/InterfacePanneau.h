
#pragma once

#include <string>
#include "Panneau.h"

class InterfacePanneau
{
public:
    InterfacePanneau();
    ~InterfacePanneau();

    bool connecter(const std::string& port);
    bool lireCapteurs(PanelReading& lecture);

    // Commandes des actionneurs
    bool commanderValveGB(double ouverture);
    bool commanderValvePB(double ouverture);
    bool commanderPompe(bool marche);
    
    void deconnecter();
    std::string obtenirErreur() const;


private:
    Panel panneau;
};
