
# Réglages du PID — panneau RAM

## But

Réguler indépendamment le niveau du grand bassin (GB) et du petit bassin (PB) par deux PID, en gardant la **pompe commune** en marche lorsque l'une des valves de remplissage doit s'ouvrir. Les valeurs de consigne sont des niveaux de bassin en pourcentage et **non** des pourcentages d'ouverture des valves.

## Réglages logiciels actuels (à valider sur le simulateur puis au laboratoire)

| Bassin | Kp | Ki | Kd | Limite de sortie |
|---|---:|---:|---:|---:|
| Grand bassin | 4,0 | 0,03 | 0 | 0 à 50 % |
| Petit bassin | 2,0 | 0,02 | 0 | 0 à 25 % |

Ces gains et limites existaient dans le code reçu. Ils ne sont **pas validés sur le panneau réel**. Une petite hystérésis de pompe évite d'attendre un écart de niveau trop élevé avant de réagir. Le calcul se fait avec le temps réellement écoulé entre deux itérations.

- `gains gb 4 0.03 0` : modifier les trois gains du GB sans relancer le programme.
- `gains pb 2 0.02 0` : modifier les gains du PB sans relancer le programme.
- Les gains doivent être finis et entre 0 et 100. Il faut démarrer par des gains conservateurs et enregistrer les courbes avant toute augmentation.

## Comportement des valves de purge

Les valves de purge sont les **sorties d'eau**. Les valves de remplissage sont les **sorties de commande du Pi**. Elles ne doivent pas être confondues.

- Purge fermée, niveau en dessous de la consigne : le PID remplit le bassin jusqu'à la consigne.
- Purge fermée et niveau à moins de 0,5 point de la consigne : la valve de remplissage du bassin ferme complètement. Si les deux bassins sont satisfaits, la pompe s'arrête.
- Purge ouverte : la boucle PID compense la baisse de niveau lorsque la sortie d'eau crée une erreur. Elle ne peut pas maintenir une consigne si la vidange dépasse le débit maximum de remplissage.
- Lorsque l'ouverture des purges est connue par MQTT, le programme reçoit `RAM/valves/etats/Ouverture_GB` et `RAM/valves/etats/Ouverture_PB`. Si l'unité de purge est absente, la régulation peut encore fonctionner à partir des mesures de niveau, mais **la fermeture physique des purges n'est alors pas confirmée**.

## Essais et méthode de réglage

1. Vérifier l'absence d'alarmes et effectuer un essai à une consigne faible, par exemple 10 % (ne pas commencer directement à 75 % sur le banc).
2. Essayer uniquement P (`Ki=0, Kd=0`), observer la montée et le dépassement, et enregistrer la courbe niveau-consigne-commande.
3. Ajouter un faible `Ki` pour corriger l'erreur permanente lors de l'ouverture d'une purge.
4. Tester un changement de consigne et l'ouverture/fermeture des purges. Ajuster les gains prudemment : le D est calculé sur la mesure pour éviter un à-coup lors d'un changement de consigne.
5. Vérifier le retour à 0 % des valves de remplissage et l'arrêt de la pompe lors de l'atteinte des consignes, en situation sans vidange.
6. Mesurer le temps de réponse, le dépassement maximal et l'erreur stationnaire. Les inscrire dans le rapport avec des captures/courbes.

**Résultat actuel :** tests unitaires validés en C++ ; performances dynamiques, précision ±1 % et comportement du matériel **non mesurés**.

**Attention :** une consigne de 95 % ou plus entre en conflit avec le seuil de protection existant (`niveau >= 95 %`). Ne pas utiliser ces consignes tant que le seuil de sécurité n'a pas été revu et validé avec le professeur.
