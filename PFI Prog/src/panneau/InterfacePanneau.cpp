
#include "panneau/InterfacePanneau.h"

#include "panneau/InterfacePanneau.h"


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

void InterfacePanneau::deconnecter()
{
    panneau.close();
}

std::string InterfacePanneau::obtenirErreur() const
{
    return panneau.lastError();
}