# PFI Automatisation — architecture de travail

**État : exemple de programme et de logique, non validé sur automate.**

- `melangeur/` : interface automate/moteurs / protocole RS-232 (partie 4 de l'énoncé).
- `balance/` : acquisition RS-232 par le Raspberry Pi (partie 5 ; dépend du modèle Toledo du labo).
- `aspiration/` : logique API et séquence de remplissage du réservoir le moins plein (partie 8).
- `communications/` : protocole série court-distance selon annexe II.

La technologie exacte de l'API, la syntaxe exportable et le brochage réel doivent être validés au laboratoire. Le fichier `.st` est un modèle IEC 61131-3 de logique, pas un téléchargement directement garanti dans CCW.

**Sécurité :** les arrêts d'urgence, verrouillages électriques et limites physiques doivent être câblés et validés indépendamment du logiciel d'essai.
