# Essais — panneau RAM

État au 25 novembre 2026 (livrable du 26 novembre). Les résultats ci-dessous proviennent des essais sur le panneau réel et des confirmations de l'opérateur.

## Essais logiciels déjà réalisés

| Essai | Résultat |
|---|---|
| `pid_unitaire` | Réussi |
| `regulation_unitaire` | Réussi |
| `regulation_purge` (dont alarmes) | Réussi |
| `pid_derivation` | Réussi |
| Compilation WSL et ARM | Réussie avant l'ajout final des publications de consignes |

## Essais sur le panneau réel

| Scénario | Résultat observé |
|---|---|
| Communication RS-232 | Lecture des niveaux, température et positions des valves |
| Mode manuel, ouverture GB/PB | Positions proches des commandes; à 25 %, 24,21 % et 25,68 % observés |
| Pompe manuelle | Commandes ON/OFF fonctionnelles |
| Mode automatique | Régulation des deux bassins et changements de consignes observés |
| Consigne zéro | Fermeture observée avec pompe arrêtée |
| Pompe en AUTO | Maintenue active à proximité des consignes lors des essais |
| Purge ouverte isolément | Pas de mesure dédiée documentée |
| Alarme de niveau | ON sans coupure de la pompe ou du PID |
| Génération d'alarme | Environ 3 s (mesure MQTT arrondie) |
| ACK / répétition | Effacement retained, puis ON après 10 s sur GB et PB |
| Arrêt logiciel | Valves proches de 0 %, pompe OFF, commandes bloquées |
| Redémarrage | Mode manuel, pompe OFF, valves proches de 0 %, aucune reprise des anciennes commandes observée |
| Perte MQTT | Message d'erreur, commandes de sécurité acceptées et arrêt du programme |
| Débordement GB | `ALR_GB_OVF ON` à 97 %, maximum publié 98 % |
| Débordement PB | `ALR_PB_OVF ON` à 91 % |
| Réarmement après débordement | Accepté et reprise manuelle confirmée par l'opérateur |
| Perte série en fonctionnement | Non testée physiquement; traitement présent dans le code |
| Publications MQTT | États sur changement observés; nouvel ajout des consignes non testé |

## Test sur panneau réel

Les essais doivent être réalisés selon la procédure du laboratoire, après vérification des capteurs, de l'arrêt matériel et du circuit hydraulique. Les journaux de débordement confirment le déclenchement des alarmes et la baisse des niveaux; ils ne contiennent pas les états simultanés de la pompe et des valves. La perte MQTT confirme l'acceptation des commandes de sécurité, mais pas la fermeture physique au moment exact de la panne.

Les résultats détaillés et les courbes sont dans [resultats_essais_26nov.md](resultats_essais_26nov.md).