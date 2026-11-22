
#include "panneau/Pid.h"
#include <algorithm>
#include <cmath>


Pid::Pid(double kp, double ki, double kd)
    : kp_(kp), ki_(ki), kd_(kd)
{
}

bool Pid::definirLimites(double minimum, double maximum)
{
    if (!std::isfinite(minimum) ||
        !std::isfinite(maximum) ||
        minimum >= maximum)
        return false;

    sortieMin_ = minimum;
    sortieMax_ = maximum;
    reinitialiser();
    return true;
}

void Pid::reinitialiser()
{
    integrale_ = 0.0;
    erreurPrecedente_ = 0.0;
    premierCalcul_ = true;
}

double Pid::calculer(double consigne, double mesure, double dt)
{
    if (!std::isfinite(consigne) ||
        !std::isfinite(mesure) ||
        !std::isfinite(dt) ||
        dt <= 0.0 ||
        !std::isfinite(kp_) ||
        !std::isfinite(ki_) ||
        !std::isfinite(kd_))
        return sortieMin_;

    const double erreur = consigne - mesure;
    const double proportionnel = kp_ * erreur;

    const double derive = premierCalcul_
        ? 0.0
        : kd_ * (erreur - erreurPrecedente_) / dt;

    const double integraleProposee = integrale_ + erreur * dt;

    const double sortieProposee =
        proportionnel + ki_ * integraleProposee + derive;

    // Anti-emballement de l'integrale
    if (std::isfinite(sortieProposee) &&
        !((sortieProposee > sortieMax_ && erreur > 0.0) ||
          (sortieProposee < sortieMin_ && erreur < 0.0)))
    {
        integrale_ = integraleProposee;
    }

    erreurPrecedente_ = erreur;
    premierCalcul_ = false;

    const double sortie =
        proportionnel + ki_ * integrale_ + derive;

    if (!std::isfinite(sortie))
        return sortieMin_;

    return std::clamp(sortie, sortieMin_, sortieMax_);
}

