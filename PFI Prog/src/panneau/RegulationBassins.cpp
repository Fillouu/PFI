
#include "panneau/RegulationBassins.h"

#include <algorithm>
#include <cmath>

RegulationBassins::RegulationBassins()
    : pidGB_(5.0, 0.7, 2.0),
      pidPB_(2.0, 0.18, 1.0)
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

    // DEBUT DU CODE MODIFIE AVEC L'AIDE D'UNE IA
    // Chaque bassin atteint sa consigne, puis coupe l'alimentation si
    // la purge est fermee. Avec une purge ouverte, le PID compense la perte.
    // -1 = etat de purge non connu; le PID fonctionne quand meme.
    auto demandeValve = [dt](Pid& pid, double consigne, double niveau,
                             int purge) -> double
    {
        // Consigne 0 : demande de fermeture.
        if (consigne <= 0.0 || niveau >= consigne + 0.5 ||
            (purge >= 0 && purge <= 1 && niveau >= consigne - 0.5))
        {
            pid.reinitialiser();
            return 0.0;
        }

        return pid.calculer(consigne, std::max(0.0, niveau), dt);
    };

    const double demandeGB = demandeValve(
        pidGB_, consigneGB, niveauGB, purgeGB);
    const double demandePB = demandeValve(
        pidPB_, consignePB, niveauPB, purgePB);
    // FIN DU CODE MODIFIE AVEC L'AIDE D'UNE IA

    const double demandeMax = std::max(demandeGB, demandePB);

    if (pompeActive_ && demandeMax <= 0.05)
    {
        pompeActive_ = false;
    }
    else if (!pompeActive_ && demandeMax >= 0.5)
    {
        pompeActive_ = true;
    }

    if (!pompeActive_)
    {
        return {};
    }

    return {demandeGB, demandePB, true};
}

// DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
bool RegulationBassins::reglerGainsGB(double kp, double ki, double kd)
{
    return pidGB_.definirGains(kp, ki, kd);
}

bool RegulationBassins::reglerGainsPB(double kp, double ki, double kd)
{
    return pidPB_.definirGains(kp, ki, kd);
}
// FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA
