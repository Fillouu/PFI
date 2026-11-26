# Résultats des essais du 25 novembre 2026 — panneau RAM

## Réglages retenus

| Bassin | Kp | Ki | Kd | Limites |
|---|---:|---:|---:|---|
| GB | 5,0 | 0,70 | 2,0 | 0–100 % |
| PB | 2,0 | 0,18 | 1,0 | 0–100 % |

### Essais de réglage à 20 %

| Bassin | Kp | Ki | Kd | Observation |
|---|---:|---:|---:|---|
| GB | 7,5 | 1,0 | 2,0 | 12–24 % |
| GB | 5,0 | 0,3 | 2,0 | Maximum 17 % |
| GB | 5,0 | 0,6 | 2,0 | Atteint 20 %, lent |
| GB | 6,0 | 0,6 | 2,0 | Trop agressif |
| GB | 5,5 | 0,6 | 2,0 | Manque de précision |
| GB | 5,0 | 0,7 | 2,0 | Retenu |
| PB | 3,0 | 0,4 | 1,0 | 8–26 % |
| PB | 1,5 | 0,15 | 1,0 | Trop lent |
| PB | 2,0 | 0,20 | 1,0 | Atteint 20 %, stabilisation lente |
| PB | 2,0 | 0,20 | 1,5 | 16–22 % |
| PB | 2,0 | 0,18 | 1,0 | Retenu |

## Résultats observés

| Essai | Résultat |
|---|---|
| Commandes manuelles et calibration GB/PB | Fonctionnelles |
| Régulation simultanée et changement de consigne | Fonctionnels selon les essais |
| Consignes 30/25 % | Mesures 29,96/25,42 % sur une capture, pompe ON |
| Consignes 30/30 % | Mesures 29,78/29,47 % sur une capture, pompe ON |
| Alarme niveau | ON, sans arrêt du PID ou de la pompe |
| ACK et répétition | ACK, message vide, puis ON si défaut persistant |
| MQTT modes, valves, consignes | Réception observée |
| MQTT états niveaux, température, valves et pompe | Publications observées |
| Arrêt logiciel | Valves ramenées à environ 0 %, verrouillage observé ; pompe déjà OFF |
| Perte MQTT | Arrêt du programme, code retour 0 ; fermeture physique au moment du défaut non confirmée |
| Défaut série au démarrage | Absence de réponse détectée lors de certains démarrages |

Les mesures ponctuelles ne constituent pas une courbe ni une preuve de stabilité sur une période prolongée. Ajouter les courbes expérimentales et les temps mesurés sans inventer de données.

## Essais complémentaires (18 à 28)

| Test | Résultat |
|---|---|
| 18 | Arrêt logiciel affiché; GB 0 %, PB 0,70 %, pompe OFF |
| 19–20 | Commandes bloquées; refus du réarmement logiciel et message explicite |
| 21 | Redémarrage en manuel, consignes nulles, pompe OFF |
| 22 | Perte MQTT détectée; message conservé et commandes de sécurité acceptées |
| 23–24 | Alarmes de niveau sans coupure; ACK puis répétition sur les deux bassins |
| 25 | Génération d'alarme cohérente avec TG = 3 s |
| 26–27 | Débordements physiques GB/PB détectés et publiés |
| 28 | Réarmement et reprise manuelle confirmés par l'opérateur |

### Temporisations

| Bassin | Franchissement apparent | ON | Délai |
|---|---|---|---|
| GB | 01:19:48 (6 %) | 01:19:51 | ≈ 3 s |
| PB | 01:20:04–08 (passage de 5 à 6 % publiés) | 01:20:08 | compatible avec 3 s, non mesuré précisément |

Le seuil des deux alarmes était de 5 %. Les valeurs MQTT sont arrondies et publiées sur changement; les horodatages ont une résolution d'une seconde. Les deux ON reçus à l'abonnement à 01:19:12 sont vraisemblablement des messages retained antérieurs. Le test de répétition a montré une nouvelle publication ON environ 10 s après ACK pour les deux bassins. Sans ACK, l'alarme reste émise et ne se répète pas.

![Niveaux lors du test de génération](courbes/regulation_15.png)

### Débordements

| Bassin | Heure ON | Niveau publié au déclenchement | Maximum publié | Premier retour à 0 % |
|---|---|---|---|---|
| GB | 01:32:45 | 97 % | 98 % | 01:33:05 (20 s) |
| PB | 01:34:47 | 91 % | 91 % | 01:34:57 (10 s) |

![Débordement GB](courbes/debordement_GB.png)

![Débordement PB](courbes/debordement_PB.png)

Les journaux MQTT montrent les niveaux et les alarmes, mais pas les positions des valves ni l'état de la pompe pendant ces débordements. Le message de défaut demandant `rearme` et le réarmement fonctionnel ont été confirmés par l'opérateur. Les données originales utilisées pour les courbes sont dans `courbes/*.csv`; elles ne sont pas rééchantillonnées. Les segments relient les publications successives et ne représentent pas une mesure continue.

## À valider / limites

- Perte série en fonctionnement : non testée physiquement. Le code tente la mise en sécurité et affiche un avertissement si celle-ci n'est pas confirmée.
- Confirmation physique des positions et de la pompe au moment exact de la perte MQTT et des débordements : non enregistrée dans les journaux fournis.
- Configuration TG/TR par MQTT, commande pompe à distance, cas de capteur invalide et refus de réarmement pendant le défaut : essais spécifiques non documentés.
- Publication des consignes locales : ajoutée aux sources après les essais, pas recompilée ni testée à la demande de l'étudiant.
- Courbes de chaque combinaison de gains PID : non enregistrées; seules les observations du tableau et les séries MQTT disponibles sont rapportées.

Les états MQTT retained peuvent être périmés après une déconnexion. Les quatre tests unitaires et les compilations WSL/ARM ont été confirmés avant l'ajout final de synchronisation MQTT.