
#pragma once

class Pid
{
public:
    Pid(double kp, double ki, double kd);

    // DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
    double obtenirKp() const { return kp_; }
    double obtenirKi() const { return ki_; }
    double obtenirKd() const { return kd_; }
    // FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA

    double calculer(double consigne, double mesure, double dt);
    void reinitialiser();
    bool definirLimites(double minimum, double maximum);
    bool definirGains(double kp, double ki, double kd);

private:
    double kp_;
    double ki_;
    double kd_;

    double integrale_ = 0.0;
    double mesurePrecedente_ = 0.0;
    bool premierCalcul_ = true;

    double sortieMin_ = 0.0;
    double sortieMax_ = 100.0;
};
