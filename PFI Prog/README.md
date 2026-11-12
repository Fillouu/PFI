# PFI Prog — panneau RAM et Power Meter

Code C++17 séparé en deux exécutables, réutilisable avec les pilotes fournis en laboratoire.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

**État réel : non fonctionnel sur matériel tant que les pilotes RAM, Power Meter et MQTT ne sont pas branchés.**
`ClientMqtt.cpp`, `InterfacePanneau.cpp` et `LecteurPowerMeter.cpp` contiennent les points d'intégration à terminer.
Ne pas prétendre que les alarmes temporisées, la régulation de température ou la synchro console/serveur sont terminées.

Pour la première démonstration, tester les commandes sans actionneurs réels puis valider les états sûrs sur le banc avec le professeur.
