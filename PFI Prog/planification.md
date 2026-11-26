# Planification de la PFI — Programmation temps réel
<!-- Planification élaborée avec l'aide de ChatGPT. -->

**Cours :** 247-GFB-LG — Programmation temps réel  
**Session :** Automne 2026  
**Portée de ce plan :** section 3 — Unité de contrôle du panneau R.A.M. et section 6 — Unité de contrôle du Power Meter  
**Remise de la planification :** jeudi 12 novembre 2026, à la fin du laboratoire  
**Remise finale du dépôt :** vendredi 11 décembre 2026, avant 23 h

## Tableau de planification

| Livrable | Date prévue | État | Date réelle | Note |
|---|---|---|---|---|

| Déposer le fichier `planification.md` dans le dépôt GitHub. | Jeudi 12 novembre | fait | 12 novembre 2026 | Premier livrable obligatoire. |

| Organiser les dossiers et vérifier la compilation C++ des programmes panneau RAM et Power Meter. | Jeudi 12 novembre | en cours | — | Structure du panneau RAM terminée. Compilation WSL et ARM réussie et 4 tests unitaires réussis avant le dernier ajout MQTT. Power Meter à compléter. |

| Faire communiquer le Raspberry Pi avec le panneau RAM et lire les niveaux des deux bassins. | Jeudi 19 novembre | fait | 19 novembre 2026 | Communication validée avec le simulateur RAM le 19 novembre et avec le panneau réel le 25 novembre. |

| Mettre en place la communication MQTT du panneau RAM : publier les états et recevoir les commandes. | Jeudi 19 novembre | en cours | — | Commandes de mode, valves et consignes testées. Publication des niveaux, température, positions des valves, pompe et alarmes fonctionnelle. Optimisation des publications réalisée. Synchronisation des consignes ajoutée, mais non testée. Finalisation visée le 3 décembre. |

| Établir une première communication série avec le Power Meter. | Jeudi 19 novembre | en retard | — | Appareil non disponible et protocole série absent de l'énoncé. Nouvelle date visée : 3 décembre 2026, sous réserve de l'accès au matériel et au protocole. |

| Programmer la régulation PID du niveau des deux bassins du panneau RAM. | Jeudi 26 novembre | fait | 25 novembre 2026 | PID GB et PB réglés et testés sur le panneau réel. Gains GB : 5 / 0,7 / 2. Gains PB : 2 / 0,18 / 1. Sorties bornées entre 0 et 100 %. Régulation simultanée fonctionnelle. |

| Détecter les débordements et dépassements de niveau, transmettre les alarmes et tester les sécurités du panneau RAM. | Jeudi 26 novembre | en cours | — | Alarmes de niveau, génération après environ 3 s, répétition après ACK de 10 s, débordements GB/PB, réarmement, arrêt logiciel et perte MQTT validés sur le panneau réel. Perte RS-232 programmée, mais non testée physiquement en fonctionnement. |

| Faire fonctionner les modes MANUEL et AUTOMATIQUE et le changement de consigne de niveau. | Jeudi 26 novembre | fait | 25 novembre 2026 | Commandes manuelles et automatiques fonctionnelles. Changements de consignes et régulation simultanée validés sur le panneau réel. Pompe maintenue active en AUTO lorsqu'elle est autorisée. |

| Vérifier le fonctionnement du PID de niveau lorsqu'une purge est ouverte. | Jeudi 26 novembre | en cours | — | Régulation indépendante des mesures des valves de purge. Essai spécifique avec une purge ouverte restant à documenter. Ne comprend pas la programmation de l'unité distincte des valves de purge (section 7). |

| Documenter les premiers essais de niveau et les réglages du PID. | Jeudi 26 novembre | en cours | — | Gains définitifs, observations, résultats des essais, courbes expérimentales et théoriques documentés. Les courbes expérimentales de chaque combinaison intermédiaire de gains ne sont pas disponibles. |

| Réguler la température du petit bassin en maintenant le contrôle de niveau. | Jeudi 3 décembre | à faire | — | Tester une consigne initiale, puis son changement, pendant que le niveau demeure régulé; documenter les réglages. |

| Finaliser l'affichage local et la synchronisation des commandes du panneau RAM. | Jeudi 3 décembre | à faire | — | Vérifier les modes et commandes depuis la console locale et le superviseur MQTT, sans désaccord entre les états. |

| Lire et afficher les mesures du Power Meter (tensions, courants, puissance, énergie, facteur de puissance). | Jeudi 3 décembre | à faire | — | Confirmer le modèle, le protocole et les unités à partir de l'appareil ou de la documentation technique; vérifier des données réelles. Dépend de la disponibilité matérielle. |

| Publier les mesures du Power Meter sur les topics MQTT prévus. | Jeudi 3 décembre | à faire | — | Utiliser les huit topics de l'annexe I et publier chaque nouvelle valeur lorsque la mesure change. |

| Détecter la perte de communication avec le Power Meter et émettre l'alarme correspondante. | Jeudi 3 décembre | à faire | — | Vérifier la détection d'une perte du lien et le comportement au retour de la communication. |

| Revoir la structure du code C++ des deux unités (SOLID, The Power of 10, machines à états si pertinentes). | Jeudi 10 décembre | à faire | — | Vérifier la séparation des responsabilités, la lisibilité, la gestion des erreurs et la conformité du code au plan de conception. |

| Rédiger les documents de conception, de réglage et les plans d'essai nécessaires pour les deux unités. | Jeudi 10 décembre | à faire | — | Conserver des documents courts expliquant le code, ses paramètres et les résultats d'essai. |

| Rédiger la documentation de maintenance du panneau RAM et celle du Power Meter. | Jeudi 10 décembre | à faire | — | Pour chaque unité : démarrage, essais de sécurité, cas limites, pannes possibles, réactions attendues et démarches de diagnostic. |

| Faire les essais de démonstration et de pannes des deux unités. | Jeudi 10 décembre | à faire | — | Vérifier les scénarios applicables : perte de lien série, appareil muet, capteur défaillant, débordement, broker arrêté, commande invalide, arrêt et relance. |

| Vérifier les fichiers, compiler les deux programmes et pousser le travail final sur GitHub. | Vendredi 11 décembre | à faire | — | Dépôt final et documents remis avant 23 h. |


## Suivi du 26 novembre 2026

### Travaux réalisés

- Communication série avec le panneau RAM réel.

- Validation des lectures de niveaux, de température et des positions des valves.

- Validation des modes MANUEL et AUTOMATIQUE.

- Réglage et validation des deux PID sur le panneau réel.

- Validation de la régulation simultanée des bassins.

- Validation des changements de consigne.

- Optimisation des publications MQTT.

- Ajout de la publication des consignes de niveau.

- Validation des alarmes de niveau.

- Validation de la temporisation de génération de 3 secondes, approximativement.

- Validation de la temporisation de répétition de 10 secondes après ACK.

- Validation des alarmes de débordement des deux bassins.

- Validation du réarmement après débordement.

- Validation de l'arrêt logiciel et du verrouillage des commandes.

- Validation du redémarrage sécuritaire.

- Validation de la détection de perte MQTT.

- Séparation de GestionAlarmes.h et GestionAlarmes.cpp.

- Compilation WSL et ARM avant le dernier ajout MQTT.

- Réussite des quatre tests unitaires avant le dernier ajout MQTT.

- Mise à jour de la documentation technique et de maintenance.

- Création des courbes expérimentales et théoriques.


### Gains PID retenus

| Bassin | Kp | Ki | Kd |
|---|---|---|---|
| GB | 5,0 | 0,70 | 2,0 |
| PB | 2,0 | 0,18 | 1,0 |


### Travaux restant à valider

- Perte de communication RS-232 pendant le fonctionnement.

- Synchronisation MQTT des nouvelles publications de consignes.

- Essai spécifique du PID avec une purge ouverte.

- Courbes expérimentales des différents réglages intermédiaires du PID.

- Régulation de température du petit bassin.

- Commandes des valves d'eau chaude et d'eau froide.

- Communication et programmation du Power Meter.


### Documentation

Les résultats des essais sont disponibles dans le dossier `PFI Prog/docs/`.

Les courbes expérimentales et théoriques sont conservées dans le dossier `PFI Prog/docs/courbes/`.

Les courbes théoriques représentent le comportement attendu et sont distinctes des mesures réelles.


## Priorités de réalisation

### Pour le suivi du 19 novembre

- Présenter la communication avec le simulateur RAM, la lecture des niveaux et les échanges MQTT effectivement validés.
- Montrer les commits distincts et les essais correspondants.
- Signaler le blocage de la liaison série du Power Meter et la nouvelle date visée; ne pas le déclarer fonctionnel sans lecture réelle.

### Pour le livrable obligatoire du 26 novembre

1. Terminer le calcul PID des deux bassins et tester ses limites.
2. Intégrer la régulation aux commandes réelles du panneau RAM, en prévoyant un comportement sécuritaire.
3. Terminer les modes MANUEL/AUTOMATIQUE, les commandes locales/MQTT et les changements de consigne.
4. Réaliser les alarmes, les protections de débordement et les systèmes de secours.
5. Tester la régulation avec une purge ouverte.
6. Documenter les réglages PID, les essais et les courbes obtenues.

### Pour le 3 décembre

- Réguler la température du petit bassin en maintenant le niveau.
- Finaliser la cohérence de la console locale et du superviseur MQTT.
- Si le protocole et l'appareil sont accessibles, terminer les lectures réelles et les publications MQTT du Power Meter ainsi que la détection de perte de lien.

### Pour le 10 et le 11 décembre

- Finaliser les vérifications de qualité du code et les documents.
- Effectuer les essais de pannes et valider la documentation de maintenance.
- Confirmer que les programmes compilent depuis le dépôt avant la remise finale du 11 décembre à 23 h.

## Mise à jour chaque jeudi

- Mettre la planification à jour et pousser le travail sur GitHub **avant 8 h le jeudi**, jusqu'au 10 décembre, conformément au gabarit du professeur. La mise à jour faite après un laboratoire est complémentaire : elle ne remplace pas celle d'avant 8 h.
- **Ne jamais changer la date prévue :** les dates du tableau initial du 12 novembre demeurent fixes.
- Utiliser uniquement les états `à faire`, `en cours`, `fait` ou `en retard`.
- Inscrire la **date réelle** seulement lorsque le livrable est terminé et vérifié.
- Pour chaque retard, indiquer la cause et une nouvelle date visée dans **Note**.
- Un livrable `fait` doit être visible dans le dépôt GitHub et démontrable. Les tests simulés et les tests sur matériel réel doivent être distingués.
- Présenter au suivi du jeudi la démonstration, le code, les essais et les commits de la semaine.

## Préparation des suivis

| Date | Démonstration / revue prévue |
|---|---|
| Jeudi 12 novembre | Planification ajoutée au dépôt GitHub. |
| Jeudi 19 novembre | Communication RAM et lectures de niveaux; MQTT partiellement fonctionnel; signalement du blocage Power Meter. |
| Jeudi 26 novembre | **Livrable obligatoire :** PID des deux bassins, modes, changements de consigne, purge, alarmes et sécurités fonctionnels. |
| Jeudi 3 décembre | Température du petit bassin, synchronisation locale/MQTT, mesures du Power Meter et perte de lien si matériel disponible. |
| Jeudi 10 décembre | Mise à jour avant 8 h : revue du code, essais, documentation de maintenance et préparation de la remise. |

## Commits Git

Un commit = **une seule modification fonctionnelle dans une seule partie du système**, suivie d'un test lorsque c'est applicable. Les messages identifient la partie et l'action précise.

```text
planif : met a jour le suivi du 19 novembre
panneau : lit les niveaux des deux bassins
panneau : publie les niveaux sur MQTT
panneau : recoit les consignes de niveau MQTT
pid : calcule la commande du grand bassin
pid : calcule la commande du petit bassin
panneau : applique la regulation des niveaux
panneau : gere les modes manuel et automatique
panneau : detecte les debordements
essai : verifie le PID avec une purge ouverte
powermeter : ouvre la liaison serie
powermeter : lit les mesures energetiques
powermeter : publie les valeurs sur MQTT
powermeter : signale la perte du lien
maintenance : documente les essais du panneau RAM
maintenance : documente les pannes du Power Meter
```

Ces exemples ne sont **pas** une liste de commits déjà réalisés : chaque message doit correspondre aux modifications réellement testées et enregistrées.

## Limites de cette planification

Cette planification porte sur les **sections 3 et 6 uniquement**, comme défini lors de sa création. Elle ne planifie pas les sections 2, 4, 5, 7 et 8. Toutefois, la grille de correction du cours porte sur **les six unités Raspberry Pi** : cette limitation de portée ne retire donc pas les critères des autres unités de l'évaluation. La répartition effective des autres parties doit être confirmée avec le professeur.

## Références

- *247-GFB-LG-AUT2026-PFI-Gabarit-Planification.pdf* — dates, tableau, suivi du jeudi, commits.
- *247-GFB-LG-AUT2026-PFI-Grille.pdf* — livrable obligatoire du 26 novembre et critères d'évaluation.
- *Projet_Integration_TSO_session_5_automne_2025.docx* — sections 3 et 6 et annexe I (topics MQTT).
