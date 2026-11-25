
# Plan d'essai — panneau RAM

État au 23 novembre 2026 : **scénarios à exécuter sur le simulateur et sur le panneau, et non résultats de laboratoire**. Les tests purement logiciels sont indiqués séparément.

## Essais logiciels déjà réalisés

| Essai | Résultat local |
|---|---|
| Tests PID existants (`test_pid.cpp`) | Réussi |
| Tests régulation existants (`test_regulation_bassins.cpp`) | Réussi |
| Tests purges/consignes/sécurités (`test_regulation_purge.cpp`) | Réussi |
| Test dérivée sur mesure (`test_pid_derivation.cpp`) | Réussi |
| Analyse syntaxique de `src/panneau/main.cpp` | Réussie avec un en-tête MQTT de remplacement ; **pas de liaison libmosquitto ni d'essai physique** |

## À vérifier sur le simulateur

| Scénario | Commandes ou manipulations | Résultat attendu | Résultat réel |
|---|---|---|---|
| Mode manuel GB | `manuel`, puis `valve gb 50` | Valve GB à 50 %, pompe ON, niveau monte | À vérifier |
| Fermeture GB | `valve gb 0` | Valve GB à 0 %, pompe OFF si aucune autre valve ouverte | À vérifier |
| Manuel PB | `valve pb 40`, puis `valve pb 0` | Le PB monte, puis sa valve ferme | À vérifier |
| Arrêt pompe | `pompe off` avec valve ouverte | La pompe s'arrête ; `pompe on` la réautorise | À vérifier |
| Régulation AUTO | `consigne gb 50`, `consigne pb 75`, `auto` | Deux niveaux convergent vers les consignes sans débordement | À vérifier |
| Purges fermées | Arrêter les purges une fois les consignes atteintes | Commandes de remplissage à 0 %, pompe OFF | À vérifier |
| Purge GB ouverte | Ouvrir la purge GB | Niveau GB baisse, PID GB rouvre pour compenser | À vérifier |
| Purge PB ouverte | Ouvrir la purge PB | PID PB compense sans perturber le GB | À vérifier |
| Changement consigne | `consigne gb 40` en AUTO | Le contrôleur réduit l'alimentation ; la descente dépend de la purge | À vérifier |
| Passage AUTO → MANUEL | `manuel` | Pompe et valves mises à 0 % avant reprise manuelle | À vérifier |
| Arrêt logiciel | `arret` | Mise en sécurité verrouillée, redémarrage nécessaire | À vérifier |
| Débordement / mesure invalide | Simulation contrôlée d'un défaut | Arrêt des actionneurs ; publier une alarme | À vérifier |
| Perte série | Déconnecter le lien sous supervision | Arrêt demandé, erreur signalée | À vérifier |
| Perte MQTT | Arrêter le broker de test | Comportement à caractériser ; protection additionnelle à prévoir | À vérifier |

## Test sur panneau réel

Ne jamais démarrer l'essai réel sans avoir vérifié la direction 4–20 mA des valves, les capteurs, l'arrêt d'urgence matériel, la calibration des niveaux et l'alimentation hydraulique. Être présent au banc, commencer à consigne et ouverture faibles, puis augmenter progressivement. Noter les valeurs réelles, les temps, les erreurs et les courbes.
