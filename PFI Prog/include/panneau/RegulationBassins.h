
#pragma once

#include "panneau/Pid.h"

struct CommandesBassins
{
    double valveGB = 0.0;
    double valvePB = 0.0;
    bool pompe = false;
};

class RegulationBassins
{
public:
    RegulationBassins();

    CommandesBassins calculer(double consigneGB, double consignePB,
                             double niveauGB, double niveauPB,
                             double dt, bool autorise);

    void reinitialiser();

private:
    Pid pidGB_;
    Pid pidPB_;
    bool pompeActive_ = false;
};
