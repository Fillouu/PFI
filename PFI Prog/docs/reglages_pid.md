# Réglages du PID — panneau RAM

## But

Réguler indépendamment le niveau du grand bassin (GB) et du petit bassin (PB) par deux PID. Les consignes représentent les niveaux d'eau en pourcentage, et non l'ouverture des valves. La pompe commune reste active en AUTO lorsqu'elle est autorisée et que les conditions de sécurité sont respectées.

## Réglages retenus sur le panneau réel

| Bassin | Kp | Ki | Kd | Limite de sortie |
|---|---:|---:|---:|---:|
| Grand bassin | 5,0 | 0,70 | 2,0 | 0 à 100 % |
| Petit bassin | 2,0 | 0,18 | 1,0 | 0 à 100 % |

Les gains sont intégrés au constructeur de `RegulationBassins`. Le calcul utilise le temps écoulé entre les itérations; la dérivée est calculée sur la mesure.

- `gains gb 5 0.7 2` : modifier les gains GB sans redémarrage.
- `gains pb 2 0.18 1` : modifier les gains PB sans redémarrage.

## Comportement des valves de purge

Les valves de remplissage sont commandées par le Pi. Les valves de purge servent à la sortie de l'eau. Les mesures de position des purges ne sont pas requises par le PID. Lorsque le niveau diminue, le PID peut augmenter la commande de remplissage pour compenser. Lorsque le niveau dépasse la consigne, il réduit la commande; la baisse du niveau dépend de l'écoulement physique. Une consigne nulle force la fermeture de la valve correspondante. La pompe ne s'arrête pas automatiquement à l'atteinte des consignes en AUTO; le circuit doit être adapté à ce fonctionnement.

## Essais et méthode de réglage

Les gains ont été ajustés progressivement à une consigne de 20 %, en observant la vitesse de réponse, les oscillations et la précision. Les essais à deux bassins ont ensuite permis de vérifier des changements de consignes.

| Consigne GB/PB | Niveau GB/PB observé | Écart signé GB/PB |
|---|---|---|
| 30 / 25 % | 29,96 / 25,42 % | -0,04 / +0,42 point |
| 30 / 30 % | 29,78 / 29,47 % | -0,22 / -0,53 point |

Ces mesures sont ponctuelles et ne prouvent pas une précision durable de ±1 %. Les observations de chaque combinaison de gains figurent dans [les résultats des essais](resultats_essais_26nov.md). Les courbes disponibles proviennent du journal MQTT à 15 %; elles ne remplacent pas les courbes manquantes de chaque réglage intermédiaire.

![Réponse des niveaux lors de l'essai à 15 %](courbes/regulation_15.png)

**Attention :** le code considère un niveau non fini, inférieur à -2 % ou supérieur ou égal à 100 % comme invalide. Les tiges de débordement déclenchent indépendamment la sécurité. Ne pas utiliser de consignes proches du débordement en fonctionnement normal.