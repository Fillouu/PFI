
#pragma once

class Pid
{
public:
    Pid(double kp, double ki, double kd);

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
