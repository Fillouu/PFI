
#include "panneau/RegulationBassins.h"

#include <algorithm>
#include <cmath>

RegulationBassins::RegulationBassins()
    : pidGB_(5.0, 0.7, 3.0),
      pidPB_(2.0, 0.22, 2.0)
{
    pidGB_.definirLimites(0.0, 100.0);
    pidPB_.definirLimites(0.0, 100.0);
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
    double dt, bool autorise, int purgeGB, int purgePB)
{
    if (!autorise ||
        !std::isfinite(niveauGB) || !std::isfinite(niveauPB) ||
        !std::isfinite(consigneGB) || !std::isfinite(consignePB) ||
        !std::isfinite(dt) || dt <= 0.0 ||
        niveauGB < -2.0 || niveauPB < -2.0 ||
        niveauGB >= 100.0 || niveauPB >= 100.0 ||
        consigneGB < 0.0 || consigneGB > 100.0 ||
        consignePB < 0.0 || consignePB > 100.0)
    {
        reinitialiser();
        return {};
    }

    // La mesure de purge est facultative (enonce, p. 5).
    (void)purgeGB;
    (void)purgePB;

    auto demandeValve = [dt](Pid& pid, double consigne, double niveau) -> double
    {
        if (consigne <= 0.0)
        {
            pid.reinitialiser();
            return 0.0;
        }
        return pid.calculer(consigne, std::max(0.0, niveau), dt);
    };

    const double demandeGB = demandeValve(pidGB_, consigneGB, niveauGB);
    const double demandePB = demandeValve(pidPB_, consignePB, niveauPB);

    // L'autorisation de la pompe vient de l'operateur, pas de la sortie PID.
    // A utiliser sur un circuit hydraulique valide pour ce fonctionnement.
    pompeActive_ = true;
    return {demandeGB, demandePB, pompeActive_};
}

bool RegulationBassins::reglerGainsGB(double kp, double ki, double kd)
{
    return pidGB_.definirGains(kp, ki, kd);
}

bool RegulationBassins::reglerGainsPB(double kp, double ki, double kd)
{
    return pidPB_.definirGains(kp, ki, kd);
}
