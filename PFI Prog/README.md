
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
gains gb 4 0.03 0
gains pb 2 0.02 0
etat
aide
arret
quitter
```

En MANUEL, la pompe démarre automatiquement quand au moins une valve de remplissage est ouverte, sauf si une commande explicite `pompe off` l'interdit. Fermer toutes les valves arrête la pompe. `pompe on` la réautorise; il ne fait pas tourner une pompe face à deux valves de remplissage fermées. En AUTO, le PID commande les deux valves indépendamment. La pompe est partagée. Un nouveau mode commence par une remise à zéro des sorties.

## MQTT

Les topics `RAM/panneau/cmd/...` sont ceux de l'annexe I. Les commandes locales et les commandes reçues par MQTT suivent le même chemin logique. Le programme publie le mode, les niveaux, la température lue, les positions de valves lues et la commande de pompe. Les positions des **valves de purge**, distinctes, sont consommées facultativement sur `RAM/valves/etats/Ouverture_GB` et `RAM/valves/etats/Ouverture_PB`.

## Limites connues

- Tests unitaires et analyse syntaxique C++ vérifiés localement. **Aucun essai du simulateur RAM ni sur panneau physique, et aucune compilation liée à une vraie libmosquitto dans l'environnement de préparation.**
- La régulation PID doit encore être réglée sur le système réel, avec courbes. Les ouvertures maximales actuelles sont 50 % GB / 25 % PB et le seuil de sécurité de niveau est 95 %.
- Les alarmes temporisées, leurs topics `RAM/alarmes/etats/...` avec retenue MQTT, la protection complète contre la panne du broker et la régulation de température ne sont pas achevées.
- La fonctionnalité de la section 6 (Power Meter) n'est pas achevée et n'est pas modifiée ici.
- Les documents `docs/essais_panneau.md`, `docs/reglages_pid.md`, `docs/maintenance_panneau.md` contiennent les procédures et les essais qui restent à exécuter et à documenter.

Les sections ajoutées avec assistance IA sont identifiées dans les fichiers et dans `docs/trace_ia.md`.
