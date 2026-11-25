// DEBUT DU FICHIER CONTENANT DES MODIFICATIONS AVEC L'AIDE D'UNE IA
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <csignal>
#include <poll.h>
#include <cctype>
#include <sstream>
#include <unistd.h>
#include <termios.h>
#include <iomanip>
#include <array>
#include <sys/ioctl.h>
#include <mutex>
#include <streambuf>
#include <deque>

#include "panneau/ControlePanneau.h"
#include "panneau/GestionAlarmes.h"
#include "panneau/InterfacePanneau.h"
#include "panneau/RegulationBassins.h"
#include "commun/ClientMqtt.h"
#include "commun/TopicsMqtt.h"


template <typename T>
T limiter(T valeur, T minimum, T maximum)
{
    if (valeur < minimum)
        return minimum;

    if (valeur > maximum)
        return maximum;

    return valeur;
}

// -1 : aucune commande
//  0 : manuel
//  1 : automatique
std::atomic<int> demandeMode{-1};
std::atomic<int> demandeConsigneGB{-1};
std::atomic<int> demandeConsignePB{-1};

std::atomic<int> demandeValveManuelleGB{-1};
std::atomic<int> demandeValveManuellePB{-1};
std::atomic<int> demandePompeManuelle{-1};
std::atomic<int> demandeSeuilAlarmeGB{-1};
std::atomic<int> demandeSeuilAlarmePB{-1};

// DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
const char* topicsAlarmes[] = {"RAM/ALARMES/ETATS/ALR_GB_OVF", "RAM/ALARMES/ETATS/ALR_PB_OVF", "RAM/ALARMES/ETATS/ALR_GB_NIV_MAX", "RAM/ALARMES/ETATS/ALR_PB_NIV_MAX"};
const char* topicsTemps[] = {"RAM/ALARMES/CMD/TGNIVGB", "RAM/ALARMES/CMD/TGNIVPB", "RAM/ALARMES/CMD/TRNIVGB", "RAM/ALARMES/CMD/TRNIVPB"};
std::atomic<bool> ackAlarmes[4]{};
std::atomic<int> tempsAlarmes[4]{{3},{3},{10},{10}};
// FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA
// Un affichage au lancement, puis seulement avec la commande etat.
std::atomic<bool> afficherEtatDemande{true};
std::atomic<bool> demandeRearmement{false};


volatile std::sig_atomic_t demandeArretUrgence = 0;



void signalArretUrgence(int)
{
    demandeArretUrgence = 1;
}


bool convertirConsigne(const std::string& texte, int& valeur)
{
    if (texte.empty() || texte.size() > 3)
    {
        return false;
    }

    int resultat = 0;

    for (char caractere : texte)
    {
        if (caractere < '0' || caractere > '9')
        {
            return false;
        }

        resultat = resultat * 10 + (caractere - '0');
    }

    if (resultat > 100)
    {
        return false;
    }

    valeur = resultat;
    return true;
}



void traiterCommandeMqtt(
    const std::string& topic,
    const std::string& message)
{
    // DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
    for (int i = 0; i < 4; ++i)
    {
        if (topic == topicsAlarmes[i] && message == "ACK") { ackAlarmes[i].store(true); return; }
        if (topic == topicsTemps[i]) {
            if (!message.empty() && message.size() <= 6 && message.find_first_not_of("0123456789") == std::string::npos)
                tempsAlarmes[i].store(std::stoi(message));
            return;
        }
    }
    // FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA
    if (topic == TopicsMqtt::CMD_MODE)
    {
        if (message == "manuel")
        {
            demandeMode.store(0);
            std::cout << "\nCommande recue : manuel"
                      << std::endl;
        }
        else if (message == "auto")
        {
            demandeMode.store(1);
            std::cout << "\nCommande recue : auto"
                      << std::endl;
        }
        else
        {
            std::cerr << "\nMode MQTT invalide : "
                      << message << std::endl;
        }

        return;
    }

    int valeur = 0;

    if (topic == "RAM/ALARMES/CMD/NIVLHGB" ||
        topic == "RAM/ALARMES/CMD/NIVLHPB")
    {
        if (convertirConsigne(message, valeur) && valeur > 0)
        {
            if (topic == "RAM/ALARMES/CMD/NIVLHGB")
                demandeSeuilAlarmeGB.store(valeur);
            else
                demandeSeuilAlarmePB.store(valeur);
        }
        return;
    }

    if (topic == TopicsMqtt::CMD_NIV_GB)
    {
        if (convertirConsigne(message, valeur))
        {
            demandeConsigneGB.store(valeur);
        }
        else
        {
            std::cerr << "\nConsigne GB invalide"
                      << std::endl;
        }
    }
    else if (topic == TopicsMqtt::CMD_NIV_PB)
    {
        if (convertirConsigne(message, valeur))
        {
            demandeConsignePB.store(valeur);
        }
        else
        {
            std::cerr << "\nConsigne PB invalide"
                      << std::endl;
        }
    }

    if (topic == TopicsMqtt::CMD_VALVE_GB)
    {
        if (convertirConsigne(message, valeur))
        {
            demandeValveManuelleGB.store(valeur);
            std::cout << "\nCommande valve GB : "
                      << valeur << " %" << std::endl;
        }
        else
        {
            std::cerr << "\nCommande manuelle GB invalide"
                      << std::endl;
        }
    }
    else if (topic == TopicsMqtt::CMD_VALVE_PB)
    {
        if (convertirConsigne(message, valeur))
        {
            demandeValveManuellePB.store(valeur);
            std::cout << "\nCommande valve PB : "
                      << valeur << " %" << std::endl;
        }
        else
        {
            std::cerr << "\nCommande manuelle PB invalide"
                      << std::endl;
        }
    }
    else if (topic == TopicsMqtt::CMD_POMPE)
    {
        if (message == "on" || message == "1")
        {
            demandePompeManuelle.store(1);
        }
        else if (message == "off" || message == "0")
        {
            demandePompeManuelle.store(0);
        }
        else
        {
            std::cerr << "\nCommande pompe invalide"
                      << std::endl;
        }
    }
}


namespace
{
    volatile std::sig_atomic_t finProgrammeDemandee = 0;

    void demanderFinProgramme(int)
    {
        finProgrammeDemandee = 1;
    }
}




// La console partage le meme terminal que la regulation PID.
// Les commandes clavier sont traitees par la boucle principale.
void afficherAideConsole()
{
    std::cout << "\nCommandes (ligne puis Entree) :\n"
              << "  manuel | auto  (ou mode manuel | mode auto)\n"
              << "  valve gb 50 | valve pb 0         (manuel, 0-100 %)\n"
              << "  pompe on | pompe off             (manuel et auto)\n"
              << "  alarme gb 80 | alarme pb 80      (seuil 1-100 %, defaut 100)\n"
              << "  consigne gb 50 | consigne pb 75  (auto, 0-100 %)\n"
              << "  gains gb 4 0.03 0 | gains pb 2 0.02 0\n"
              << "  etat | rearme | aide | arret | quitter\n\n";
}

void traiterLigneConsole(const std::string& ligne, RegulationBassins& regulation)
{
    std::istringstream iss(ligne);
    std::string action, cible, valeur, surplus;
    if (!(iss >> action))
        return;
    std::transform(action.begin(), action.end(), action.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (action == "aide" || action == "help")
    {
        afficherAideConsole();
        return;
    }
    if (action == "quitter" || action == "q")
    {
        finProgrammeDemandee = 1;
        return;
    }
    if (action == "arret")
    {
        demandeArretUrgence = 1;
        std::cout << "Arret logiciel demande, redemarrer pour reprendre.\n";
        return;
    }
    if (action == "rearme")
    {
        demandeRearmement.store(true);
        return;
    }
    if (action == "etat")
    {
        afficherEtatDemande.store(true);
        return;
    }
    if (action == "auto" || action == "manuel")
    {
        if (iss >> surplus)
            std::cerr << "Commande invalide.\n";
        else
            demandeMode.store(action == "auto" ? 1 : 0);
        return;
    }
    // Accepter aussi "mode auto" et "mode manuel".
    if (action == "mode")
    {
        if (!(iss >> cible) || (iss >> surplus) ||
            (cible != "auto" && cible != "manuel"))
            std::cerr << "Syntaxe : mode manuel | mode auto\n";
        else
            demandeMode.store(cible == "auto" ? 1 : 0);
        return;
    }
    if (action == "pompe")
    {
        if (!(iss >> cible) || (iss >> surplus) ||
            (cible != "on" && cible != "off"))
            std::cerr << "Syntaxe : pompe on | pompe off\n";
        else
            demandePompeManuelle.store(cible == "on" ? 1 : 0);
        return;
    }
    if (!(iss >> cible >> valeur))
    {
        std::cerr << "Commande incomplete; saisir aide.\n";
        return;
    }
    std::transform(cible.begin(), cible.end(), cible.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (action == "gains" && (cible == "gb" || cible == "pb"))
    {
        double kp, ki, kd;
        std::istringstream kpStream(valeur);
        if (!(kpStream >> kp) || (kpStream >> surplus) ||
            !(iss >> ki >> kd) || (iss >> surplus))
        {
            std::cerr << "Syntaxe : gains gb 4 0.03 0\n";
            return;
        }
        const bool valide = cible == "gb"
            ? regulation.reglerGainsGB(kp, ki, kd)
            : regulation.reglerGainsPB(kp, ki, kd);
        std::cout << (valide ? "Gains appliques.\n"
                             : "Gains invalides (0 a 100).\n");
        return;
    }
    if (iss >> surplus)
    {
        std::cerr << "Trop d'arguments.\n";
        return;
    }
    int nombre = 0;
    if (!convertirConsigne(valeur, nombre))
    {
        std::cerr << "Pourcentage entier invalide (0 a 100).\n";
        return;
    }
    if (action == "alarme" && (cible == "gb" || cible == "pb"))
    {
        if (nombre == 0)
            std::cerr << "Seuil d'alarme : minimum 1 %, maximum 100 %.\n";
        else if (cible == "gb")
            demandeSeuilAlarmeGB.store(nombre);
        else
            demandeSeuilAlarmePB.store(nombre);
    }
    else if (action == "valve" && cible == "gb")
        demandeValveManuelleGB.store(nombre);
    else if (action == "valve" && cible == "pb")
        demandeValveManuellePB.store(nombre);
    else if (action == "consigne" && cible == "gb")
        demandeConsigneGB.store(nombre);
    else if (action == "consigne" && cible == "pb")
        demandeConsignePB.store(nombre);
    else
        std::cerr << "Commande inconnue; saisir aide.\n";
}

// Interface terminal : une grille fixe, redessinee uniquement si une valeur change.
// Les messages ne font pas defiler l'ecran et le clavier est lu frequemment.
class JournalTerminal : public std::streambuf
{
public:
    std::string dernierMessage() const
    {
        std::lock_guard<std::mutex> garde(mutex_);
        return message_;
    }
protected:
    int overflow(int caractere) override
    {
        if (caractere == traits_type::eof()) return traits_type::not_eof(caractere);
        std::lock_guard<std::mutex> garde(mutex_);
        if (caractere == '\n')
        {
            if (!ligne_.empty()) message_ = ligne_;
            ligne_.clear();
        }
        else if (caractere != '\r' && ligne_.size() < 220)
            ligne_.push_back(static_cast<char>(caractere));
        return caractere;
    }
    std::streamsize xsputn(const char* texte, std::streamsize taille) override
    {
        for (std::streamsize i = 0; i < taille; ++i) overflow(texte[i]);
        return taille;
    }
private:
    mutable std::mutex mutex_;
    std::string ligne_;
    std::string message_ = "Pret.";
};

class ConsoleInteractive
{
public:
    ConsoleInteractive()
    {
        actif_ = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
        if (actif_ && tcgetattr(STDIN_FILENO, &ancien_) == 0)
        {
            termios nouveau = ancien_;
            nouveau.c_lflag &= ~(ICANON | ECHO);
            nouveau.c_cc[VMIN] = 0;
            nouveau.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &nouveau) == 0)
            {
                restaure_ = true;
                envoyer("\033[?1049h\033[2J\033[H\033[?25h");
                ancienCout_ = std::cout.rdbuf(&journal_);
                ancienCerr_ = std::cerr.rdbuf(&journal_);
                filClavier_ = std::thread([this] { boucleClavier(); });
            }
        }
        if (!restaure_) actif_ = false;
    }

    ~ConsoleInteractive()
    {
        if (restaure_)
        {
            finClavier_.store(true);
            if (filClavier_.joinable()) filClavier_.join();
            std::cout.rdbuf(ancienCout_);
            std::cerr.rdbuf(ancienCerr_);
            tcsetattr(STDIN_FILENO, TCSANOW, &ancien_);
            envoyer("\033[?25h\033[?1049l\033[0m");
        }
    }

    void lire(RegulationBassins& regulation)
    {
        std::deque<std::string> commandes;
        {
            std::lock_guard<std::mutex> garde(ecranMutex_);
            commandes.swap(commandes_);
        }
        for (const auto& commande : commandes)
            traiterLigneConsole(commande, regulation);
    }

    void afficher(const PanelReading& lecture,
                  const ControlePanneau& controle,
                  double pidGB, double pidPB, bool pompe,
                  bool alarmeGB, bool alarmePB,
                  bool alarmeVerrouillee,
                  int seuilGB, int seuilPB)
    {
        if (!actif_) return;
        std::lock_guard<std::mutex> garde(ecranMutex_);
        struct winsize ws{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) return;
        if (ws.ws_col < 62 || ws.ws_row < 23)
        {
            if (!tailleInvalide_)
                envoyer("\033[2J\033[HTerminal trop petit : minimum 62 colonnes et 23 lignes.");
            tailleInvalide_ = true;
            return;
        }
        if (tailleInvalide_)
        {
            lignes_.fill("");
            envoyer("\033[2J\033[H");
            tailleInvalide_ = false;
        }
        auto valeurs = [](const std::string& titre, double gb, double pb)
        {
            std::ostringstream v;
            v << std::fixed << std::setprecision(2) << std::left << std::setw(22)
              << titre << std::right << std::setw(10) << gb << "     "
              << std::setw(10) << pb;
            return v.str();
        };
        std::ostringstream temperature;
        temperature << std::fixed << std::setprecision(2)
                    << "Temperature PB : " << lecture.temperaturePetitBassin
                    << " C   Courant PB : " << lecture.courant[2] << " mA";
        dessinerLigne(1,  "========== PFI - PANNEAU RAM (PI / REEL) ==========");
        dessinerLigne(2,  "Mode : " + controle.obtenirMode() +
                         "        Pompe : " + (pompe ? "ON" : "OFF"));
        dessinerLigne(3,  "---------------------------------------------------");
        dessinerLigne(4,  "                              GB             PB");
        dessinerLigne(5,  valeurs("Niveau (%)", lecture.niveauGrosBassin, lecture.niveauPetitBassin));
        dessinerLigne(6,  valeurs("Consigne (%)", controle.obtenirConsigneGB(), controle.obtenirConsignePB()));
        dessinerLigne(7,  valeurs("Valve mesuree (%)", lecture.ouvertureValve[1], lecture.ouvertureValve[2]));
        dessinerLigne(8,  valeurs("Sortie PID (%)", pidGB, pidPB));
        dessinerLigne(20, valeurs("Seuil alarme (%)", seuilGB, seuilPB));
        dessinerLigne(9,  std::string("Tige debordement : GB ") +
                          (lecture.debordementGrosBassin ? "OUI" : "NON") + "   PB " +
                          (lecture.debordementPetitBassin ? "OUI" : "NON"));
        dessinerLigne(10, temperature.str());
        dessinerLigne(11, "---------------------------------------------------");
        dessinerLigne(12, std::string("Securite : ") +
                          (alarmeVerrouillee ? "ALARME - sorties fermees" : "OK"));
        dessinerLigne(13, std::string("Bassin en alarme : ") +
                          (alarmeGB ? "GB " : "") + (alarmePB ? "PB" : ""));
        dessinerLigne(14, "---------------------------------------------------");
        dessinerLigne(15, "manuel | auto | valve gb 10 | alarme gb 80 | aide");
        dessinerLigne(16, "consigne pb 50 | pompe on/off | rearme | quitter");
        dessinerLigne(17, "Commande : " + saisie_);
        dessinerLigne(18, "Entree : valider | Retour arriere : effacer");
        dessinerLigne(19, "Message : " + journal_.dernierMessage());
        placerCurseur();
    }

private:
    void boucleClavier()
    {
        while (!finClavier_.load())
        {
            pollfd fd{};
            fd.fd = STDIN_FILENO;
            fd.events = POLLIN;
            if (poll(&fd, 1, 30) <= 0 || !(fd.revents & POLLIN)) continue;
            char c = 0;
            if (::read(STDIN_FILENO, &c, 1) != 1) continue;
            std::lock_guard<std::mutex> garde(ecranMutex_);
            if (etatEchap_ != 0)
            {
                if (etatEchap_ == 1 && c == '[') { etatEchap_ = 2; continue; }
                if (etatEchap_ == 2 && c >= '0' && c <= '9') { etatEchap_ = 3; continue; }
                if (etatEchap_ == 3 && c != '~') continue;
                etatEchap_ = 0;
                continue;
            }
            if (c == '\033') { etatEchap_ = 1; continue; }
            if (c == '\r' || c == '\n')
            {
                if (!saisie_.empty()) commandes_.push_back(saisie_);
                saisie_.clear();
            }
            else if (c == 127 || c == 8)
            {
                if (!saisie_.empty()) saisie_.pop_back();
            }
            else if (c == 3)
                finProgrammeDemandee = 1;
            else if (static_cast<unsigned char>(c) >= 32 && saisie_.size() < 40)
                saisie_.push_back(c);
            dessinerLigne(17, "Commande : " + saisie_);
            placerCurseur();
        }
    }
    static void envoyer(const std::string& valeur)
    {
        const char* ptr = valeur.data();
        std::size_t reste = valeur.size();
        while (reste > 0)
        {
            const ssize_t n = ::write(STDOUT_FILENO, ptr, reste);
            if (n <= 0) return;
            ptr += n;
            reste -= static_cast<std::size_t>(n);
        }
    }
    void dessinerLigne(int numero, const std::string& texte)
    {
        if (lignes_[numero] == texte) return;
        lignes_[numero] = texte;
        envoyer("\033[" + std::to_string(numero) + ";1H\033[2K" + texte);
    }
    void placerCurseur()
    {
        envoyer("\033[17;" + std::to_string(12 + saisie_.size()) + "H");
    }
    bool actif_ = false;
    bool restaure_ = false;
    bool tailleInvalide_ = false;
    int etatEchap_ = 0;
    termios ancien_{};
    std::streambuf* ancienCout_ = nullptr;
    std::streambuf* ancienCerr_ = nullptr;
    JournalTerminal journal_;
    std::mutex ecranMutex_;
    std::atomic<bool> finClavier_{false};
    std::thread filClavier_;
    std::deque<std::string> commandes_;
    std::array<std::string, 22> lignes_{};
    std::string saisie_;
};

int main(int argc, char* argv[])
{
    std::cout << "PFI - Panneau RAM" << std::endl;

    std::signal(SIGUSR1, signalArretUrgence);
    std::signal(SIGINT, demanderFinProgramme);
    std::signal(SIGTERM, demanderFinProgramme);

    if (argc < 3)
    {
        std::cerr << "Utilisation : "
                  << argv[0]
                  << " <port serie> <adresse broker MQTT>"
                  << std::endl;
        return 1;
    }

    InterfacePanneau panneau;

    // Connexion au panneau
    if (!panneau.connecter(argv[1]))
    {
        std::cerr << "Erreur de connexion : "
                  << panneau.obtenirErreur()
                  << std::endl;
        return 1;
    }

    std::cout << "Connexion ouverte." << std::endl;

    // Fermer les actionneurs meme si le broker est ensuite inaccessible.
    if (!panneau.mettreEnSecuriteSimulateur())
    {
        std::cerr << "Etat initial non securise : "
                  << panneau.obtenirErreur() << std::endl;
        panneau.deconnecter();
        return 1;
    }

    ClientMqtt mqtt;

    mqtt.definirGestionnaire(traiterCommandeMqtt);
    if (!mqtt.connecter(argv[2]))
    {
        std::cerr << "Impossible de lancer MQTT."
                << std::endl;
        return 1;
    }


    if (!mqtt.abonner(TopicsMqtt::CMD_MODE))
    {
        std::cerr << "Erreur abonnement MQTT au mode."
                << std::endl;

        mqtt.deconnecter();
        panneau.deconnecter();
        return 1;
    }

    std::cout << "Abonnement MQTT au mode demande." << std::endl;

    // Abonnement aux consignes des deux bassins
    if (!mqtt.abonner(TopicsMqtt::CMD_NIV_GB) ||
        !mqtt.abonner(TopicsMqtt::CMD_NIV_PB))
    {
        std::cerr << "Erreur abonnement consignes MQTT"
                  << std::endl;

        mqtt.deconnecter();
        panneau.deconnecter();
        return 1;
    }

    std::cout << "Abonnements aux consignes demandes."<< std::endl;

    if (!mqtt.abonner(TopicsMqtt::CMD_VALVE_GB) ||
        !mqtt.abonner(TopicsMqtt::CMD_VALVE_PB) ||
        !mqtt.abonner(TopicsMqtt::CMD_POMPE))
    {
        std::cerr << "Erreur abonnement commandes manuelles"
                  << std::endl;

        panneau.mettreEnSecuriteSimulateur();
        mqtt.deconnecter();
        panneau.deconnecter();
        return 1;
    }

    std::cout << "Abonnements manuels demandes."
              << std::endl;

    // DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
    for (int i = 0; i < 4; ++i)
        if (!mqtt.abonner(topicsAlarmes[i]) || !mqtt.abonner(topicsTemps[i])) {
            std::cerr << "Abonnement alarmes impossible.\n";
            return 1;
        }
    GestionAlarme alarmes[4];
    // FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA
    if (!mqtt.abonner("RAM/ALARMES/CMD/NIVLHGB") ||
        !mqtt.abonner("RAM/ALARMES/CMD/NIVLHPB"))
        std::cerr << "Abonnement seuils d'alarme incomplet.\n";

    ControlePanneau controle;

    RegulationBassins regulation;

    bool sortiesAutoActives = false;

    double valveManuelleGB = 0.0;
    double valveManuellePB = 0.0;
    bool pompeManuelle = false;
    bool pompeAutoriseeAuto = false;
    int seuilAlarmeGB = 100;
    int seuilAlarmePB = 100;
    bool commandesManuellesActives = false;

    // Le pilote Panel prend en charge le simulateur et le panneau reel.
    // La securite de depart a ete appliquee avant la connexion MQTT.
    bool pompeAppliquee = false;
    double derniereCommandePidGB = 0.0;
    double derniereCommandePidPB = 0.0;


    // Publier le mode initial
    if (!mqtt.publier(
            TopicsMqtt::MODE,
            controle.obtenirMode()))
    {
        std::cerr << "Erreur publication mode initial"
                << std::endl;
    }


    afficherAideConsole();
    ConsoleInteractive console;
    bool alarmeNiveauVerrouillee = false;
    bool alarmeGB = false;
    bool alarmePB = false;

    // Vrai delta t (la lecture serie prend un temps variable).
    auto derniereIteration = std::chrono::steady_clock::now();

    while (!finProgrammeDemandee)
    {
        if (!mqtt.estConnecte()) {
            std::cerr << "Perte MQTT : arret securitaire, relancer le programme.\n";
            break;
        }
        console.lire(regulation);
        if (finProgrammeDemandee)
            break;
        if (demandeArretUrgence != 0 &&
            !controle.estArretUrgenceActif())
        {
            controle.declencherArretUrgence();
            demandeMode.store(-1);
            pompeAppliquee = false;

            std::cerr << "\nARRET LOGICIEL DEMANDE"
                    << std::endl;

            if (panneau.mettreEnSecuriteSimulateur())
            {
                std::cout << "Commandes de mise en securite acceptees"
                        << std::endl;
            }
            else
            {
                std::cerr << "ERREUR : mise en securite non confirmee : "
                        << panneau.obtenirErreur()
                        << std::endl;
            }

            if (!mqtt.publier(TopicsMqtt::MODE,
                            controle.obtenirMode()))
            {
                std::cerr << "Erreur publication mode MQTT"
                        << std::endl;
            }
        }



        int commande = demandeMode.exchange(-1);


        if (commande != -1 && alarmeNiveauVerrouillee)
        {
            std::cerr << "Mode refuse : alarme active, utiliser rearme.\n";
            commande = -1;
        }

        if (commande != -1)
        {
            std::string nouveauMode =
                (commande == 1) ? "auto" : "manuel";

            if (controle.changerMode(nouveauMode))
            {
                regulation.reinitialiser();

                // Ne pas perdre les commandes manuelles arrivees juste
                // apres un changement de mode par MQTT.
                if (nouveauMode == "auto")
                {
                    demandeValveManuelleGB.store(-1);
                    demandeValveManuellePB.store(-1);
                    demandePompeManuelle.store(-1);
                }

                valveManuelleGB = 0.0;
                valveManuellePB = 0.0;
                pompeManuelle = false;
                pompeAutoriseeAuto = false;
                demandePompeManuelle.store(-1);
                commandesManuellesActives = false;

                if (!panneau.mettreEnSecuriteSimulateur())
                {
                    controle.declencherArretUrgence();

                    std::cerr
                        << "ERREUR : changement de mode non securise"
                        << std::endl;
                }

                sortiesAutoActives = false;
                pompeAppliquee = false;

                std::cout << "\nMode actuel : "
                        << controle.obtenirMode()
                        << std::endl;

                if (!mqtt.publier(
                        TopicsMqtt::MODE,
                        controle.obtenirMode()))
                {
                    std::cerr << "Erreur publication du mode"
                            << std::endl;
                }
            }
        }


        int nouvelleConsigneGB = demandeConsigneGB.exchange(-1);
        int nouvelleConsignePB = demandeConsignePB.exchange(-1);

        if (nouvelleConsigneGB != -1)
        {
            if (controle.definirConsigneGB(nouvelleConsigneGB))
            {
                std::cout << "\nNouvelle consigne GB : "
                        << controle.obtenirConsigneGB()
                        << " %" << std::endl;
            }
        }

        if (nouvelleConsignePB != -1)
        {
            if (controle.definirConsignePB(nouvelleConsignePB))
            {
                std::cout << "\nNouvelle consigne PB : "
                        << controle.obtenirConsignePB()
                        << " %" << std::endl;
            }
        }

        const int nouveauSeuilGB = demandeSeuilAlarmeGB.exchange(-1);
        const int nouveauSeuilPB = demandeSeuilAlarmePB.exchange(-1);
        if (nouveauSeuilGB >= 0) seuilAlarmeGB = nouveauSeuilGB;
        if (nouveauSeuilPB >= 0) seuilAlarmePB = nouveauSeuilPB;

        const int commandePompe = demandePompeManuelle.exchange(-1);
        bool pompeManuelleModifiee = false;
        if (commandePompe >= 0 && !alarmeNiveauVerrouillee &&
            !controle.estArretUrgenceActif())
        {
            if (controle.obtenirMode() == "auto")
                pompeAutoriseeAuto = commandePompe == 1;
            else
            {
                pompeManuelle = commandePompe == 1;
                pompeManuelleModifiee = true;
            }
        }

        PanelReading lecture{};

        
        
        if (!panneau.lireCapteurs(lecture))
        {
            const std::string erreurSerie =
                panneau.obtenirErreur();

            std::cerr << "\nALARME : PERTE COMMUNICATION SERIE"
                      << std::endl;

            std::cerr << "Detail : " << erreurSerie
                      << std::endl;

            controle.declencherArretUrgence();
            regulation.reinitialiser();
            demandeMode.store(-1);
            sortiesAutoActives = false;
            pompeAppliquee = false;

            const bool securiteOK =
                panneau.mettreEnSecuriteSimulateur();

            if (!securiteOK)
            {
                std::cerr
                    << "ERREUR CRITIQUE : mise en securite "
                    << "non confirmee"
                    << std::endl;
            }

            if (!mqtt.publier(
                    TopicsMqtt::ALARMES,
                    "PERTE_COMMUNICATION_SERIE", true))
            {
                std::cerr
                    << "Erreur publication alarme MQTT"
                    << std::endl;
            }

            mqtt.deconnecter();
            panneau.deconnecter();

            return 1;
        }


        const bool debordement =
            lecture.debordementGrosBassin ||
            lecture.debordementPetitBassin;

        const bool niveauInvalide =
            !std::isfinite(lecture.niveauGrosBassin) ||
            !std::isfinite(lecture.niveauPetitBassin) ||
            lecture.niveauGrosBassin < -2.0 ||
            lecture.niveauPetitBassin < -2.0 ||
            lecture.niveauGrosBassin >= 100.0 ||
            lecture.niveauPetitBassin >= 100.0;

        

        
        // Depassement de niveau / debordement : une alarme de procede,
        // distincte d'un arret d'urgence materiel ou d'une panne serie.
        // La pompe est commune : on ferme les deux arrivees pour eviter
        // tout remplissage involontaire pendant l'alarme.
        const bool nouveauDefautGB = lecture.debordementGrosBassin ||
                                     lecture.niveauGrosBassin > seuilAlarmeGB;
        const bool nouveauDefautPB = lecture.debordementPetitBassin ||
                                     lecture.niveauPetitBassin > seuilAlarmePB;
        const bool mesureDefectueuse =
            !std::isfinite(lecture.niveauGrosBassin) ||
            !std::isfinite(lecture.niveauPetitBassin) ||
            lecture.niveauGrosBassin < -2.0 ||
            lecture.niveauPetitBassin < -2.0;

        // DEBUT DU CODE AJOUTE AVEC L'AIDE D'UNE IA
        const bool conditionsAlarme[] = {lecture.debordementGrosBassin,
            lecture.debordementPetitBassin, lecture.niveauGrosBassin > seuilAlarmeGB,
            lecture.niveauPetitBassin > seuilAlarmePB};
        for (int i = 0; i < 4; ++i) {
            const bool ack = ackAlarmes[i].exchange(false);
            if (ack && !mqtt.publier(topicsAlarmes[i], "", true)) {
                ackAlarmes[i].store(true);
                continue;
            }
            if (alarmes[i].actualiser(conditionsAlarme[i], ack,
                    i < 2 ? 0 : tempsAlarmes[i-2].load(),
                    i < 2 ? 10 : tempsAlarmes[i].load()))
                if (!mqtt.publier(topicsAlarmes[i], "ON", true))
                    alarmes[i].publicationEchouee();
        }
        // FIN DU CODE AJOUTE AVEC L'AIDE D'UNE IA
        if ((nouveauDefautGB || nouveauDefautPB || niveauInvalide || mesureDefectueuse) &&
            !alarmeNiveauVerrouillee)
        {
            alarmeNiveauVerrouillee = true;
            alarmeGB = nouveauDefautGB;
            alarmePB = nouveauDefautPB;
            regulation.reinitialiser();
            derniereCommandePidGB = 0.0;
            derniereCommandePidPB = 0.0;
            valveManuelleGB = 0.0;
            valveManuellePB = 0.0;
            commandesManuellesActives = false;
            sortiesAutoActives = false;
            pompeAppliquee = false;
            pompeManuelle = false;
            pompeAutoriseeAuto = false;
            demandeValveManuelleGB.store(-1);
            demandeValveManuellePB.store(-1);
            demandePompeManuelle.store(-1);
            if (!panneau.mettreEnSecuriteSimulateur())
            {
                std::cerr << "ERREUR CRITIQUE : fermeture non confirmee : "
                          << panneau.obtenirErreur() << std::endl;
                controle.declencherArretUrgence();
            }
            std::cerr << "ALARME NIVEAU : actionneurs fermes. "
                      << "Corriger le niveau puis saisir rearme.\n";
        }

        // Un rearmement est refuse tant que le niveau n'est pas redescendu
        // sous 90 %, que le capteur signale encore le debordement, ou si
        // une mesure est defectueuse. L'hysteresis evite les rearmements
        // immediats autour du seuil d'alarme de 95 %.
        if (demandeRearmement.exchange(false))
        {
            const bool retourSur = !mesureDefectueuse &&
                !lecture.debordementGrosBassin &&
                !lecture.debordementPetitBassin &&
                lecture.niveauGrosBassin < std::max(0, seuilAlarmeGB - 1) &&
                lecture.niveauPetitBassin < std::max(0, seuilAlarmePB - 1);
            if (alarmeNiveauVerrouillee && retourSur &&
                !controle.estArretUrgenceActif())
            {
                alarmeNiveauVerrouillee = false;
                alarmeGB = false;
                alarmePB = false;
                regulation.reinitialiser();
                // Reprise en MANUEL, sans ouverture automatique.
                controle.changerMode("manuel");
                pompeManuelle = false;
                pompeAutoriseeAuto = false;
                mqtt.publier(TopicsMqtt::MODE, controle.obtenirMode());
                std::cout << "Alarme rearmee : MANUEL, pompe OFF, valves fermees.\n";
            }
            else
                std::cerr << "Rearmement refuse : niveau/capteur non sur "
                          << "ou arret d'urgence actif.\n";
        }

        const bool modeAuto =
            controle.obtenirMode() == "auto";

        const auto maintenant = std::chrono::steady_clock::now();
        double dtRegulation =
            std::chrono::duration<double>(maintenant - derniereIteration).count();
        derniereIteration = maintenant;
        if (!std::isfinite(dtRegulation) || dtRegulation < 0.01 ||
            dtRegulation > 5.0)
        {
            regulation.reinitialiser();
            dtRegulation = 1.0;
        }

        const bool regulationAutorisee =
            modeAuto &&
            pompeAutoriseeAuto &&
            !controle.estArretUrgenceActif() &&
            !debordement &&
            !niveauInvalide &&
            !alarmeNiveauVerrouillee;

        if (regulationAutorisee)
        {
            CommandesBassins cmd = regulation.calculer(
                controle.obtenirConsigneGB(),
                controle.obtenirConsignePB(),
                lecture.niveauGrosBassin,
                lecture.niveauPetitBassin,
                dtRegulation,
                true
            );

            bool succes = true;

            if (!cmd.pompe)
            {
                succes = panneau.commanderPompe(false);
            }

            if (succes)
            {
                succes = panneau.commanderValveGB(cmd.valveGB);
            }

            if (succes)
            {
                succes = panneau.commanderValvePB(cmd.valvePB);
            }

            if (succes && cmd.pompe)
            {
                succes = panneau.commanderPompe(true);
            }

            if (!succes)
            {
                controle.declencherArretUrgence();
                regulation.reinitialiser();

                std::cerr << "ARRET : erreur commande actionneur : "
                        << panneau.obtenirErreur()
                        << std::endl;

                if (!panneau.mettreEnSecuriteSimulateur())
                {
                    std::cerr << "ERREUR CRITIQUE : "
                            << "mise en securite non confirmee"
                            << std::endl;
                }

                sortiesAutoActives = false;
                pompeAppliquee = false;
                mqtt.publier(TopicsMqtt::MODE, controle.obtenirMode());
            }
            else
            {
                sortiesAutoActives = true;
                pompeAppliquee = cmd.pompe;

                // Conserver les sorties PID pour la commande etat.
                // Aucun affichage periodique pendant la saisie.
                derniereCommandePidGB = cmd.valveGB;
                derniereCommandePidPB = cmd.valvePB;
            }
        }
        else
        {
            regulation.reinitialiser();

            if (sortiesAutoActives ||
                controle.estArretUrgenceActif())
            {
                if (!panneau.mettreEnSecuriteSimulateur())
                {
                    std::cerr << "ERREUR CRITIQUE : "
                            << "arret des actionneurs non confirme"
                            << std::endl;

                    controle.declencherArretUrgence();
                }

                sortiesAutoActives = false;
                pompeAppliquee = false;
            }
        }


        const int nouvelleValveGB =
            demandeValveManuelleGB.exchange(-1);

        const int nouvelleValvePB =
            demandeValveManuellePB.exchange(-1);

        const bool manuelAutorise =
            controle.obtenirMode() == "manuel" &&
            !controle.estArretUrgenceActif() &&
            !debordement &&
            !niveauInvalide &&
            !alarmeNiveauVerrouillee;

        if (manuelAutorise)
        {
            bool modification = pompeManuelleModifiee;

            if (nouvelleValveGB >= 0)
            {
                valveManuelleGB = nouvelleValveGB;
                modification = true;
            }

            if (nouvelleValvePB >= 0)
            {
                valveManuellePB = nouvelleValvePB;
                modification = true;
            }

            if (modification || commandesManuellesActives)
            {
                const bool demandePompe =
                    pompeManuelle &&
                    (valveManuelleGB > 0.0 ||
                     valveManuellePB > 0.0);

                bool succes = true;

                if (!demandePompe)
                {
                    succes = panneau.commanderPompe(false);
                }

                if (succes)
                {
                    succes = panneau.commanderValveGB(
                        valveManuelleGB);
                }

                if (succes)
                {
                    succes = panneau.commanderValvePB(
                        valveManuellePB);
                }

                if (succes && demandePompe)
                {
                    succes = panneau.commanderPompe(true);
                }

                if (!succes)
                {
                    controle.declencherArretUrgence();

                    std::cerr
                        << "ARRET : erreur commande manuelle"
                        << std::endl;

                    if (!panneau.mettreEnSecuriteSimulateur())
                    {
                        std::cerr
                            << "ERREUR CRITIQUE : "
                            << "mise en securite non confirmee"
                            << std::endl;
                    }

                    commandesManuellesActives = false;
                    pompeAppliquee = false;
                    pompeManuelle = false;
                    mqtt.publier(TopicsMqtt::MODE, controle.obtenirMode());
                }
                else
                {
                    commandesManuellesActives = true;
                    pompeAppliquee = demandePompe;

                    if (modification)
                    {
                        std::cout
                            << "MANUEL GB : " << valveManuelleGB
                            << " % | PB : " << valveManuellePB
                            << " % | Pompe : "
                            << (demandePompe ? "ON" : "OFF")
                            << std::endl;
                    }
                }
            }
        }
        else if (commandesManuellesActives)
        {
            if (!panneau.mettreEnSecuriteSimulateur())
            {
                std::cerr
                    << "ERREUR CRITIQUE : "
                    << "arret manuel non confirme"
                    << std::endl;

                controle.declencherArretUrgence();
            }

            commandesManuellesActives = false;
            pompeAppliquee = false;
            pompeManuelle = false;
            valveManuelleGB = 0.0;
            valveManuellePB = 0.0;
        }

        if (!mqtt.publier(TopicsMqtt::VALVE_GB,
                std::to_string(limiter(
                    static_cast<int>(std::lround(lecture.ouvertureValve[1])), 0, 100))) ||
            !mqtt.publier(TopicsMqtt::VALVE_PB,
                std::to_string(limiter(
                    static_cast<int>(std::lround(lecture.ouvertureValve[2])), 0, 100))) ||
            !mqtt.publier(TopicsMqtt::POMPE,
                pompeAppliquee ? "on" : "off"))
            std::cerr << "Publication des actionneurs impossible.\n";

        // Publication des niveaux mesures
        int niveauGB = static_cast<int>(
            std::lround(lecture.niveauGrosBassin)
        );

        int niveauPB = static_cast<int>(
            std::lround(lecture.niveauPetitBassin)
        );

        // Limiter les valeurs MQTT entre 0 et 100 %
        niveauGB = limiter(niveauGB, 0, 100);
        niveauPB = limiter(niveauPB, 0, 100);

        if (!mqtt.publier(TopicsMqtt::NIV_GB,std::to_string(niveauGB)))
        {
            std::cerr << "Erreur publication niveau GB" << std::endl;
        }

        if (!mqtt.publier(TopicsMqtt::NIV_PB,std::to_string(niveauPB)))
        {
            std::cerr << "Erreur publication niveau PB" << std::endl;
        }

        // Publication de la temperature du petit bassin
        int temperaturePB = static_cast<int>(std::lround(lecture.temperaturePetitBassin));

        if (!mqtt.publier(TopicsMqtt::TMP_PB,std::to_string(temperaturePB)))
        {
            std::cerr << "Erreur publication temperature PB" << std::endl;
        }

        // Le tableau reste a sa place et se met a jour a chaque lecture.
        console.afficher(lecture, controle, derniereCommandePidGB,
                        derniereCommandePidPB, pompeAppliquee,
                        alarmeGB, alarmePB, alarmeNiveauVerrouillee,
                        seuilAlarmeGB, seuilAlarmePB);

        for (int i = 0; i < 50 && !finProgrammeDemandee; ++i)
        {
            console.lire(regulation);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    
    
    std::cout << "\nARRET DU PROGRAMME DEMANDE"
              << std::endl;

    regulation.reinitialiser();

    const bool securiteOK =
        panneau.mettreEnSecuriteSimulateur();

    if (securiteOK)
    {
        std::cout << "Commandes de mise en securite acceptees"
                  << std::endl;
    }
    else
    {
        std::cerr << "ERREUR CRITIQUE : "
                  << "mise en securite non confirmee : "
                  << panneau.obtenirErreur()
                  << std::endl;
    }

    mqtt.deconnecter();
    panneau.deconnecter();

    std::cout << "Fermeture du programme Panneau RAM"
              << std::endl;

    return securiteOK ? 0 : 1;
}
// FIN DU FICHIER CONTENANT DES MODIFICATIONS AVEC L'AIDE D'UNE IA