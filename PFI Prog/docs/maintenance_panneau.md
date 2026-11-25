
# Procédure de maintenance — unité du panneau RAM (version préliminaire)

**État :** guide rédigé avant validation des pannes en laboratoire. Il ne constitue pas une procédure de maintenance approuvée.

## Démarrer

1. Vérifier l'alimentation, les arrivées d'eau, les actionneurs et surtout le bouton d'arrêt d'urgence matériel.
2. Confirmer le port série et l'adresse IP du broker MQTT.
3. Démarrer le programme et vérifier l'ouverture du port série, la connexion MQTT et la mise en sécurité initiale.
4. Confirmer que les niveaux GB/PB et la température du PB sont plausibles avant d'envoyer des commandes.
5. Tester d'abord en manuel à faible ouverture; ne passer en AUTO qu'après validation des capteurs.

## Réagir aux anomalies

| Panne / symptôme | Réaction prévue | Vérification de maintenance |
|---|---|---|
| Lecture série absente | Sorties mises en sécurité, arrêt du programme | Câble, adaptateur USB–RS-232, port, débit et réponse DCON |
| Niveau non fini / hors limites | Arrêt logiciel et fermeture des actionneurs | Boucle 4–20 mA, alimentation capteur, calibration |
| Débordement | Fermeture immédiate demandée et passage en mode sécurité | Contact débordement et arrêt d'urgence matériel |
| Commande d'ouverture refusée | Arrêt logiciel | Module de sortie, pression pneumatique, circuit de commande |
| Pompe ne démarre pas | Contrôler d'abord le mode et l'autorisation `pompe on` | Pompe, relai de sortie, circuit hydraulique |
| Niveau n'atteint pas la consigne | Vérifier purge, capacité de remplissage et saturation PID | Débit, sens des valves, gains, capteurs |
| Broker MQTT indisponible | La protection complète contre la perte de broker doit encore être testée et complétée | Broker, IP, port 1883, réseau et surveillance de connexion |
| Arrêt logiciel `arret` | Verrouiller l'usage jusqu'au redémarrage | Corriger la panne avant de relancer |

## Cas limites à faire valider

Arrêt d'urgence, capteur ouvert/court-circuité, seuil de 95 %, reboot en cours de remplissage, câble série débranché, broker arrêté, commandes invalides, changements rapides de mode, perte des messages d'état des purges. Ces essais doivent être conduits sous supervision, avec résultats et traces datés.
