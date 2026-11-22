
#pragma once

#include <string>


enum class ModePanneau
{
    MANUEL,
    AUTOMATIQUE
};

class ControlePanneau
{
public:
    ControlePanneau();

    bool changerMode(const std::string& nouveauMode);

    std::string obtenirMode() const;

    bool definirConsigneGB(int consigne);
    bool definirConsignePB(int consigne);

    int obtenirConsigneGB() const;
    int obtenirConsignePB() const;

    void declencherArretUrgence();
    bool estArretUrgenceActif() const;

private:
    ModePanneau modeActuel;

    bool arretUrgenceActif = false;

    int consigneGB = 0;
    int consignePB = 0;
};

