# Complément aux essais et à la maintenance — 25 novembre 2026

Ce complément remplace les attentes antérieures contradictoires concernant l'arrêt automatique de la pompe et les alarmes de niveau. Les résultats matériels sont maintenant consignés dans [resultats_essais_26nov.md](resultats_essais_26nov.md). Ce document est conservé comme complément historique; le tableau ci-dessous décrit les attentes avant les essais.

Référence : énoncé PFI, p. 5, 11–12, 23–24.

**Avant tout essai réel :** vérifier avec le responsable du banc que la pompe peut rester active lorsque les deux valves sont fermées (recirculation ou protection adaptée). Vérifier le sens physique des valves et l'arrêt d'urgence. Ne pas provoquer de débordement réel pour tester le contact ; utiliser le simulateur ou une méthode approuvée par le professeur.

| Essai | Attendu | Résultat réel |
|---|---|---|
| AUTO, consigne 10 %, pompe autorisée | PID reste actif à proximité de la consigne ; pompe ON | À vérifier |
| AUTO, pompe OFF | Pompe OFF, valves fermées | À vérifier |
| Seuil de niveau dépassé pendant moins de TG | Pas de notification | À vérifier |
| Seuil dépassé pendant TG | ON retained ; avertissement sans arrêt des actionneurs | À vérifier |
| ACK retained | Message vide retained envoyé sur le même topic | À vérifier |
| Défaut persistant après ACK pendant TR | Nouvelle notification ON retained | À vérifier |
| Débordement simulé | Commande d'arrêt de la pompe et de fermeture des valves sans attendre TG | À vérifier |
| Réarmement | Refus si défaut critique persistant ; sinon reprise en manuel sans redémarrage automatique | À vérifier |
| Perte série ou MQTT | Mise en sécurité demandée ; vérifier effectivement les sorties et employer l'arrêt matériel si nécessaire | À vérifier |

TG et TR sont configurables par MQTT, valeurs initiales 3 et 10 secondes. Le seuil est configurable de 0 à 100 %. La limite de sécurité logicielle à 100 % est conservée par précaution et doit être confirmée par le professeur. Sans vidange, une baisse de consigne ne fait pas redescendre activement le niveau. Les mesures des valves de purge ne sont pas nécessaires au PID.

## Validation logicielle de cette correction

Les quatre exécutables de tests C++ ont été compilés et exécutés indépendamment de CMake avec assertions actives : PID, dérivée, régulation et compatibilité sans purge (incluant désormais des assertions de temporisation/ACK/répétition). Depuis cette version, compilation WSL/ARM et essais physiques réalisés; consulter les résultats détaillés. L’ajout final MQTT des consignes reste non recompilé.