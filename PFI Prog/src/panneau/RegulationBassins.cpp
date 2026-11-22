
#include "panneau/RegulationBassins.h"

#include <algorithm>
#include <cmath>

RegulationBassins::RegulationBassins()
    : pidGB_(4.0, 0.03, 0.0),
      pidPB_(2.0, 0.02, 0.0)
{
    pidGB_.definirLimites(0.0, 50.0);
    pidPB_.definirLimites(0.0, 25.0);
}

void RegulationBassins::reinitialiser()
{
    pidGB_.reinitialiser();
    pidPB_.reinitialiser();
    pompeActive_ = false;
}

CommandesBassins RegulationBassins::calculer(
    double consigneGB, double consignePB,
    double niveauGB, double niveauPB,
    double dt, bool autorise)
{
    if (!autorise ||
        !std::isfinite(niveauGB) || !std::isfinite(niveauPB) ||
        !std::isfinite(consigneGB) || !std::isfinite(consignePB) ||
        !std::isfinite(dt) || dt <= 0.0 ||
        niveauGB < -2.0 || niveauPB < -2.0 ||
        niveauGB >= 95.0 || niveauPB >= 95.0 ||
        consigneGB < 0.0 || consigneGB > 100.0 ||
        consignePB < 0.0 || consignePB > 100.0)
    {
        reinitialiser();
        return {};
    }

    const double demandeGB = pidGB_.calculer(
        consigneGB,
        std::max(0.0, niveauGB),
        dt
    );

    const double demandePB = pidPB_.calculer(
        consignePB,
        std::max(0.0, niveauPB),
        dt
    );

    const double demandeMax = std::max(demandeGB, demandePB);

    if (pompeActive_ && demandeMax <= 4.0)
    {
        pompeActive_ = false;
    }
    else if (!pompeActive_ && demandeMax >= 10.0)
    {
        pompeActive_ = true;
    }

    if (!pompeActive_)
    {
        return {};
    }

    return {demandeGB, demandePB, true};
}
