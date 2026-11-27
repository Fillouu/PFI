# Planification – PFI Automatisation industrielle

**Projet :** Granudoseur – Micro850, POINT I/O et HMI PanelView 800  
**Cours :** 247-GFC-LG – Automatisation industrielle  
**Début :** vendredi 27 novembre 2026  
**Démonstration et entretien oral :** jeudi 17 décembre 2026, en avant-midi  
**Remise sur Moodle :** jeudi 17 décembre 2026 à 23 h 30  
**Objectif :** compléter tous les critères obligatoires de la grille d’évaluation. Le bonus Modbus TCP sera réalisé seulement si le reste est terminé.

## 1. Calendrier de travail

Les cinq journées de travail prévues sont le 27 novembre ainsi que les 4, 8, 11 et 15 décembre. Le 17 décembre est réservé à la démonstration devant les enseignants et à la remise finale. Aucun développement n’est prévu en dehors de ces cinq journées.

| Date prévue | Étape | Résultat attendu | Date réelle | État |
|---|---|---|---|---|
| 2026-11-27 | Analyse et conception | GRAFCET et liste des variables préparés | — | À faire |
| 2026-12-04 | Configuration et programmation | POINT I/O, GPN, GMM et modes de marche fonctionnels | — | À faire |
| 2026-12-08 | Sécurité et HMI | GAU, interlocks, arrêt momentané et six pages HMI | — | À faire |
| 2026-12-11 | Gestion des données et intégration | Saisies, horodatage, historique et essais intégrés | — | À faire |
| 2026-12-15 | Validation finale | Essais finaux, rapport et livrables prêts | — | À faire |
| 2026-12-17 (AM) | Démonstration | Présentation et entretien devant les enseignants | — | À faire |
| 2026-12-17 (23 h 30 au plus tard) | Remise | Dépôt complet sur Moodle | — | À faire |

## 2. Tâches détaillées

### Vendredi 27 novembre – Analyse et conception

- [ ] Lire l’énoncé et analyser la grille d’évaluation.
- [ ] Identifier les entrées, les sorties, les actionneurs et les variables nécessaires.
- [ ] Relever, si le banc est disponible, les références des modules POINT I/O et les adresses IP.
- [ ] Dessiner le GRAFCET de production normale (GPN) à partir de la séquence de l’énoncé.
- [ ] Dessiner le GRAFCET des modes de marche (GMM).
- [ ] Dessiner le GRAFCET d’arrêt d’urgence (GAU).
- [ ] Prévoir la gestion de l’arrêt momentané, du redémarrage et du Reset.
- [ ] Prévoir les étapes de saisie des niveaux et du poids, ainsi que les interlocks.

**Résultat attendu :** conception prête à être traduite dans CCW.

### Vendredi 4 décembre – Configuration et programmation LD

- [ ] Créer le projet CCW et configurer le Micro850.
- [ ] Confirmer l’ordre des modules POINT I/O et l’adresse IP du 1734-AENT.
- [ ] Remplir le calculateur Rockwell 2080-RD001 et configurer les tailles d’assemblage EtherNet/IP.
- [ ] Mapper les entrées et les sorties POINT I/O.
- [ ] Programmer le GMM et les modes Manuel et Automatique en LD.
- [ ] Programmer le GPN en LD, incluant la sélection du réservoir le moins plein.
- [ ] Définir un traitement déterministe des niveaux égaux.
- [ ] Tester les GRAFCET dans le simulateur CCW et vérifier la communication POINT I/O sur le banc si disponible (LEDs et E/S), avec les précautions nécessaires.

**Résultat attendu :** critères 1 à 4 de la grille visés, sous réserve des essais matériels. Les sorties physiques ne sont pas activées sans les vérifications de sécurité nécessaires.

### Mardi 8 décembre – Sécurité et interface HMI

- [ ] Programmer le GAU en LD : arrêt et retour à l’étape initiale de tous les GRAFCET.
- [ ] Programmer Start, Stop, Reset, arrêt momentané et reprise de la séquence.
- [ ] Programmer le voyant Start (fixe en marche, clignotant en arrêt momentané).
- [ ] Programmer les interlocks de sécurité, de processus et de permission nécessaires.
- [ ] Tester les modes de marche et les interlocks dans le simulateur.
- [ ] Configurer le PanelView 800 (2711R-T4T) et sa communication avec le Micro850.
- [ ] Créer les pages Accueil, Mode Automatique, Mode Manuel, Saisie Niveaux, Diagnostic / Alarmes et Database Niveaux.
- [ ] Associer les tags HMI, les commandes, les voyants et les indications de mode.
- [ ] Programmer l’attente de saisie et la validation des niveaux (0 à 100 %) et du poids de la balance.

**Résultat attendu :** interface HMI et fonctions de sécurité programmées; intégration des saisies et des données à compléter au prochain bloc si nécessaire.

### Vendredi 11 décembre – Gestion des données et intégration

- [ ] Finaliser la saisie manuelle des niveaux et du poids, l’ouverture de la page de saisie à la demande du GPN et la reprise après validation.
- [ ] Programmer et tester l’horodatage des saisies.
- [ ] Créer le UDT `Enregistrement_Niveau` avec les niveaux A/B/C, le poids et l’horodatage.
- [ ] Programmer en ST l’historique FIFO des 10 derniers enregistrements avec `RETAIN`.
- [ ] Afficher les 10 enregistrements sur le HMI et vérifier le remplacement du plus ancien relevé.
- [ ] Compléter la page Diagnostic / Alarmes, incluant la perte de communication POINT I/O, l’arrêt d’urgence et les erreurs de séquence, avec historique des défauts.
- [ ] Tester l’intégration Micro850, POINT I/O et HMI sur le banc, après validation des interlocks et de la sécurité.
- [ ] Tester les modes de marche, la reprise, le Reset, le GAU, les saisies et la conservation de l’historique après redémarrage.
- [ ] Corriger les problèmes, optimiser la séquence et conserver les résultats d’essais.
- [ ] Prendre les captures d’écran du HMI et commencer le rapport technique.

**Résultat attendu :** critères techniques obligatoires visés; système intégré et prêt pour la validation finale.

### Mardi 15 décembre – Validation finale et préparation de la remise

- [ ] Terminer les corrections et les essais physiques restants.
- [ ] Vérifier chaque critère de la grille d’évaluation.
- [ ] Finaliser les GRAFCET et vérifier leur correspondance avec le code LD.
- [ ] Finaliser le rapport technique PDF (architecture, GRAFCET, code LD/ST, résultats des tests et difficultés rencontrées).
- [ ] Vérifier les captures d’écran des six pages HMI.
- [ ] Vérifier le fichier Excel Rockwell 2080-RD001.
- [ ] Sauvegarder le projet CCW complet et vérifier qu’il peut être rouvert.
- [ ] Préparer la démonstration du 17 décembre et les explications pour l’entretien oral.
- [ ] Regrouper tous les livrables dans une version prête à remettre.
- [ ] Si tous les critères obligatoires sont validés et qu’il reste du temps, envisager le bonus Modbus TCP.

**Résultat attendu :** projet terminé et prêt pour la présentation et la remise du 17 décembre.

### Jeudi 17 décembre – Démonstration et remise officielle

**En avant-midi :**

- [ ] Présenter le granudoseur fonctionnel devant les enseignants.
- [ ] Démontrer les modes Manuel et Automatique, le GPN, les fonctions d’arrêt, les interlocks, les saisies et l’historique.
- [ ] Répondre aux questions de l’entretien oral.
- [ ] Noter les commentaires des enseignants.

**Au plus tard à 23 h 30 :**

- [ ] Déposer les livrables sur Moodle.
- [ ] Vérifier que les bons fichiers sont déposés et que la remise est confirmée.

Aucune séance de programmation n’est prévue le 17 décembre.

## 3. Suivi de la grille d’évaluation

| Critère | Description | Objectif | État |
|---|---|---|---|
| 1 | Configuration POINT I/O fonctionnelle | Au moins 60 % | À faire |
| 2 | Modes Manuel et Automatique fonctionnels | Au moins 60 % | À faire |
| 3 | Traduction des GPN et GMM | Au moins 60 % | À faire |
| 4 | Identification du réservoir le moins plein | Au moins 60 % | À faire |
| 5 | Communication CCW et HMI T4T | 60–80 % | À faire |
| 6 | Interface HMI complète | 60–80 % | À faire |
| 7 | Saisie des niveaux, validation et horodatage | 60–80 % | À faire |
| 8 | Mode de Marche et Arrêt fonctionnel | 60–80 % | À faire |
| 9 | Interlocks implémentés et testés | 60–80 % | À faire |
| 10 | Gestion du GAU | 60–80 % | À faire |
| 11 | Arrêt momentané, redémarrage et Reset | 80–100 % | À faire |
| 12 | Database de suivi fonctionnelle | 80–100 % | À faire |
| 13 | Optimisation et choix méthodologiques | 80–100 % | À faire |
| 14 | Qualité du rapport et des GRAFCET | 80–100 % | À faire |
| 15 | Démonstration et entretien oral | 80–100 % | À faire |
| 16 | Communication Modbus TCP avec Raspberry Pi | Bonus | Non planifié |
| 18 | Sécurité au laboratoire | Éliminatoire si non respectée | À respecter en tout temps |

## 4. Livrables à remettre

- [ ] Projet CCW complet : Micro850, configuration POINT I/O et HMI PanelView.
- [ ] Rapport technique PDF.
- [ ] GRAFCET dessinés (papier scanné ou PDF numérique).
- [ ] Captures d’écran des pages HMI.
- [ ] Fichier Excel Rockwell 2080-RD001 rempli.
- [ ] Scripts Raspberry Pi, uniquement si le bonus est réalisé.

## 5. Méthode de suivi

- Conserver les dates prévues et inscrire les dates réelles lorsque les étapes sont réalisées.
- Utiliser les états : À faire, En cours, Terminé, Bloqué.
- Cocher les tâches uniquement après leur réalisation et leur validation lorsque applicable.
- Sauvegarder les versions du projet et conserver les preuves d’essais au fur et à mesure.
- Prioriser les critères obligatoires avant le bonus.
- Tester la logique en simulation avant les essais sur le banc. Le GAU logiciel et les interlocks programmés ne remplacent pas les dispositifs de sécurité physiques requis.
- Si une tâche prend du retard, la reporter sur la prochaine journée de travail prévue sans modifier la date initiale.

**Hypothèses de planification :** les heures disponibles et l’accès au matériel pour chaque journée ne sont pas encore confirmés. L’horaire précis de la démonstration du 17 décembre n’est pas fourni. Les objectifs sont des jalons de travail, pas des notes garanties.

**Référence :** énoncé *A26 – PFI – Granudoseur – CCW Micro850 PointIO HMI*, parties 1 à 7 et grille d’évaluation (pages 11 et 12).
