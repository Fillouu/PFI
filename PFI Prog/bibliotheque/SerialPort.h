// Deux façons de s'en servir :
//  - question-réponse (les modules du panneau RAM, Modbus) : request() envoie une trame
//    et attend la réponse, sous un mutex. Plusieurs tâches peuvent partager le port.
//  - réception continue (le cas de la balance) : une tâche appelle readUntil() en boucle.
//
// À l'ouverture, le port est verrouillé avec flock. Sous Linux, tout est un fichier : le port
// série aussi (/dev/ttyUSB0). flock le réserve à un seul programme à la fois, comme un mutex
// entre programmes; un deuxième programme reçoit une erreur au lieu d'attendre.
// Attention : seuls les programmes qui appellent flock le respectent (un cat ou un echo sur
// le port passe quand même).
//
// Exemple, le panneau RAM :
//     SerialPort port;
//     if (!port.open("/dev/ttyUSB0", 115200)) { ... port.lastError() ... }
//     char reponse[80];
//     int n = port.request("#01", reponse, sizeof reponse, 100);
//     // n > 0 : reponse contient ">+04.000+12.000...", sans le '\r'
#pragma once

#include <cstddef>
#include "Mutex.h"

class SerialPort {
public:
    SerialPort();
    ~SerialPort();   // ferme le port s'il est ouvert

    // Un port ne se copie pas : deux objets fermeraient le même descripteur.
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    // chemin : "/dev/ttyUSB0" sur le panneau, "/tmp/ttyRAM" vers le simulateur (socat).
    // bauds  : 1200 à 230400. parite : 'N', 'E' (paire) ou 'O' (impaire). 8 bits, 1 arrêt.
    // false si le port n'existe pas, est refusé (groupe dialout) ou déjà pris.
    bool open(const char* chemin, int bauds, char parite = 'N');
    void close();
    bool isOpen() const;
    const char* lastError() const;   // la raison du dernier échec

    // Écrit tous les octets. false si l'écriture échoue.
    bool write(const void* donnees, size_t taille);

    // Écrit le texte, puis le terminateur ('\r' par défaut).
    bool writeLine(const char* texte, char fin = '\r');

    // Lit jusqu'au terminateur, sans le garder; le tampon finit par '\0'.
    // Retourne le nombre de caractères, 0 si le délai est écoulé, -1 si erreur.
    int readUntil(char* tampon, size_t taille, char fin, int delaiMs);

    // Lit des octets jusqu'à un silence (Modbus RTU : la trame finit par un silence).
    // Retourne le nombre d'octets, 0 si rien n'arrive dans delaiMs, -1 si erreur.
    int readUntilSilence(unsigned char* tampon, size_t taille, int delaiMs, int silenceMs);

    // Jette ce qui attend en réception (une vieille réponse arrivée trop tard). false si erreur.
    bool flushInput();

    // Envoie une trame terminée par '\r' et lit la réponse jusqu'au '\r', sous le mutex.
    // Retourne comme readUntil : le nombre de caractères, 0 si pas de réponse, -1 si erreur.
    int request(const char* trame, char* reponse, size_t taille, int delaiMs);

private:
    int attendre(int delaiMs);   // 1 si un octet attend, 0 si délai écoulé, -1 si erreur
    void noterErreur(const char* quoi);

    int fd = -1;
    Mutex verrou;
    char erreur[128];
};
