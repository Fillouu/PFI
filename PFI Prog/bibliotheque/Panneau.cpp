#include "Panneau.h"

#include <cstdio>
#include <cstdlib>

// ===== LES RÉGLAGES ======================================================================
// J'ai mis les valeurs du simulateur qui est un cas "idéal".
// Sur le panneau, mesurez et changez les réglages
static const int BAUDS = 115200;
static const int DELAI_REPONSE_MAX_MS = 200;

// Le sens de chaque valve
// true = fermée à 4 mA et ouverte à 20 mA; false = fermée à 20mA et ouverte à 4mA.
static const bool OUVRE_AU_COURANT_FORT[4] = {false, true, false, true};

static const double RELECTURE_4MA[4] = {4.0, 4.0, 4.0, 4.0};
static const double RELECTURE_20MA[4] = {20.0, 20.0, 20.0, 20.0};

// Les niveaux d'eau dans les deux bassins: niveau en % = (mA - zéro) / étendue x 100. Étendue = 20mA - 4mA
static const double ZERO_NIVEAU_GROS_MA = 4.0;
static const double ZERO_NIVEAU_PETIT_MA = 4.0;
static const double ETENDUE_NIVEAU_GROS_MA = 16.0;
static const double ETENDUE_NIVEAU_PETIT_MA = 16.0;

// Les températures
static const double TEMPERATURE_A_4MA_C = 0.0;
static const double TEMPERATURE_A_20MA_C = 100.0;
static const int ENTREE_TEMPERATURE_PETIT = 7;

// Les contacts de débordement : true = le bit vaut 1 tant qu'il n'y a pas de débordement.
static const bool LSHH_A_1_SANS_DEBORDEMENT = true;
// ===== FIN DES RÉGLAGES ==================================================================

static double borner(double valeur, double bas, double haut) {
    if (valeur < bas) {
        return bas;
    }
    if (valeur > haut) {
        return haut;
    }
    return valeur;
}

Panel::Panel() {
    erreur[0] = '\0';
}

const char* Panel::lastError() const {
    return erreur;
}

bool Panel::open(const char* cheminPort) {
    if (!port.open(cheminPort, BAUDS)) {
        snprintf(erreur, sizeof erreur, "%s", port.lastError());
        return false;
    }
    return true;
}

void Panel::close() {
    port.close();
}

double Panel::valveCurrent(Valve valve, double ouverturePourcent) const {
    int v = static_cast<int>(valve);
    double fraction = borner(ouverturePourcent, 0.0, 100.0) / 100.0;
    if (OUVRE_AU_COURANT_FORT[v]) {
        return 4.0 + 16.0 * fraction;
    }
    return 20.0 - 16.0 * fraction;
}

double Panel::valveFromReadback(Valve valve, double courantRelu) const {
    int v = static_cast<int>(valve);

    double ecart = RELECTURE_20MA[v] - RELECTURE_4MA[v];
    double courantEnvoye = 4.0;

    if (ecart != 0.0) {
        courantEnvoye = 4.0 + 16.0 * (courantRelu - RELECTURE_4MA[v]) / ecart;
    }

    double fraction = borner((courantEnvoye - 4.0) / 16.0, 0.0, 1.0);

    // Correction de la relecture des valves GB et PB
    if (valve == Valve::LargeTank) {
        return 100.0 * (1.0 - fraction);
    }

    if (valve == Valve::SmallTank) {
        return 100.0 * fraction;
    }

    if (OUVRE_AU_COURANT_FORT[v]) {
        return 100.0 * fraction;
    }
    return 100.0 * (1.0 - fraction);
}

bool Panel::sendCommand(const char* trame) {
    char reponse[32];
    int n = port.request(trame, reponse, sizeof reponse, DELAI_REPONSE_MAX_MS);
    if (n < 0) {
        snprintf(erreur, sizeof erreur, "%s : %s", trame, port.lastError());
        return false;
    }
    if (n == 0) {
        snprintf(erreur, sizeof erreur, "%s : aucune réponse en %d ms", trame, DELAI_REPONSE_MAX_MS);
        return false;
    }
    if (reponse[0] != '>') {
        snprintf(erreur, sizeof erreur, "%s : réponse refusée : %s", trame, reponse);
        return false;
    }
    return true;
}

const char* Panel::errorDetail(int n, const char* reponse) const {
    if (n < 0) {
        return port.lastError();
    }
    return reponse;
}

bool Panel::read(PanelReading& lecture) {
    char reponse[96];
    int n = port.request("#01", reponse, sizeof reponse, DELAI_REPONSE_MAX_MS);
    if (n <= 0 || reponse[0] != '>') {
        snprintf(erreur, sizeof erreur, "#01 : pas de réponse valide (%d) : %s", n,
                 errorDetail(n, reponse));
        return false;
    }

    // huit nombres signés à la suite : >+04.000+04.000...
    const char* p = reponse + 1;
    for (int i = 0; i < 8; i++) {
        char* fin = NULL;
        lecture.courant[i] = strtod(p, &fin);
        if (fin == p) {
            snprintf(erreur, sizeof erreur, "#01 : entrée AI-%d illisible : %s", i + 1, reponse);
            return false;
        }
        p = fin;
    }
    for (int v = 0; v < 4; v++) {
        lecture.ouvertureValve[v] = valveFromReadback(static_cast<Valve>(v), lecture.courant[v]);
    }
    lecture.niveauGrosBassin = 100.0 * (lecture.courant[4] - ZERO_NIVEAU_GROS_MA) / ETENDUE_NIVEAU_GROS_MA;
    lecture.niveauPetitBassin = 100.0 * (lecture.courant[5] - ZERO_NIVEAU_PETIT_MA) / ETENDUE_NIVEAU_PETIT_MA;
    double pente = (TEMPERATURE_A_20MA_C - TEMPERATURE_A_4MA_C) / 16.0;
    double t7 = TEMPERATURE_A_4MA_C + pente * (lecture.courant[6] - 4.0);
    double t8 = TEMPERATURE_A_4MA_C + pente * (lecture.courant[7] - 4.0);
    if (ENTREE_TEMPERATURE_PETIT == 7) {
        lecture.temperaturePetitBassin = t7;
        lecture.temperatureGrosBassin = t8;
    } else {
        lecture.temperaturePetitBassin = t8;
        lecture.temperatureGrosBassin = t7;
    }

    n = port.request("@05", reponse, sizeof reponse, DELAI_REPONSE_MAX_MS);
    if (n <= 0 || reponse[0] != '>') {
        snprintf(erreur, sizeof erreur, "@05 : pas de réponse valide (%d) : %s", n,
                 errorDetail(n, reponse));
        return false;
    }
    char* finDesChiffres = NULL;
    unsigned long bits = strtoul(reponse + 1, &finDesChiffres, 16);
    if (finDesChiffres == reponse + 1) {
        snprintf(erreur, sizeof erreur, "@05 : réponse sans valeur : %s", reponse);
        return false;
    }
    bool contactGros = (bits & 0x1) != 0;
    bool contactPetit = (bits & 0x2) != 0;
    if (LSHH_A_1_SANS_DEBORDEMENT) {
        lecture.debordementGrosBassin = !contactGros;
        lecture.debordementPetitBassin = !contactPetit;
    } else {
        lecture.debordementGrosBassin = contactGros;
        lecture.debordementPetitBassin = contactPetit;
    }
    return true;
}

bool Panel::setValve(Valve valve, double ouverturePourcent) {
    char trame[24];
    snprintf(trame, sizeof trame, "#02%d%+07.3f", static_cast<int>(valve),
             valveCurrent(valve, ouverturePourcent));
    return sendCommand(trame);
}

bool Panel::setDigitalOutput(int sortie, bool active) {
    char trame[16];
    int valeur = 0;
    if (active) {
        valeur = 1;
    }
    snprintf(trame, sizeof trame, "#041%d%02d", sortie, valeur);
    return sendCommand(trame);
}

bool Panel::setPump(bool enMarche) {
    return setDigitalOutput(0, enMarche);
}

// !! l'eau chaude est sur la sortie 3 (DO-4), pas sur DO-2 comme le dit le schéma !!
bool Panel::setHotWater(bool ouverte) {
    return setDigitalOutput(3, ouverte);
}

bool Panel::setColdWater(bool ouverte) {
    return setDigitalOutput(2, ouverte);
}

bool Panel::readModuleName(int adresse, char* nom, size_t taille) {
    if (nom == NULL || taille == 0) {
        snprintf(erreur, sizeof erreur, "readModuleName : aucun tampon pour le nom");
        return false;
    }
    char trame[8];
    char reponse[32];
    snprintf(trame, sizeof trame, "$%02dM", adresse);
    int n = port.request(trame, reponse, sizeof reponse, DELAI_REPONSE_MAX_MS);
    if (n < 3 || reponse[0] != '!') {
        snprintf(erreur, sizeof erreur, "%s : pas de réponse valide", trame);
        return false;
    }
    snprintf(nom, taille, "%s", reponse + 3);   // !01 suivi du nom
    return true;
}
