# Planification de la PFI — Programmation temps réel
// Chat GPT a aider a la conception de la Planification 

**Cours :** 247-GFB-LG — Programmation temps réel  
**Session :** Automne 2026  
**Portée de ce plan :** section 3 — Unité de contrôle du panneau R.A.M. et section 6 — Unité de contrôle du Power Meter  
**Remise de la planification :** jeudi 12 novembre 2026, à la fin du laboratoire  
**Remise finale du dépôt :** vendredi 11 décembre 2026, avant 23 h

## Tableau de planification

| Livrable | Date prévue | État | Date réelle | Note |
|---|---|---|---|---|


| Déposer le fichier `planification.md` dans le dépôt GitHub. | Jeudi 12 novembre |  Fait | 12 Novembre 2026 | Premier livrable obligatoire. |

| Organiser les dossiers et vérifier la compilation C++ des programmes panneau RAM et Power Meter. | Jeudi 12 novembre |  Fait | — | Préparer une démonstration et des commits distincts. |

| Faire communiquer le Raspberry Pi avec le panneau RAM et lire les niveaux des deux bassins. | Jeudi 19 novembre | fait  | — | Afficher localement les valeurs acquises. |

| Mettre en place la communication MQTT du panneau RAM : publier les états et recevoir les commandes. | Jeudi 19 novembre | à faire | — | Respecter les topics définis à l'annexe I. |

| Établir une première communication série avec le Power Meter. | Jeudi 19 novembre | à faire | — | Vérifier la lecture d'au moins une donnée réelle. |

| Programmer la régulation PID du niveau des deux bassins du panneau RAM. | Jeudi 26 novembre | à faire | — | Réguler les deux niveaux avec des sorties de commande bornées. |

| Détecter les débordements et dépassements de niveau, transmettre les alarmes et tester les sécurités du panneau RAM. | Jeudi 26 novembre | à faire | — | Livrable obligatoire du 26 novembre : alarmes et systèmes de secours fonctionnels. |

| Faire fonctionner les modes MANUEL et AUTOMATIQUE et le changement de consigne de niveau. | Jeudi 26 novembre | à faire | — | Commandes locales et MQTT cohérentes; pompe et valves selon le mode. |

| Vérifier le fonctionnement du PID de niveau lorsqu'une purge est ouverte. | Jeudi 26 novembre | à faire | — | Essai avec purge du panneau; ne comprend pas la programmation de l'unité distincte des valves de purge (section 7). |

| Documenter les premiers essais de niveau et les réglages du PID. | Jeudi 26 novembre | à faire | — | Conserver les résultats et les observations dans le dépôt. |

| Réguler la température du petit bassin en maintenant le contrôle de niveau. | Jeudi 3 décembre | à faire | — | Tester une consigne initiale et un changement de consigne. |

| Finaliser l'affichage local et la synchronisation des commandes du panneau RAM. | Jeudi 3 décembre | à faire | — | Vérifier les changements depuis le clavier SSH et le superviseur MQTT. |

| Lire et afficher les mesures du Power Meter (tensions, courants, puissance, énergie, facteur de puissance). | Jeudi 3 décembre | à faire | — | Confirmer les valeurs et unités selon l'appareil et le protocole fourni. |

| Publier les mesures du Power Meter sur les topics MQTT prévus. | Jeudi 3 décembre | à faire | — | Publier les changements selon l'annexe I. |

| Détecter la perte de communication avec le Power Meter et émettre l'alarme correspondante. | Jeudi 3 décembre | à faire | — | Préparer un essai de déconnexion/reconnexion. |

| Revoir la structure du code C++ des deux unités (SOLID, The Power of 10, machines à états si pertinentes). | Jeudi 10 décembre | à faire | — | Vérifier la lisibilité et les réactions attendues. |

| Rédiger les documents de conception, de réglage et les plans d'essai nécessaires pour les deux unités. | Jeudi 10 décembre | à faire | — | Documents courts accompagnant le code, selon les attentes du professeur. |

| Rédiger la documentation de maintenance du panneau RAM et celle du Power Meter. | Jeudi 10 décembre | à faire | — | Mise en marche, essais sécuritaires, limites, pannes et réaction attendue. |

| Faire les essais de démonstration et de pannes des deux unités. | Jeudi 10 décembre | à faire | — | Tester les scénarios applicables : communication, capteurs, débordement, commandes, arrêt et relance. |

| Vérifier les fichiers, compiler les deux programmes et pousser le travail final sur GitHub. | Vendredi 11 décembre | à faire | — | Remise finale avant 23 h. |

## Mise à jour chaque jeudi

- Avant le laboratoire du jeudi, mettre la planification à jour et pousser les changements sur GitHub (consigne : **avant 8 h**).
- **Ne jamais changer la date prévue** : cette date est fixée dans la planification initiale du 12 novembre.
- Utiliser les états : `à faire`, `en cours`, `fait`, `en retard`.
- Lorsqu'un livrable est terminé, indiquer la date d'achèvement dans **Date réelle**.
- En cas de retard, écrire la cause et la nouvelle date visée dans **Note**, sans modifier **Date prévue**.
- Un livrable `fait` doit être visible dans le dépôt et démontrable lorsqu'il s'agit d'une fonctionnalité.

## Préparation des suivis

| Date | Démonstration / revue prévue |
|---|---|


| Jeudi 12 novembre | Planification ajoutée au dépôt GitHub. |
| Jeudi 19 novembre | Communication panneau RAM, acquisition des niveaux, premiers échanges MQTT et lecture série du Power Meter. |
| Jeudi 26 novembre | Livrable obligatoire : PID de niveau des deux bassins, modes, changement de consigne, essai avec purge, alarmes et sécurités. |
| Jeudi 3 décembre | Température du petit bassin, synchronisation, mesures MQTT du Power Meter et alarme de perte de lien. |
| Jeudi 10 décembre | Bilan des essais, revue de code et documents de maintenance des deux unités, en préparation de la remise. |

## Exemples de messages Git

Une seule idée par commit, dans une seule partie du système.

```text
planif : ajoute la planification des sections 3 et 6
panneau : lit les niveaux des deux bassins
panneau : publie les niveaux sur MQTT
pid : regule le niveau du grand bassin
pid : regule le niveau du petit bassin
panneau : synchronise les modes manuel et automatique
panneau : detecte les alarmes de debordement
powermeter : lit les mesures par liaison serie
powermeter : publie les valeurs sur MQTT
powermeter : detecte la perte de communication
maintenance : documente les essais du panneau RAM
maintenance : documente les pannes du Power Meter
```

## Limites de la planification

Ce fichier couvre **uniquement les sections 3 et 6** de l'énoncé d'intégration. Il ne planifie pas la programmation du superviseur Web (section 2), du mélangeur (section 4), de la balance (section 5), des capteurs des valves de purge (section 7), ni du système d'aspiration (section 8). Les interactions MQTT nécessaires aux deux unités sont incluses, mais la réalisation des autres unités n'est pas attribuée à ce plan.

**Références de travail :** grille d'évaluation PFI en Programmation temps réel (automne 2026), consignes « Planification de la PFI » (automne 2026), énoncé *Système de contrôle du panneau RAM* (révision automne 2025), sections 3, 6 et annexe I. En cas de différence, suivre les consignes et la grille actuelles du professeur.