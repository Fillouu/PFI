// Panneau.h : parler au panneau RAM ou au simulateur.
//
// LES TRAMES DCON
// Le Pi parle aux modules du panneau par un port série, en texte ASCII : le protocole DCON
// des modules ICP DAS. Une trame, c'est une commande en texte. Exemple : #020+12.000
//   #          le caractère de commande : #, $ ou @
//   02         l'adresse du module, en deux chiffres
//   0+12.000   les données : ici, le canal (channel) 0, à 12 mA
//   \r         le retour de chariot, fin du message
//
// Le premier caractère de la réponse dit ce qui s'est passé :
//   >          la commande est faite
//   !          la réponse à une commande $
//   ?          la commande est refusée (format invalide, canal qui n'existe pas)
//   rien       le module est absent, ou la trame n'avait pas son \r
//
// Les modules du panneau : (voir annexe III : La configuration du panneau)
//   01 : 8 entrées 4-20 mA      #01            -> >+dd.ddd x 8
//   02 : 4 sorties 4-20 mA      #02N+dd.ddd
//   04 : sorties numériques     #041c0v
//   05 : entrées numériques     @05
//   tous : le nom du module     $AAM
//
// Deux exemples :
//   #021+07.200   ouvrir la valve du gros bassin à 20 % : module 02, canal 1, 7,2 mA
//                 (4 mA + 20 % de 16 mA)
//                 réponse : >
//   #041001       démarrer la pompe : module 04, sortie 0, valeur 01
//                 réponse : >
// Avec Panel, panneau.setValve(Valve::LargeTank, 20.0); puis panneau.setPump(true);
//
// LE SIMULATEUR
//   Sur le PC : SimulateurBassins.exe (il écoute sur le port TCP 5100).
//   Sur le Pi : socat pty,link=/tmp/ttyRAM,raw,echo=0 tcp:<adresse du PC>:5100 sert à créer un faux port série vers TCP
//   Ensuite, dans le code : panneau.open("/tmp/ttyRAM")
//   Les réglages du haut de Panneau.cpp sont ceux du simulateur.
//
// LE PANNEAU RAM
//   Dans le code : panneau.open("/dev/ttyUSB0")   (il faut faire sudo pour utiliser le port série)
//   Au banc, mesurez chaque réglage du haut de Panneau.cpp et changez-le :
//   - le sens de chaque valve : envoyer 4 mA, puis 20 mA, et regarder la tige de la valve bouger;
//   - les niveaux : bassins vides (le zéro), puis à la ligne noire (100 %) et jusqu'à déclencher l'alarme (tenez-vous près du bouton d'arrêt d'urgence);
//   - la température du petit bassin, l'eau chaude l'eau froide;
//
// Toutes les fonctions sur le port série sont sûres entre plusieurs fils d'exécution (threads).
// SerialPort::request est protégé par un mutex.
//
// Exemple :
//     Panel panneau;
//     if (!panneau.open("/tmp/ttyRAM")) { printf("%s\n", panneau.lastError()); return 1; }
//     bool ok = panneau.setValve(Valve::LargeTank, 50.0);   // la valve d'abord...
//     if (ok) {
//         ok = panneau.setPump(true);                        // ... puis la pompe
//     }
//     if (!ok) { printf("%s\n", panneau.lastError()); }
//     PanelReading lecture;
//     if (panneau.read(lecture)) { printf("%.1f %%\n", lecture.niveauGrosBassin); }
#pragma once

#include <cstddef>
#include "SerialPort.h"

// Les quatre valves proportionnelles
enum class Valve {
    HotWater = 0,
    LargeTank = 1,
    SmallTank = 2,
    ColdWater = 3
};

struct PanelReading {
    double ouvertureValve[4];      // %
    double niveauGrosBassin;       // %
    double niveauPetitBassin;      // %
    double temperatureGrosBassin;  // °C
    double temperaturePetitBassin; // °C
    bool debordementGrosBassin;
    bool debordementPetitBassin;
    double courant[8];             // mA
};

class Panel {
public:
    Panel();

    bool open(const char* cheminPort);
    void close();
    const char* lastError() const;

    bool read(PanelReading& lecture);

    // Sorties analogiques : ouverture en %, de 0 (fermée) à 100 (grande ouverte).
    // Le sens de chaque valve (ouverte à 4 ou à 20 mA) est à ajuster dans Panneau.cpp.
    bool setValve(Valve valve, double ouverturePourcent);

    // Sorties numériques du module 04 : 0 la pompe, 2 l'électrovalve d'eau froide, 3 celle d'eau chaude
    bool setPump(bool enMarche);
    bool setHotWater(bool ouverte);
    bool setColdWater(bool ouverte);

    // Le nom d'un module ($AAM -> !AAnom). Pour vérifier le branchement.
    bool readModuleName(int adresse, char* nom, size_t taille);

    // Les conversions, publiques pour les tests et l'affichage.
    double valveCurrent(Valve valve, double ouverturePourcent) const;   // % -> mA envoyés
    double valveFromReadback(Valve valve, double courantRelu) const;    // mA relus -> %

private:
    bool sendCommand(const char* trame);
    bool setDigitalOutput(int sortie, bool active);
    const char* errorDetail(int n, const char* reponse) const;

    SerialPort port;
    char erreur[160];
};
