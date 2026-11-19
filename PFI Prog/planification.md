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

| Organiser les dossiers et vérifier la compilation C++ des programmes panneau RAM et Power Meter. | Jeudi 12 novembre | en cours | — | Structure et compilation du panneau RAM avancées. La compilation d'un programme Power Meter complet n'est pas encore démontrée. |

| Faire communiquer le Raspberry Pi avec le panneau RAM et lire les niveaux des deux bassins. | Jeudi 19 novembre | fait | 19 novembre 2026 | Communication et lecture des niveaux vérifiées avec le simulateur RAM. Validation sur le panneau physique à conserver pour le livrable du 26 novembre. |

| Mettre en place la communication MQTT du panneau RAM : publier les états et recevoir les commandes. | Jeudi 19 novembre | en cours | — | Publication des niveaux et de la température et réception de certaines commandes testées. Il reste à intégrer et vérifier toutes les commandes et tous les états nécessaires au fonctionnement du panneau. |

| Établir une première communication série avec le Power Meter. | Jeudi 19 novembre | à faire  | — | Appareil non disponible et protocole série absent de l'énoncé. Nouvelle date visée : 3 décembre 2026, sous réserve de l'accès au matériel et au protocole; signaler le blocage au professeur. Une simulation MQTT ne remplace pas une lecture série réelle. |

| Programmer la régulation PID du niveau des deux bassins du panneau RAM. | Jeudi 26 novembre | en cours | — | Réaliser et tester les deux calculs PID, puis les intégrer aux commandes du panneau avec sorties bornées et comportement sécuritaire. |

| Détecter les débordements et dépassements de niveau, transmettre les alarmes et tester les sécurités du panneau RAM. | Jeudi 26 novembre | à faire | — | Livrable obligatoire : alarmes et systèmes de secours fonctionnels et démontrables, y compris en cas de panne de communication. |

| Faire fonctionner les modes MANUEL et AUTOMATIQUE et le changement de consigne de niveau. | Jeudi 26 novembre | en cours | — | Réception et mémorisation logicielle commencées. Reste à commander réellement la pompe et les valves selon le mode et à synchroniser les états locaux et MQTT. |

| Vérifier le fonctionnement du PID de niveau lorsqu'une purge est ouverte. | Jeudi 26 novembre | à faire | — | Démontrer la réaction de la régulation lorsqu'une purge est ouverte. Ne comprend pas la programmation de l'unité distincte des valves de purge (section 7). |

| Documenter les premiers essais de niveau et les réglages du PID. | Jeudi 26 novembre | à faire | — | Décrire la méthode de réglage, les gains essayés, les résultats et conserver une courbe pour chaque choix de gains. |

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
