# Optimisation des PID — Panneau RAM

**PFI — Programmation temps réel | 27 novembre 2026**

## 1. Objectif

Le but des essais est d’améliorer la régulation du niveau du gros bassin (GB) et du petit bassin (PB). On cherche à maintenir les niveaux à ±1 point de pourcentage de leur consigne, tout en réduisant les oscillations, les dépassements et les creux lors des changements de consigne. Les deux bassins partagent une pompe, ce qui peut créer des interactions hydrauliques.

## 2. Méthode expérimentale

Les acquisitions sont enregistrées sur le Raspberry Pi dans `logs/pid_niveaux.csv`, puis sauvegardées séparément pour chaque essai. Le fichier initial `pid_niveaux.csv` correspond à l’essai 00; les fichiers `pid_essai_01.csv` à `pid_essai_07.csv` correspondent aux essais suivants. Les copies des données originales sont fournies dans `captures/pid/`.

Les 36 colonnes permettent notamment de suivre le temps, le pas réel et le pas PID, les consignes, les niveaux, les erreurs, les commandes PID, les ouvertures mesurées, les gains, la pompe et les alarmes. Le calcul des performances utilise uniquement les lignes où `regulation_autorisee=1` et `pompe_appliquee=1`. Le pas médian est d’environ 1,02 s. Les mesures de valves précèdent l’application de la commande PID du cycle; l’état de pompe est logiciel.

Pour les essais 00 à 06, le protocole consiste à modifier les gains et à observer les transitions entre 50/30 %, 30/30 %, 30/60 % et 30/30 % (GB/PB), selon les transitions effectivement enregistrées. L’essai 00 ne comprend pas la dernière descente PB. L’essai 07 maintient les deux consignes à 30 %. Les phases ne durent pas toutes aussi longtemps et l’essai 01 comporte une interruption de la régulation.

## 3. Comparaison des gains

| Essai | GB (Kp / Ki / Kd) | PB (Kp / Ki / Kd) | Mesures totales | Mesures actives | Durée totale (s) |
|---|---|---|---:|---:|---:|
| 00 | 5 / 0.7 / 2 | 2 / 0.18 / 1 | 294 | 94 | 299,8 |
| 01 | 5 / 0.7 / 2.5 | 2 / 0.22 / 1 | 198 | 131 | 200,9 |
| 02 | 5 / 0.7 / 3 | 2 / 0.22 / 1.5 | 163 | 114 | 165,3 |
| 03 | 5 / 0.7 / 3 | 2 / 0.22 / 2 | 157 | 109 | 159,1 |
| 04 | 5 / 0.7 / 3 | 2 / 0.22 / 2.5 | 181 | 96 | 183,3 |
| 05 | 5 / 0.7 / 3 | 1.8 / 0.22 / 2 | 139 | 92 | 140,9 |
| 06 | 5 / 0.7 / 3 | 2 / 0.22 / 2 | 150 | 102 | 151,9 |
| 07 | 5 / 0.7 / 3 | 2 / 0.22 / 2 | 101 | 46 | 101,8 |

Les gains sont lus dans les CSV pendant la régulation, et non seulement à partir des commandes prévues.

## 4. Comparaison des performances

Le creux représente l’écart maximal sous la nouvelle consigne de 30 % après une descente, jusqu’à la fin des mesures actives à cette consigne. Il peut donc inclure des perturbations provoquées par l’autre bassin. L’erreur absolue moyenne (EAM) finale est calculée sur les dix dernières mesures actives de chaque essai. Les consignes finales ne sont pas toutes identiques (PB à 60 % pour l’essai 00).

| Essai | Creux GB (pts) | Creux PB (pts) | EAM GB (pts) | EAM PB (pts) | GB dans ±1 pt (/10) | PB dans ±1 pt (/10) |
|---|---:|---:|---:|---:|---:|---:|
| 00 | 7,93 | — | 0,209 | 1,096 | 10 | 5 |
| 01 | 3,46 | 9,59 | 0,244 | 1,234 | 10 | 4 |
| 02 | 3,18 | 6,31 | 0,076 | 0,770 | 10 | 8 |
| 03 | 2,98 | 4,76 | 0,197 | 1,593 | 10 | 3 |
| 04 | 1,68 | 5,43 | 0,491 | 2,654 | 10 | 2 |
| 05 | 5,02 | 5,18 | 0,151 | 1,618 | 10 | 3 |
| 06 | 5,37 | 5,19 | 0,114 | 0,641 | 10 | 7 |
| 07 | — | — | 0,218 | 0,506 | 10 | 10 |

— : transition absente du fichier. Les tableaux détaillés par phase sont disponibles dans [`phases.csv`](captures/pid/phases.csv). La synthèse numérique est disponible dans [`synthese.csv`](captures/pid/synthese.csv).

## 5. Analyse de chaque essai

### Essai 00

Référence initiale. Le GB suit bien la montée à 50 %, mais descend trop bas après la réduction de consigne. Un débordement GB est enregistré plus tard, hors régulation automatique.

Derniers niveaux actifs : GB 29,88 %, PB 59,81 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 8,5 %, PB 9,6 %.

![Essai 00 — niveau GB](captures/pid/essai_00_GB_niveau.png)

![Essai 00 — valve GB](captures/pid/essai_00_GB_valve.png)

![Essai 00 — niveau PB](captures/pid/essai_00_PB_niveau.png)

![Essai 00 — valve PB](captures/pid/essai_00_PB_valve.png)

### Essai 01

Augmentation du Kd du GB et du Ki du PB. Le creux GB diminue par rapport à la référence. Le PB descend fortement sous sa consigne lors du retour à 30 %. Une interruption de pompe limite la comparabilité.

Derniers niveaux actifs : GB 30,03 %, PB 29,07 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 9,2 %, PB 6,9 %.

![Essai 01 — niveau GB](captures/pid/essai_01_GB_niveau.png)

![Essai 01 — valve GB](captures/pid/essai_01_GB_valve.png)

![Essai 01 — niveau PB](captures/pid/essai_01_PB_niveau.png)

![Essai 01 — valve PB](captures/pid/essai_01_PB_valve.png)

### Essai 02

Augmentation des deux Kd. Le creux PB diminue, et la précision finale du GB est bonne.

Derniers niveaux actifs : GB 30,18 %, PB 29,41 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 6,1 %, PB 7,9 %.

![Essai 02 — niveau GB](captures/pid/essai_02_GB_niveau.png)

![Essai 02 — valve GB](captures/pid/essai_02_GB_valve.png)

![Essai 02 — niveau PB](captures/pid/essai_02_PB_niveau.png)

![Essai 02 — valve PB](captures/pid/essai_02_PB_valve.png)

### Essai 03

Augmentation du Kd PB à 2. Le creux PB est le plus faible des essais comprenant une descente PB.

Derniers niveaux actifs : GB 30,24 %, PB 29,29 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 6,4 %, PB 7,3 %.

![Essai 03 — niveau GB](captures/pid/essai_03_GB_niveau.png)

![Essai 03 — valve GB](captures/pid/essai_03_GB_valve.png)

![Essai 03 — niveau PB](captures/pid/essai_03_PB_niveau.png)

![Essai 03 — valve PB](captures/pid/essai_03_PB_valve.png)

### Essai 04

Augmentation du Kd PB à 2,5. La descente PB et la précision finale se dégradent par rapport à l’essai 03.

Derniers niveaux actifs : GB 30,40 %, PB 28,00 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 7,3 %, PB 8,3 %.

![Essai 04 — niveau GB](captures/pid/essai_04_GB_niveau.png)

![Essai 04 — valve GB](captures/pid/essai_04_GB_valve.png)

![Essai 04 — niveau PB](captures/pid/essai_04_PB_niveau.png)

![Essai 04 — valve PB](captures/pid/essai_04_PB_valve.png)

### Essai 05

Retour au Kd PB de 2 et réduction du Kp à 1,8. Le creux PB ne s’améliore pas par rapport à l’essai 03.

Derniers niveaux actifs : GB 29,95 %, PB 29,02 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 7,6 %, PB 7,6 %.

![Essai 05 — niveau GB](captures/pid/essai_05_GB_niveau.png)

![Essai 05 — valve GB](captures/pid/essai_05_GB_valve.png)

![Essai 05 — niveau PB](captures/pid/essai_05_PB_niveau.png)

![Essai 05 — valve PB](captures/pid/essai_05_PB_valve.png)

### Essai 06

Reprise des gains de l’essai 03. Les niveaux finaux sont proches de 30 %, mais les creux varient malgré des gains identiques, ce qui limite l’attribution des différences aux seuls PID.

Derniers niveaux actifs : GB 30,08 %, PB 29,97 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 6,9 %, PB 7,8 %.

![Essai 06 — niveau GB](captures/pid/essai_06_GB_niveau.png)

![Essai 06 — valve GB](captures/pid/essai_06_GB_valve.png)

![Essai 06 — niveau PB](captures/pid/essai_06_PB_niveau.png)

![Essai 06 — valve PB](captures/pid/essai_06_PB_valve.png)

### Essai 07

Validation courte à consignes constantes de 30 %. Les dix dernières mesures de chaque bassin respectent la tolérance de ±1 point.

Derniers niveaux actifs : GB 29,84 %, PB 29,48 %. Fraction des sorties PID à saturation haute (≥ 99,99 %) : GB 8,7 %, PB 0,0 %.

![Essai 07 — niveau GB](captures/pid/essai_07_GB_niveau.png)

![Essai 07 — valve GB](captures/pid/essai_07_GB_valve.png)

![Essai 07 — niveau PB](captures/pid/essai_07_PB_niveau.png)

![Essai 07 — valve PB](captures/pid/essai_07_PB_valve.png)

## 6. Gains retenus

| Coefficient | GB | PB |
|---|---:|---:|
| Kp | 5,0 | 2,0 |
| Ki | 0,7 | 0,22 |
| Kd | 3,0 | 2,0 |

Les gains retenus constituent un compromis entre la rapidité, le creux en descente et la précision finale. Le Kd PB de 2 est préféré à 2,5, et le Kp de 2 est préféré à 1,8 sur la base des essais disponibles. Les résultats ne démontrent pas une optimisation globale.

### Validation à 30 % (essai 07)

| Indicateur | GB | PB |
|---|---:|---:|
| Niveau final (%) | 29,84 | 29,48 |
| Plage des dix dernières mesures (%) | 29,58 à 30,42 | 29,22 à 29,74 |
| EAM finale (pt) | 0,218 | 0,506 |
| Dernière entrée dans ±1 pt (s) | 21,46 | 35,77 |
| Maintien observé après cette entrée (s) | 24,54 | 10,22 |

Le temps est mesuré à partir de la première ligne de régulation active. La dernière entrée dans la bande correspond au premier échantillon suivant le dernier échantillon hors tolérance; le maintien est mesuré jusqu’à la dernière ligne active.

## 7. Intégration et limites

Les valeurs retenues sont à utiliser dans le constructeur de `RegulationBassins.cpp` et dans l’aide de `main.cpp`. La compilation et les tests ont été confirmés par l’utilisateur, mais les sorties de compilation ne sont pas annexées à ce rapport.

La régulation de l’essai 07 a duré environ 46 s et non cinq minutes. Il reste donc à effectuer la validation prolongée, avec les mêmes positions de purge, des phases de durée comparable, le panneau surveillé et les sécurités actives. Les positions de purge ne sont pas enregistrées dans ces CSV; on ne peut pas les supposer identiques. Les variations de niveau entre essais à gains identiques montrent l’importance de la répétabilité. Les graphiques représentent les données mesurées, sans interpolation des interruptions ni ajout de courbes théoriques.

## 8. Conclusion

Les essais ont permis de sélectionner des gains plus adaptés au panneau réel. Les deux bassins atteignent la tolérance de ±1 point en fin de l’essai 07. Les résultats sont satisfaisants à court terme, mais la stabilité prolongée et la répétabilité des transitions restent à valider.
