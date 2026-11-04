> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; 🇫🇷 FR &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# Manuel OS-Flux

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introduction

L'OS-Flux est un décodeur DCC compact construit autour d'un microcontrôleur ATtiny85. Il dispose de quatre sorties LED/fonction et se loge directement dans le boîtier d'un signal ou dans une locomotive.

Selon le firmware flashé, il remplit l'un de deux rôles :

| Firmware | Rôle |
|---|---|
| **Flux-Signal** | Décodeur de signaux fixe — commande jusqu'à 4 feux à LED via les commandes d'accessoires DCC étendues (aspects de signal) |
| **Flux-Mobile** | Décodeur de fonctions embarqué — commande jusqu'à 4 sorties de fonction (AUX) dans une locomotive via les commandes de fonctions DCC |

Choisissez le firmware avant la programmation. Les deux fichiers hex sont distribués séparément.

---

## Présentation de la carte

La carte comporte **deux languettes sécables**.

![](snapaway.png)
*Les deux languettes sécables*

### Languette supérieure — connecteur de programmation

C'est le connecteur ICSP qui sert à flasher le firmware dans l'ATtiny85. Il n'est utile que pendant la programmation. **Détachez-la ensuite** — elle ne sert à rien en exploitation et l'installation est plus propre sans elle.

Le guide de programmation complet se trouve ici :
[Flasher le firmware d'un décodeur OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-FR.md)

### Languette inférieure — bouton d'adressage + connecteur JST

Cette languette porte :

- un **bouton-poussoir** pour régler l'adresse DCC
- un **connecteur JST** pour un raccordement facile au bus DCC

**Conservez cette languette** pour une installation en décodeur de signaux. Le connecteur JST permet de brancher et débrancher le signal du bus DCC, ce qui rend les signaux amovibles très simples à réaliser.

En utilisation embarquée (locomotive), la languette inférieure peut elle aussi être détachée si elle gêne.

---

## Pastilles à souder au verso

![](rearsidesolderpads.png)
*Pastilles à souder au verso*

Le verso de la carte offre des pastilles à souder pour les quatre sorties. Elles sont reliées électriquement aux mêmes points que les pastilles du recto — utilisez le côté qui convient le mieux à votre installation.

---

## Programmation

1. Choisissez **Flux-Signal.hex** ou **Flux-Mobile.hex** sur la page des releases.
2. Raccordez la languette de programmation à un Arduino exécutant ArduinoISP. Câblage complet et instructions :
   [Flasher le firmware d'un décodeur OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-FR.md)
3. Flashez le fichier hex.
4. **Détachez la languette de programmation.**

---

## Flux-Signal — Décodeur de signaux

### Fonctionnement

Le décodeur Flux-Signal commande quatre sorties LED. Il ne connaît pas la couleur de chaque LED et cela ne le concerne pas — tout dépend de la façon dont vous câblez votre signal. Les positions de bits correspondent simplement aux quatre pastilles de sortie physiques :

| Position du bit | Pastille de sortie |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Le décodeur écoute les **commandes d'accessoires DCC étendues (aspects de signal)** (`notifyDccSigOutputState`) adressées à son adresse configurée. La valeur d'état sur 8 bits de cette commande détermine directement quelles LED sont allumées et lesquelles clignotent :

| Bits de l'octet d'état | Fonction |
|---|---|
| Bits 3:0 (quartet bas) | Masque d'aspect — quelles sorties sont allumées |
| Bits 7:4 (quartet haut) | Masque de clignotement — lesquelles de ces sorties clignotent au lieu de rester fixes |

Mettez un bit à 1 dans le masque d'aspect pour allumer la sortie correspondante. Mettez le même bit à 1 dans le masque de clignotement pour la faire clignoter au lieu de rester fixe. Les sorties dont le bit d'aspect est à 0 restent éteintes, quel que soit le masque de clignotement.

La période de clignotement est de **333 ms**.

### Composer l'octet d'état

Le plus simple pour composer une valeur d'état est d'utiliser la **calculatrice Windows en mode Programmeur avec affichage binaire**. Passez en BIN et saisissez directement les 8 bits — les quatre bits de poids fort forment le masque de clignotement, les quatre de poids faible le masque d'aspect. La calculatrice affiche immédiatement la valeur HEX à envoyer.

![](calculator.png)
*Calculatrice Windows — mode Programmeur, affichage binaire. Quartet haut = masque de clignotement, quartet bas = masque d'aspect.*

L'exemple ci-dessus montre `0010 1011` (hex `0x2B`) :

| Quartet | Bits (4 3 2 1) | Signification |
|---|---|---|
| Haut `0010` | bit 1 à 1 | LED 1 clignote |
| Bas `1011` | bits 0, 1, 3 à 1 | LED 0 allumée, LED 1 allumée, LED 2 éteinte, LED 3 allumée |

Résultat : LED 0 allumée fixe — LED 1 clignotante — LED 2 éteinte — LED 3 allumée fixe.

Tant que le décodeur attend d'apprendre une nouvelle adresse, les quatre LED clignotent pour signaler ce mode.

### Réglages par défaut

| Réglage | Valeur par défaut |
|---|---|
| Adresse DCC | 1 |
| Mode de commande | DCC étendu (`dccExt = 1`) |

### Réglage de l'adresse DCC

Maintenez le **bouton de la languette inférieure** enfoncé pendant **1 seconde**. Toutes les LED se mettent à clignoter — le décodeur attend maintenant une adresse.

Envoyez une commande DCC quelconque à l'adresse que vous voulez attribuer :

- Si vous envoyez une **commande d'accessoire DCC étendue (aspect de signal)**, le décodeur enregistre cette adresse et reste en mode étendu (`dccExt = 1`). C'est le fonctionnement normal.
- Si vous envoyez une **commande d'aiguillage DCC (commande d'accessoire de base)**, le décodeur enregistre cette adresse et passe en mode aiguillage (`dccExt = 0`). Dans ce mode, le décodeur ne réagit plus aux commandes d'accessoires étendues. Pratique si votre centrale ne sait pas envoyer de commandes d'accessoires étendues.

La nouvelle adresse et le mode sont immédiatement enregistrés en EEPROM.

---

## Flux-Mobile — Décodeur de fonctions embarqué

### Fonctionnement

Le firmware Flux-Mobile se présente comme un **décodeur DCC multifonction (décodeur de locomotive)**. Il dispose de quatre sorties de fonction (AUX), chacune pouvant être associée à un numéro de fonction DCC et configurée avec divers comportements par programmation des CV.

L'adresse par défaut est **3** (adresse courte).

### Configuration par défaut des sorties

Sur une carte neuve, les quatre sorties se comportent comme des feux avant/arrière classiques, plus deux sorties auxiliaires commandées par des fonctions :

| Sortie | Broche | Comportement par défaut |
|---|---|---|
| Aux 0 | 3 | Feux avant — allumés en marche avant, logique inversée (LED active à l'état bas) |
| Aux 1 | 4 | Feux arrière — allumés en marche arrière |
| Aux 2 | 1 | Commandée par F1 |
| Aux 3 | 0 | Commandée par F2 |

### Adresse DCC

Réglage par programmation standard des CV sur la voie de programmation DCC :

| CV | Fonction |
|---|---|
| CV 1 | Adresse courte (1–127) |
| CV 17 + CV 18 | Adresse longue (écrire d'abord CV17, puis CV18) |
| CV 8 = 8 | Retour aux réglages d'usine |

### Tableau des CV des sorties

Chacune des quatre sorties dispose d'un bloc de 10 CV à partir de CV 30 :

| Sortie | Début du bloc de CV |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Dans chaque bloc :

| Décalage | CV (exemple Aux 0) | Nom | Description |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = la sortie suit le bit de sens de marche de la locomotive |
| +1 | CV 31 | `inverted` | 1 = la sortie est logiquement inversée |
| +2 | CV 32 | `number` | Numéro de fonction DCC qui commande cette sortie (0 = aucune) |
| +3 | CV 33 | `bleeps` | Nombre de bips (en développement) |
| +4 | CV 34 | `bleepOn` | Durée d'allumage d'un bip (en développement) |
| +5 | CV 35 | `bleepOff` | Durée d'extinction d'un bip (en développement) |
| +6 | CV 36 | `pulseTime` | Durée d'impulsion en unités de 100 ms (0 = désactivé) |
| +7 | CV 37 | `slaved` | Recopier une autre sortie : indiquer le numéro de broche de la sortie source (0 = désactivé) |
| +8 | CV 38 | *(réservé)* | — |
| +9 | CV 39 | `state` | Dernier état connu de la sortie — mis à jour automatiquement, ne pas écrire |

### Logique de comportement des sorties

À chaque passage de boucle, chaque sortie applique la chaîne de priorités suivante :

1. **Asservie** (`CV+7 ≠ 0`) — la sortie recopie la broche indiquée dans cette CV, éventuellement inversée.
2. **Impulsion** (`CV+6 ≠ 0`) — la sortie délivre une impulsion unique de `pulseTime × 100 ms` à chaque front montant de la fonction qui la commande.
3. **Fixe** — la sortie suit l'état de la fonction, éventuellement inversée via `CV+1`.

L'indicateur `onDirChange` (`CV+0`) combine la sortie par XOR avec le sens de marche actuel de la locomotive, ce qui permet à une même fonction de commander des sorties différentes selon le sens de marche — c'est ainsi que fonctionnent les feux avant/arrière par défaut.
