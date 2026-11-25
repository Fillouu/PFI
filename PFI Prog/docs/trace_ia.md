
# Délimitation et traçabilité des modifications faites avec l'aide d'une IA

**Date :** 23 novembre 2026.

Le travail de départ est celui du dépôt fourni par l'étudiant. Cette version ajoute ou adapte les éléments suivants avec l'assistance d'une IA :

- `src/panneau/main.cpp` : commandes locales non bloquantes, changement mode/consignes/valves/pompe, réglages PID au clavier, abonnement facultatif aux états des purges, suppression de la limitation au seul simulateur, amorçage en état sûr, publication des états de valves et pompe, synchronisation MQTT supplémentaire, prise en compte du temps réel des itérations. Sections ajoutées commentées `DEBUT ... AVEC L'AIDE D'UNE IA` / `FIN ...` ; les changements ponctuels (par ex. les limites manuelles et la pompe) font aussi partie de cette intervention.
- `include/panneau/Pid.h`, `src/panneau/Pid.cpp` : réglage dynamique des gains et dérivée calculée sur la mesure.
- `include/panneau/RegulationBassins.h`, `src/panneau/RegulationBassins.cpp` : paramètres facultatifs de purge, fermeture à consigne atteinte si purge fermée, réduction du seuil d'activation de la pompe, réglages des gains par bassin.
- `include/commun/TopicsMqtt.h` : états MQTT des deux purges.
- `CMakeLists.txt` et `tests/test_regulation_purge.cpp`, `tests/test_pid_derivation.cpp` : nouveaux tests logiciels automatiques.
- `docs/reglages_pid.md`, `docs/essais_panneau.md`, `docs/maintenance_panneau.md`, ce fichier et `README.md` : mise à jour de la documentation.

**Important pour la remise :** les passages assistés doivent rester identifiables conformément au plan de cours. Revoir les changements et leurs raisons, vérifier la conformité avec les consignes du professeur et ne pas présenter les tests de simulateur ou les essais matériels comme réalisés tant qu'ils ne l'ont pas été. Aucun commit ni push Git n'a été effectué par l'assistant.
