
# PFI Prog — panneau RAM et Power Meter

Programme du panneau RAM en C++17, basé sur la bibliothèque `Panneau` fournie en laboratoire. Cette version travaille sur les **niveaux d'eau GB/PB et les commandes locales/MQTT**. Le Power Meter et la régulation de température ne sont pas terminés par cette modification.

## Compilation sur Raspberry Pi / Linux

```bash
sudo apt install cmake g++ libmosquitto-dev

mkdir -p build
cd build
cmake ..
cmake --build . -- -j2
ctest --output-on-failure
cd ..
```

Le projet est conçu pour être compatible avec CMake 3.7.2.

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
./build/panneau_ram /dev/ttyUSB0 172.17.15.206
```

Les commandes sont tapées dans **le même terminal où tourne le programme**.

### Commandes en mode manuel

```text
manuel
valve gb 50
valve gb 0
valve pb 40
valve pb 0
pompe on
pompe off
alarme gb 80
alarme pb 80
```

### Commandes en mode automatique

```text
auto
consigne gb 30
consigne pb 30
gains gb 5 0.7 3
gains pb 2 0.22 2
pompe on
pompe off
```

### Commandes générales

```text
etat
rearme
aide
arret
quitter
```

En MANUEL, les valves sont commandées directement. La pompe doit être autorisée avec `pompe on` et au moins une valve doit être ouverte.

En AUTO, les deux PID fonctionnent indépendamment et la pompe reste active tant qu'elle est autorisée et qu'aucun défaut de sécurité ne survient. Ce fonctionnement exige un circuit hydraulique adapté. Une consigne de 0 % ferme la valve correspondante. Les mesures des valves de purge ne sont pas nécessaires à la régulation.

## Réglages des PID

Les gains suivants ont été retenus après huit essais expérimentaux réalisés sur le panneau réel le 27 novembre 2026.

| Bassin | Kp | Ki | Kd | Limites de sortie |
|---|---:|---:|---:|---:|
| Grand bassin (GB) | 5,0 | 0,70 | 3,0 | 0 à 100 % |
| Petit bassin (PB) | 2,0 | 0,22 | 2,0 | 0 à 100 % |

Ces gains sont intégrés au constructeur de la classe `RegulationBassins` et sont appliqués au démarrage du programme. Ils peuvent aussi être modifiés directement à partir de la console sans redémarrage.

### Résultats expérimentaux

Lors de l'essai 07, les deux bassins ont été régulés à une consigne de 30 %.

| Indicateur | GB | PB |
|---|---:|---:|
| Consigne | 30 % | 30 % |
| Niveau final | 29,84 % | 29,48 % |
| Erreur absolue moyenne des 10 dernières mesures | 0,218 point | 0,506 point |
| Mesures finales dans la tolérance de ±1 point | 10/10 | 10/10 |

Aucune alarme ni aucun débordement n'a été enregistré durant cet essai.

La précision de ±1 point de pourcentage a été observée en fin d'essai. La validation de la stabilité pendant cinq minutes reste à effectuer.

Les résultats complets sont disponibles dans le [rapport d'optimisation PID](docs/optimisation_pid.md).

## Acquisition des données expérimentales

Le programme enregistre automatiquement les données dans :

```text
logs/pid_niveaux.csv
```

Le fichier contient 36 colonnes permettant notamment d'analyser :

- Le temps écoulé et le temps réel entre les acquisitions.
- Les consignes et les niveaux mesurés des deux bassins.
- Les erreurs de régulation.
- Les sorties des PID.
- Les ouvertures mesurées des valves.
- Les gains des deux PID.
- L'état de la pompe.
- La température du petit bassin.
- Les alarmes et les sécurités.

Pour observer les acquisitions dans un deuxième terminal :

```bash
tail -F logs/pid_niveaux.csv
```

Après avoir terminé l'essai et arrêté le programme de manière sécuritaire, sauvegarder les données :

```bash
cp logs/pid_niveaux.csv logs/pid_essai_08.csv
```

Le fichier `pid_niveaux.csv` est réinitialisé à chaque démarrage du programme. Les acquisitions doivent donc être sauvegardées avant un nouveau lancement.

Le dossier `logs/` est exclu du suivi Git. Les acquisitions expérimentales retenues et leurs graphiques sont conservés dans `docs/captures/pid/`.

## MQTT

Broker utilisé au laboratoire : `172.17.15.206:1883`. Les commandes locales et MQTT utilisent la même logique. Les topics sont sensibles à la casse.

| Préfixe | Topics |
|---|---|
| `RAM/PANNEAU/CMD/` | `MODE`, `VALVEGB`, `VALVEPB`, `CONSNIVGB`, `CONSNIVPB`, `POMPE` |
| `RAM/PANNEAU/ETATS/` | `MODE`, `NIVGB`, `NIVPB`, `TMPPB`, `VALVEGB`, `VALVEPB`, `POMPE` |
| `RAM/PANNEAU/ETATS/` | `CONSNIVGB`, `CONSNIVPB` (ajout de synchronisation, publication non validée spécifiquement sur le panneau réel) |
| `RAM/ALARMES/CMD/` | `NIVLHGB`, `NIVLHPB`, `TGNIVGB`, `TGNIVPB`, `TRNIVGB`, `TRNIVPB` |
| `RAM/ALARMES/ETATS/` | `ALR_GB_NIV_MAX`, `ALR_PB_NIV_MAX`, `ALR_GB_OVF`, `ALR_PB_OVF` |

Les états sont publiés sur changement et conservés par le broker (retained). Les dernières valeurs peuvent être périmées après une déconnexion. Les alarmes sont publiées avec `ON`; la reconnaissance `ACK` entraîne l'effacement du message retained.

Les alarmes de niveau utilisent par défaut TG = 3 s et TR = 10 s après reconnaissance. Elles ne coupent pas les actionneurs. Un débordement déclenche la mise en sécurité et nécessite un réarmement après disparition du défaut.

## Validation et limites connues

- Compilation WSL et ARM réussie selon les essais et les confirmations de l'opérateur.
- Quatre tests unitaires réussis.
- Compilation et tests réussis après l'intégration des gains optimisés, selon la confirmation de l'opérateur.
- Régulation GB/PB, alarmes de niveau, débordements, réarmement et perte MQTT testés sur panneau réel. Voir `docs/resultats_essais_26nov.md` pour les preuves et limites.
- Acquisition automatique des données CSV fonctionnelle sur le panneau réel.
- Huit essais expérimentaux réalisés pour optimiser les gains PID.
- Précision finale des deux bassins dans la tolérance de ±1 point de pourcentage lors de l'essai 07.
- Stabilité prolongée de cinq minutes non validée.
- Positions des valves de purge non enregistrées dans les acquisitions CSV.
- Perte série en fonctionnement non testée physiquement; la fermeture des actionneurs ne peut pas être garantie si la liaison est coupée.
- La régulation de température, les commandes des valves d'eau chaude/froide et le Power Meter restent à compléter.

## Documentation

| Document | Description |
|---|---|
| [Conception du panneau](docs/conception_panneau.md) | Architecture et fonctionnement du programme |
| [Essais du panneau](docs/essais_panneau.md) | Validation logicielle et essais physiques |
| [Réglages PID](docs/reglages_pid.md) | Gains retenus et fonctionnement de la régulation |
| [Optimisation PID](docs/optimisation_pid.md) | Analyse complète des huit essais expérimentaux |
| [Captures expérimentales](docs/captures/pid/) | Données CSV et graphiques des huit essais |
| [Résultats du 26 novembre](docs/resultats_essais_26nov.md) | Résultats des essais précédents |
| [Complément des essais](docs/complement_essais_alarmes_pid.md) | Essais supplémentaires des alarmes et des PID |
| [Maintenance du panneau](docs/maintenance_panneau.md) | Procédures de maintenance |
| [Conception du Power Meter](docs/conception_powermeter.md) | Documentation de conception du Power Meter |
| [Essais du Power Meter](docs/essais_powermeter.md) | Documentation des essais du Power Meter |
| [Maintenance du Power Meter](docs/maintenance_powermeter.md) | Procédures de maintenance du Power Meter |
| [Planification](planification.md) | Suivi de l'avancement du projet |
| [Trace IA](docs/trace_ia.md) | Documentation de l'utilisation de l'intelligence artificielle |

Les sections ajoutées avec assistance IA sont identifiées dans les fichiers et dans `docs/trace_ia.md`.
