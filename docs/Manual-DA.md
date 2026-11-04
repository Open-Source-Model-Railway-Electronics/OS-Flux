> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; 🇩🇰 DA &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# OS-Flux Manual

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introduktion

OS-Flux er en kompakt DCC-dekoder bygget op omkring en ATtiny85-mikrocontroller. Den har fire LED-/funktionsudgange og kan bygges direkte ind i et signal eller et lokomotiv.

Den findes i to forskellige roller, afhængigt af hvilken firmware der flashes:

| Firmware | Rolle |
|---|---|
| **Flux-Signal** | Stationær signaldekoder — styrer op til 4 LED'er i et signalbillede via udvidede DCC-signalkommandoer (DCC extended) |
| **Flux-Mobile** | Mobil funktionsdekoder — styrer op til 4 AUX-udgange i et lokomotiv via DCC-funktionskommandoer |

Vælg firmware, før du programmerer. De to hex-filer distribueres hver for sig.

---

## Oversigt over printet

Printet har **to afbrækkelige flige**.

![](snapaway.png)
*De to afbrækkelige flige*

### Øverste flig — programmeringsstik

Dette er ICSP-stikket, som bruges til at flashe firmware ind i ATtiny85. Det skal kun bruges under programmeringen. **Knæk den af bagefter** — den har ingen funktion på anlægget, og printet bliver pænere uden.

Se den komplette programmeringsvejledning her:
[Sådan flasher du firmware på en OS-dekoder (engelsk)](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Nederste flig — adresseknap + JST-stik

Denne flig rummer:

- En **trykknap** til indstilling af DCC-adressen
- Et **JST-stik**, så dekoderen nemt kan tilsluttes DCC-bussen

**Behold denne flig**, når dekoderen bruges som signaldekoder. Med JST-stikket kan signalet sættes i og tages ud af DCC-bussen, så det er nemt at lave aftagelige signaler.

Ved mobil brug (i lokomotiver) kan den nederste flig også knækkes af, hvis den er i vejen.

---

## Loddepunkter på bagsiden

![](rearsidesolderpads.png)
*Loddepunkter på bagsiden*

På bagsiden af printet er der loddepunkter til de fire udgange. Det er de samme elektriske forbindelser som loddepunkterne på forsiden — brug den side, der passer bedst til din montering.

---

## Programmering

1. Hent **Flux-Signal.hex** eller **Flux-Mobile.hex** fra releases-siden.
2. Forbind programmeringsfligen til en Arduino, der kører ArduinoISP. Komplet ledningsføring og vejledning:
   [Sådan flasher du firmware på en OS-dekoder (engelsk)](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Flash hex-filen.
4. **Knæk programmeringsfligen af.**

---

## Flux-Signal — signaldekoder

### Sådan virker den

Flux-Signal-dekoderen styrer fire LED-udgange. Dekoderen ved ikke, hvilken farve hver LED har, og er også ligeglad — det afhænger helt af, hvordan du har forbundet dit signal. Bitpositionerne svarer ganske enkelt til de fire fysiske udgangs-loddepunkter:

| Bitposition | Udgang |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Dekoderen lytter efter **udvidede DCC-signalkommandoer** (DCC extended accessory, `notifyDccSigOutputState`), der er sendt til dens indstillede adresse. Den 8-bit tilstandsværdi i kommandoen bestemmer direkte, hvilke LED'er der lyser, og hvilke der blinker:

| Bits i tilstandsbyten | Funktion |
|---|---|
| Bit 3:0 (nederste nibble) | Signalbillede-maske — hvilke udgange der er tændt |
| Bit 7:4 (øverste nibble) | Blinkmaske — hvilke af disse udgange der blinker i stedet for at lyse fast |

Sæt en bit i signalbillede-masken for at tænde den pågældende udgang. Sæt den samme bit i blinkmasken for at få udgangen til at blinke i stedet for at lyse fast. Udgange, hvis bit i signalbillede-masken er 0, er slukket, uanset blinkmasken.

Blinkperioden er **333 ms**.

### Sammensætning af tilstandsbyten

Den nemmeste måde at sammensætte en tilstandsværdi på er at bruge **Windows Lommeregner i tilstanden Programmør med binær visning**. Skift til BIN og indtast de 8 bits direkte — de øverste fire bits er blinkmasken, de nederste fire er signalbillede-masken. Lommeregneren viser straks den HEX-værdi, der skal sendes.

![](calculator.png)
*Windows Lommeregner — tilstanden Programmør, binær visning. Øverste nibble = blinkmaske, nederste nibble = signalbillede-maske.*

Eksemplet ovenfor viser `0010 1011` (hex `0x2B`):

| Nibble | Bits (4 3 2 1) | Betydning |
|---|---|---|
| Øverste `0010` | bit 1 sat | LED 1 blinker |
| Nederste `1011` | bit 0, 1, 3 sat | LED 0 tændt, LED 1 tændt, LED 2 slukket, LED 3 tændt |

Resultat: LED 0 lyser fast — LED 1 blinker — LED 2 slukket — LED 3 lyser fast.

Mens dekoderen venter på at lære en ny adresse, blinker alle fire LED'er for at vise denne tilstand.

### Fabriksindstillinger

| Indstilling | Standardværdi |
|---|---|
| DCC-adresse | 1 |
| Kommandotilstand | DCC extended (`dccExt = 1`) |

### Indstilling af DCC-adressen

Hold **knappen på den nederste flig** inde i **1 sekund**. Alle LED'er begynder at blinke — dekoderen venter nu på en adresse.

Send en vilkårlig DCC-kommando til den adresse, du vil tildele:

- Sender du en **udvidet DCC-signalkommando** — gemmer dekoderen adressen og bliver i extended-tilstand (`dccExt = 1`). Dette er normal drift.
- Sender du en **almindelig DCC-sporskiftekommando** — gemmer dekoderen adressen og skifter til sporskiftetilstand (`dccExt = 0`). I denne tilstand reagerer dekoderen ikke længere på udvidede signalkommandoer. Det er praktisk, hvis din digitalcentral ikke kan sende udvidede signalkommandoer.

Den nye adresse og tilstand gemmes straks i EEPROM.

---

## Flux-Mobile — mobil funktionsdekoder

### Sådan virker den

Flux-Mobile-firmwaren melder sig som en **DCC-lokdekoder (multifunktionsdekoder)**. Den har fire AUX-udgange, som hver kan knyttes til et DCC-funktionsnummer og indstilles til forskellige funktioner via CV-programmering.

Standardadressen er **3** (kort adresse).

### Standardopsætning af udgangene

På et nyt print opfører de fire udgange sig som almindelige for- og baglygter plus to AUX-udgange, der styres af funktionstaster:

| Udgang | Pin | Standardfunktion |
|---|---|---|
| Aux 0 | 3 | Forlygte — tændt ved kørsel fremad, logisk inverteret (LED aktiv lav) |
| Aux 1 | 4 | Baglygte — tændt ved kørsel baglæns |
| Aux 2 | 1 | Styres af F1 |
| Aux 3 | 0 | Styres af F2 |

### DCC-adresse

Indstilles med almindelig CV-programmering på programmeringssporet:

| CV | Funktion |
|---|---|
| CV 1 | Kort adresse (1–127) |
| CV 17 + CV 18 | Lang adresse (skriv CV17 først, derefter CV18) |
| CV 8 = 8 | Nulstilling til fabriksindstillinger |

### CV-tabel for udgangene

Hver af de fire udgange har en blok på 10 CV'er, der starter ved CV 30:

| Udgang | Første CV i blokken |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Inden for hver blok:

| Offset | CV (eksempel Aux 0) | Navn | Beskrivelse |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = udgangen følger lokomotivets retningsbit |
| +1 | CV 31 | `inverted` | 1 = udgangen er logisk inverteret |
| +2 | CV 32 | `number` | DCC-funktionsnummer, der styrer denne udgang (0 = ingen) |
| +3 | CV 33 | `bleeps` | Antal blink (under udvikling) |
| +4 | CV 34 | `bleepOn` | Tændtid pr. blink (under udvikling) |
| +5 | CV 35 | `bleepOff` | Slukket tid pr. blink (under udvikling) |
| +6 | CV 36 | `pulseTime` | Pulsvarighed i enheder af 100 ms (0 = deaktiveret) |
| +7 | CV 37 | `slaved` | Spejl en anden udgang: sæt til kildeudgangens pinnummer (0 = deaktiveret) |
| +8 | CV 38 | *(reserveret)* | — |
| +9 | CV 39 | `state` | Sidst kendte udgangstilstand — opdateres automatisk, må ikke skrives |

### Udgangenes logik

Hver udgang gennemløber følgende prioritetskæde i hver programløkke:

1. **Slave** (`CV+7 ≠ 0`) — udgangen spejler den pin, der er angivet i denne CV, eventuelt inverteret.
2. **Puls** (`CV+6 ≠ 0`) — udgangen giver en enkelt puls på `pulseTime × 100 ms` ved hver stigende flanke af den styrende funktion.
3. **Fast** — udgangen følger funktionens tilstand, eventuelt inverteret via `CV+1`.

`onDirChange`-flaget (`CV+0`) XOR'er udgangen med lokomotivets aktuelle køreretning, så den samme funktion kan styre forskellige udgange afhængigt af køreretningen — det er sådan, standardopsætningen af for- og baglygter fungerer.
