# Superviseur Web — PFI

Prototype Node.js / Express / EJS / MQTT / MariaDB (document d'intégration révision 2025).

## Installation
1. `npm install`
2. Copier `.env.example` vers `.env`, remplacer les secrets, configurer le broker.
3. Installer le schéma `db/schema.sql` dans MariaDB.
4. `node db/creer_utilisateur.js admin administrateur "MOT_DE_PASSE_LONG_ET_PERSONNEL"`.
5. Définir `TEST_LOCAL_ONLY=0` pour autoriser la connexion et lancer `npm start`.
6. Ouvrir `http://localhost:3000`.

## Limites importantes
- Version pédagogique, ne constitue pas un système industriel sécuritaire.
- Le broker doit être connecté, sinon les commandes ne sont pas transmises.
- Les alarmes sont publiées par les Raspberry Pi; la gestion des ACK / retained et la journalisation automatique d'alarmes restent à implémenter.
- Les consignes MQTT 2025 sont utilisées, à confirmer pour la PFI 2026.
- En production il faut un stockage persistant des sessions et une protection CSRF explicite.
- La gestion de la caméra, les autorisations fines et le contrôle des recettes sont à finaliser.
- TEST_LOCAL_ONLY=1 désactive volontairement l'authentification : il n'offre pas de session invité.
