
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
    void deconnecter();

    std::string obtenirErreur() const;


private:
    Panel panneau;
};
