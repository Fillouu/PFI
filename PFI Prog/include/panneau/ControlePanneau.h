
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

private:
    ModePanneau modeActuel;
};

