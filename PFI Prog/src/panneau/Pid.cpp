
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
    mesurePrecedente_ = 0.0;
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

    // Derivee sur la mesure : pas de pic lorsque la consigne change.
    const double derive = premierCalcul_
        ? 0.0
        : -kd_ * (mesure - mesurePrecedente_) / dt;

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

    mesurePrecedente_ = mesure;
    premierCalcul_ = false;

    const double sortie =
        proportionnel + ki_ * integrale_ + derive;

    if (!std::isfinite(sortie))
        return sortieMin_;

    if (sortie < sortieMin_)
    {
        return sortieMin_;
    }

    if (sortie > sortieMax_)
    {
        return sortieMax_;
    }

    return sortie;
}

// DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
bool Pid::definirGains(double kp, double ki, double kd)
{
    if (!std::isfinite(kp) || !std::isfinite(ki) || !std::isfinite(kd) ||
        kp < 0.0 || ki < 0.0 || kd < 0.0 ||
        kp > 100.0 || ki > 100.0 || kd > 100.0)
        return false;

    kp_ = kp;
    ki_ = ki;
    kd_ = kd;
    // Comme au Labo 5, conserver l'integrale quand les gains changent.
    return true;
}
