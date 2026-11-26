# Procédure de maintenance — unité du panneau RAM

**État :** guide actualisé après les essais du 25 novembre 2026. À utiliser avec les procédures approuvées du laboratoire.

## Démarrer

1. Vérifier l'alimentation, les arrivées d'eau, les actionneurs et la coupure matérielle.
2. Confirmer le port série (`/dev/ttyUSB0` au laboratoire) et l'adresse du broker MQTT (`172.17.15.206`).
3. Démarrer le programme et vérifier la connexion série, MQTT et l'état initial sécuritaire.
4. Vérifier que les mesures sont plausibles, que la pompe est OFF et que les valves sont fermées.
5. Tester en manuel à faible ouverture avant de passer en AUTO.

## Réagir aux anomalies

| Panne / symptôme | Réaction prévue | Vérification de maintenance |
|---|---|---|
| Lecture série absente | Tentative de mise en sécurité, message d'erreur, arrêt du programme | Liaison RS-232, adaptateur, port; vérifier physiquement les sorties |
| Niveau invalide | Verrouillage de sécurité | Capteur, boucle 4–20 mA, calibration |
| Débordement | Mise en sécurité et demande de réarmement | Tige et niveau réel; attendre la disparition du défaut avant `rearme` |
| Alarme de niveau | Avertissement MQTT sans arrêt du PID ou de la pompe | Seuil, niveau, TG/TR et reconnaissance ACK |
| Commande d'ouverture refusée | Arrêt logiciel en cas d'erreur de commande matérielle | Module de sortie, pression, circuit de commande |
| Pompe ne démarre pas | Vérifier `pompe on`, mode et sécurité | Pompe, relais, circuit hydraulique |
| Niveau n'atteint pas la consigne | Vérifier débit et saturation PID | Débit, purge, sens des valves, gains, capteurs |
| Broker MQTT indisponible | Arrêt sécuritaire demandé, programme terminé | Broker, IP, port 1883, réseau; vérifier les sorties avant relance |
| Arrêt logiciel `arret` | Commandes bloquées jusqu'au redémarrage | Corriger la cause et vérifier les sorties avant relance |

## Réarmement et reprise

Après un débordement, vérifier que les deux tiges indiquent NON et que les niveaux sont sécuritaires. `rearme` permet de revenir en manuel, pompe OFF et valves fermées. Après `arret`, le redémarrage du programme est nécessaire. Le réarmement ne doit pas déclencher automatiquement une ancienne commande de pompe.

## Cas limites à faire valider

La perte RS-232 en fonctionnement, les défauts de capteur et le refus de réarmement pendant un débordement persistant n'ont pas fait l'objet d'un essai spécifique documenté. En cas de perte de liaison, une commande logicielle de fermeture peut ne jamais atteindre le panneau; ne pas considérer l'arrêt du programme comme une preuve de mise en sécurité physique. Les états MQTT retained peuvent être périmés.