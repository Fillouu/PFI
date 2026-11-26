# PFI Prog — panneau RAM et Power Meter

Programme du panneau RAM en C++17, basé sur la bibliothèque `Panneau` fournie en laboratoire. Cette version travaille sur les **niveaux d'eau GB/PB et les commandes locales/MQTT**. Le Power Meter et la régulation de température ne sont pas terminés par cette modification.

## Compilation sur Raspberry Pi / Linux

```bash
sudo apt install cmake g++ libmosquitto-dev
cmake -S . -B build
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Le projet attend un broker MQTT accessible sur le port 1883, et un port série utilisable par l'utilisateur courant.

## Lancer

Pour le simulateur RAM (une fois le pont `socat` créé vers le port TCP 5100 du simulateur sur le PC) :

```bash
socat pty,link=/tmp/ttyRAM,raw,echo=0 tcp:<IP_DU_PC>:5100
# Dans un autre terminal sur le Raspberry Pi :
./build/panneau_ram /tmp/ttyRAM <IP_DU_BROKER>
```

Sur le panneau physique, **après calibration et validation des sécurités** :

```bash
./build/panneau_ram /dev/ttyUSB0 <IP_DU_BROKER>
```

Les commandes sont tapées dans **le même terminal où tourne le programme** :

```text
manuel
valve gb 50
valve gb 0
valve pb 40
valve pb 0
pompe off
pompe on
consigne gb 50
consigne pb 75
auto
gains gb 5 0.7 2
gains pb 2 0.18 1
etat
aide
arret
quitter
```

En MANUEL, les valves sont commandées directement. La pompe doit être autorisée avec `pompe on` et au moins une valve doit être ouverte. En AUTO, les deux PID fonctionnent indépendamment et la pompe reste active tant qu'elle est autorisée et qu'aucun défaut de sécurité ne survient. Ce fonctionnement exige un circuit hydraulique adapté. Une consigne de 0 % ferme la valve correspondante. Les mesures des valves de purge ne sont pas nécessaires à la régulation.

## MQTT

Broker utilisé au laboratoire : `172.17.15.206:1883`. Les commandes locales et MQTT utilisent la même logique. Les topics sont sensibles à la casse.

| Préfixe | Topics |
|---|---|
| `RAM/PANNEAU/CMD/` | `MODE`, `VALVEGB`, `VALVEPB`, `CONSNIVGB`, `CONSNIVPB`, `POMPE` |
| `RAM/PANNEAU/ETATS/` | `MODE`, `NIVGB`, `NIVPB`, `TMPPB`, `VALVEGB`, `VALVEPB`, `POMPE` |
| `RAM/PANNEAU/ETATS/` | `CONSNIVGB`, `CONSNIVPB` (ajout de synchronisation après les essais, non recompilé ni testé) |
| `RAM/ALARMES/CMD/` | `NIVLHGB`, `NIVLHPB`, `TGNIVGB`, `TGNIVPB`, `TRNIVGB`, `TRNIVPB` |
| `RAM/ALARMES/ETATS/` | `ALR_GB_NIV_MAX`, `ALR_PB_NIV_MAX`, `ALR_GB_OVF`, `ALR_PB_OVF` |

Les états sont publiés sur changement et conservés par le broker (retained). Les dernières valeurs peuvent être périmées après une déconnexion. Les alarmes sont publiées avec `ON`; la reconnaissance `ACK` entraîne l'effacement du message retained. Les alarmes de niveau utilisent par défaut TG = 3 s et TR = 10 s après reconnaissance. Elles ne coupent pas les actionneurs. Un débordement déclenche la mise en sécurité et nécessite un réarmement après disparition du défaut.

## Validation et limites connues

- Compilation WSL et ARM et quatre tests unitaires réussis selon les essais du 25 novembre, avant le dernier ajout MQTT des consignes.
- Régulation GB/PB, alarmes de niveau, débordements, réarmement et perte MQTT testés sur panneau réel. Voir `docs/resultats_essais_26nov.md` pour les preuves et limites.
- Perte série en fonctionnement non testée physiquement; la fermeture des actionneurs ne peut pas être garantie si la liaison est coupée.
- La régulation de température, les commandes des valves d'eau chaude/froide et le Power Meter restent à compléter.
- Les documents `docs/essais_panneau.md`, `docs/reglages_pid.md` et `docs/maintenance_panneau.md` décrivent les essais, réglages et procédures.

Les sections ajoutées avec assistance IA sont identifiées dans les fichiers et dans `docs/trace_ia.md`.