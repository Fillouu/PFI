// SerialPort.cpp : termios, flock, et des lectures avec délai (poll).
#include "SerialPort.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <poll.h>
#include <sys/file.h>
#include <termios.h>
#include <unistd.h>

// Bornes pour les boucles
// au plus INTERRUPTIONS_MAX appels interrompus (EINTR, EAGAIN, rien d'écrit) par écriture,
static const int INTERRUPTIONS_MAX = 100;
// au plus PASSAGES_MAX tours par lecture; au-delà, la fonction rend une erreur.
static const int PASSAGES_MAX = 4096;

// millisecondes écoulées depuis debut
// (si l'horloge ne répond pas, le délai est considéré comme écoulé)
// voir TP4 TODO #3 dans Clock.cpp
static long msDepuis(const struct timespec& debut) {
    struct timespec maintenant;
    if (clock_gettime(CLOCK_MONOTONIC, &maintenant) != 0) {
        return LONG_MAX;
    }
    return (maintenant.tv_sec - debut.tv_sec) * 1000L
           + (maintenant.tv_nsec - debut.tv_nsec) / 1000000L;
}

static speed_t vitesse(int bauds) {
    switch (bauds) {
    case 1200: return B1200;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    default: return B0;
    }
}

SerialPort::SerialPort() {
    erreur[0] = '\0';
}

SerialPort::~SerialPort() {
    close();
}

void SerialPort::noterErreur(const char* quoi) {
    snprintf(erreur, sizeof erreur, "%s : %s", quoi, strerror(errno));
}

const char* SerialPort::lastError() const {
    return erreur;
}

bool SerialPort::isOpen() const {
    return fd >= 0;
}

bool SerialPort::open(const char* chemin, int bauds, char parite) {
    close();
    if (chemin == NULL) {
        snprintf(erreur, sizeof erreur, "open : aucun chemin de port");
        return false;
    }
    speed_t v = vitesse(bauds);
    if (v == B0) {
        snprintf(erreur, sizeof erreur, "vitesse non prise en charge : %d bauds", bauds);
        return false;
    }
    // On configure le port dans un descripteur de fichier (file descriptor) local. Il ne devient
    // le port de l'objet, le membre fd, qu'une fois tout réussi : en cas d'erreur, on le referme,
    // et fd reste à -1. noterErreur() d'abord, parce que close() peut changer errno.
    int descripteur = ::open(chemin, O_RDWR | O_NOCTTY);
    if (descripteur < 0) {
        noterErreur(chemin);   // ENOENT : pas de port; EACCES : pas les droits (sudo)
        return false;
    }
    if (flock(descripteur, LOCK_EX | LOCK_NB) != 0) {
        noterErreur("port déjà pris par un autre programme");
        (void) ::close(descripteur);
        return false;
    }
    struct termios t;
    if (tcgetattr(descripteur, &t) != 0) {
        noterErreur("tcgetattr");
        (void) ::close(descripteur);
        return false;
    }
    cfmakeraw(&t);
    if (cfsetispeed(&t, v) != 0 || cfsetospeed(&t, v) != 0) {
        noterErreur("cfsetispeed, cfsetospeed");
        (void) ::close(descripteur);
        return false;
    }
    t.c_cflag |= CLOCAL | CREAD;
    t.c_cflag &= ~static_cast<tcflag_t>(CSTOPB);
    t.c_cflag &= ~static_cast<tcflag_t>(PARENB | PARODD);
    if (parite == 'E') {
        t.c_cflag |= PARENB;
    } else if (parite == 'O') {
        t.c_cflag |= PARENB | PARODD;
    }
    t.c_cc[VMIN] = 0;              // read() ne bloque jamais : poll() attend à sa place
    t.c_cc[VTIME] = 0;
    if (tcsetattr(descripteur, TCSANOW, &t) != 0) {
        noterErreur("tcsetattr");
        (void) ::close(descripteur);
        return false;
    }
    if (tcflush(descripteur, TCIOFLUSH) != 0) {
        noterErreur("tcflush");
        (void) ::close(descripteur);
        return false;
    }
    fd = descripteur;
    erreur[0] = '\0';
    return true;
}

// À la fermeture, une erreur ne change rien : le port est relâché quand même.
void SerialPort::close() {
    if (fd >= 0) {
        (void) flock(fd, LOCK_UN);
        (void) ::close(fd);
        fd = -1;
    }
}

bool SerialPort::write(const void* donnees, size_t taille) {
    if (donnees == NULL && taille > 0) {
        snprintf(erreur, sizeof erreur, "write : aucune donnée");
        return false;
    }
    const unsigned char* octets = static_cast<const unsigned char*>(donnees);
    size_t envoyes = 0;
    int interruptions = 0;
    // au plus taille + INTERRUPTIONS_MAX tours : chacun écrit au moins un octet, ou compte une interruption
    while (envoyes < taille) {
        ssize_t n = ::write(fd, octets + envoyes, taille - envoyes);
        if (n < 0 && errno != EINTR && errno != EAGAIN) {
            noterErreur("écriture");
            return false;
        }
        if (n <= 0) {
            interruptions++;
            if (interruptions >= INTERRUPTIONS_MAX) {
                snprintf(erreur, sizeof erreur, "écriture : %d tentatives sans succès", INTERRUPTIONS_MAX);
                return false;
            }
            continue;
        }
        envoyes += static_cast<size_t>(n);
    }
    return true;
}

bool SerialPort::writeLine(const char* texte, char fin) {
    if (texte == NULL) {
        snprintf(erreur, sizeof erreur, "writeLine : aucun texte");
        return false;
    }
    if (!write(texte, strlen(texte))) {
        return false;
    }
    return write(&fin, 1);
}

int SerialPort::attendre(int delaiMs) {
    if (delaiMs < 0) {
        delaiMs = 0;
    }
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;
    p.revents = 0;
    int r = poll(&p, 1, delaiMs);
    if (r < 0) {
        if (errno == EINTR) {
            return 0;
        }
        noterErreur("poll");
        return -1;
    }
    if (r > 0 && (p.revents & (POLLERR | POLLHUP | POLLNVAL))) {
        snprintf(erreur, sizeof erreur, "le port s'est fermé (lien coupé ?)");
        return -1;
    }
    return r;
}

int SerialPort::readUntil(char* tampon, size_t taille, char fin, int delaiMs) {
    if (tampon == NULL || taille == 0) {
        snprintf(erreur, sizeof erreur, "readUntil : aucun tampon");
        return -1;
    }
    struct timespec debut;
    if (clock_gettime(CLOCK_MONOTONIC, &debut) != 0) {
        noterErreur("clock_gettime");
        tampon[0] = '\0';
        return -1;
    }
    size_t n = 0;
    for (int passage = 0; passage < PASSAGES_MAX; passage++) {
        int pret = attendre(delaiMs - static_cast<int>(msDepuis(debut)));
        if (pret < 0) {
            tampon[n] = '\0';
            return -1;
        }
        if (pret == 0) {
            tampon[n] = '\0';
            if (msDepuis(debut) >= delaiMs) {
                return 0;   // délai écoulé, terminateur jamais reçu
            }
            continue;
        }
        char c;
        ssize_t lu = ::read(fd, &c, 1);
        if (lu < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            noterErreur("lecture");
            tampon[n] = '\0';
            return -1;
        }
        if (lu == 0) {
            continue;
        }
        if (c == fin) {
            tampon[n] = '\0';
            return static_cast<int>(n);
        }
        if (n < taille - 1) {   // trop long : on garde le début, on lit jusqu'au terminateur
            tampon[n] = c;
            n = n + 1;
        }
    }
    tampon[n] = '\0';
    snprintf(erreur, sizeof erreur, "lecture : %d tours sans terminateur", PASSAGES_MAX);
    return -1;
}

int SerialPort::readUntilSilence(unsigned char* tampon, size_t taille, int delaiMs, int silenceMs) {
    if (tampon == NULL || taille == 0) {
        snprintf(erreur, sizeof erreur, "readUntilSilence : aucun tampon");
        return -1;
    }
    int pretAuDebut = attendre(delaiMs);
    if (pretAuDebut <= 0) {
        return pretAuDebut;   // 0 : rien reçu; -1 : erreur
    }
    size_t n = 0;
    for (int passage = 0; passage < PASSAGES_MAX && n < taille; passage++) {
        ssize_t lu = ::read(fd, tampon + n, taille - n);
        if (lu < 0 && errno != EINTR && errno != EAGAIN) {
            noterErreur("lecture");
            return -1;
        }
        if (lu > 0) {
            n += static_cast<size_t>(lu);
        }
        int pret = attendre(silenceMs);
        if (pret < 0) {
            return -1;
        }
        if (pret == 0) {
            break;   // silence : la trame est finie
        }
    }
    return static_cast<int>(n);
}

bool SerialPort::flushInput() {
    if (tcflush(fd, TCIFLUSH) != 0) {
        noterErreur("tcflush");
        return false;
    }
    return true;
}

int SerialPort::request(const char* trame, char* reponse, size_t taille, int delaiMs) {
    if (trame == NULL || reponse == NULL || taille == 0) {
        snprintf(erreur, sizeof erreur, "request : trame ou tampon absent");
        return -1;
    }
    verrou.take();
    int resultat = -1;
    if (flushInput() && writeLine(trame)) {
        resultat = readUntil(reponse, taille, '\r', delaiMs);
    }
    verrou.release();
    return resultat;
}
