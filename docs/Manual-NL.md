> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; 🇳🇱 NL &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# OS-Flux Handleiding

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Inleiding

De OS-Flux is een compacte DCC-decoder op basis van een ATtiny85-microcontroller. Hij heeft vier LED-/functie-uitgangen en past direct in de behuizing van een sein of in een loc.

Afhankelijk van de geflashte firmware vervult hij een van twee rollen:

| Firmware | Rol |
|---|---|
| **Flux-Signal** | Stationaire seindecoder — stuurt tot 4 LED's van een sein aan via uitgebreide DCC-accessoirecommando's (seinbeeldcommando's) |
| **Flux-Mobile** | Functiedecoder voor rijdend materieel — stuurt tot 4 functie-uitgangen (AUX) in een loc aan via DCC-functiecommando's |

Kies de firmware voordat u gaat programmeren. De twee hex-bestanden worden apart aangeboden.

---

## Overzicht van de print

De print heeft **twee afbreekbare lipjes**.

![](snapaway.png)
*De twee afbreekbare lipjes*

### Bovenste lipje — programmeerconnector

Dit is de ICSP-connector waarmee de firmware op de ATtiny85 wordt geflasht. Hij is alleen nodig tijdens het programmeren. **Breek hem daarna af** — in gebruik heeft hij geen functie en zonder is de inbouw netter.

De volledige programmeerhandleiding vindt u hier:
[Firmware flashen op een OS-decoder](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-NL.md)

### Onderste lipje — adresknop + JST-connector

Op dit lipje zitten:

- een **drukknop** voor het instellen van het DCC-adres
- een **JST-connector** voor eenvoudige aansluiting op de DCC-bus

**Laat dit lipje zitten** bij inbouw als seindecoder. Met de JST-connector kunt u het sein loskoppelen van en weer aansluiten op de DCC-bus, zodat uitneembare seinen eenvoudig te realiseren zijn.

Bij gebruik in rijdend materieel (loc) mag ook het onderste lipje worden afgebroken als het in de weg zit.

---

## Soldeerpads aan de achterzijde

![](rearsidesolderpads.png)
*Soldeerpads aan de achterzijde*

Aan de achterzijde van de print zitten soldeerpads voor de vier uitgangen. Elektrisch zijn dit dezelfde aansluitingen als de pads aan de voorzijde — gebruik de kant die het beste bij uw inbouw past.

---

## Programmeren

1. Kies **Flux-Signal.hex** of **Flux-Mobile.hex** op de releasepagina.
2. Sluit het programmeerlipje aan op een Arduino met ArduinoISP. Volledige bedrading en instructies:
   [Firmware flashen op een OS-decoder](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-NL.md)
3. Flash het hex-bestand.
4. **Breek het programmeerlipje af.**

---

## Flux-Signal — Seindecoder

### Werking

De Flux-Signal-decoder stuurt vier LED-uitgangen aan. Welke kleur elke LED heeft, weet de decoder niet en maakt hem ook niet uit — dat hangt volledig af van hoe u uw sein bedraadt. De bitposities komen eenvoudigweg overeen met de vier fysieke uitgangspads:

| Bitpositie | Uitgangspad |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

De decoder luistert naar **uitgebreide DCC-accessoirecommando's (seinbeeldcommando's)** (`notifyDccSigOutputState`) die aan zijn ingestelde adres zijn gericht. De 8-bits statuswaarde in dat commando bepaalt direct welke LED's branden en welke knipperen:

| Bits van de statusbyte | Functie |
|---|---|
| Bits 3:0 (lage nibble) | Seinbeeldmasker — welke uitgangen aan zijn |
| Bits 7:4 (hoge nibble) | Knippermasker — welke van die uitgangen knipperen in plaats van continu te branden |

Zet een bit in het seinbeeldmasker om die uitgang in te schakelen. Zet hetzelfde bit in het knippermasker om hem te laten knipperen in plaats van continu te laten branden. Uitgangen waarvan het seinbeeldbit 0 is, zijn uit, ongeacht het knippermasker.

De knipperperiode is **333 ms**.

### De statusbyte samenstellen

De eenvoudigste manier om een statuswaarde samen te stellen is met de **Windows-rekenmachine in de modus Programmeur met binaire weergave**. Schakel over naar BIN en typ de 8 bits direct in — de bovenste vier bits zijn het knippermasker, de onderste vier het seinbeeldmasker. De rekenmachine toont meteen de HEX-waarde die u moet versturen.

![](calculator.png)
*Windows-rekenmachine — modus Programmeur, binaire weergave. Hoge nibble = knippermasker, lage nibble = seinbeeldmasker.*

Het voorbeeld hierboven toont `0010 1011` (hex `0x2B`):

| Nibble | Bits (4 3 2 1) | Betekenis |
|---|---|---|
| Hoog `0010` | bit 1 gezet | LED 1 knippert |
| Laag `1011` | bits 0, 1, 3 gezet | LED 0 aan, LED 1 aan, LED 2 uit, LED 3 aan |

Resultaat: LED 0 continu aan — LED 1 knippert — LED 2 uit — LED 3 continu aan.

Zolang de decoder wacht op het leren van een nieuw adres, knipperen alle vier LED's om deze modus aan te geven.

### Standaardinstellingen

| Instelling | Standaard |
|---|---|
| DCC-adres | 1 |
| Commandomodus | DCC extended (`dccExt = 1`) |

### Het DCC-adres instellen

Houd de **drukknop op het onderste lipje** **1 seconde** ingedrukt. Alle LED's gaan knipperen — de decoder wacht nu op een adres.

Stuur een willekeurig DCC-commando naar het adres dat u wilt toekennen:

- Stuurt u een **uitgebreid DCC-accessoirecommando (seinbeeldcommando)**, dan slaat de decoder dat adres op en blijft hij in de extended-modus (`dccExt = 1`). Dit is de normale werking.
- Stuurt u een **DCC-wisselcommando (basis-accessoirecommando)**, dan slaat de decoder dat adres op en schakelt hij over naar de wisselmodus (`dccExt = 0`). In deze modus reageert de decoder niet meer op uitgebreide seinbeeldcommando's. Handig als uw centrale geen uitgebreide accessoirecommando's kan versturen.

Het nieuwe adres en de modus worden direct in de EEPROM opgeslagen.

---

## Flux-Mobile — Functiedecoder voor rijdend materieel

### Werking

De Flux-Mobile-firmware meldt zich als **multifunctiedecoder (locdecoder) voor DCC**. Hij heeft vier functie-uitgangen (AUX) die elk aan een DCC-functienummer kunnen worden gekoppeld en via CV-programmering met verschillende gedragingen kunnen worden ingesteld.

Het standaardadres is **3** (kort adres).

### Standaardconfiguratie van de uitgangen

Op een nieuwe print gedragen de vier uitgangen zich als gangbare frontverlichting voor/achter plus twee via functies geschakelde extra uitgangen:

| Uitgang | Pin | Standaardgedrag |
|---|---|---|
| Aux 0 | 3 | Frontverlichting voor — aan bij vooruit rijden, logisch geïnverteerd (actief-laag LED) |
| Aux 1 | 4 | Frontverlichting achter — aan bij achteruit rijden |
| Aux 2 | 1 | Geschakeld met F1 |
| Aux 3 | 0 | Geschakeld met F2 |

### DCC-adres

In te stellen via standaard CV-programmering op het DCC-programmeerspoor:

| CV | Functie |
|---|---|
| CV 1 | Kort adres (1–127) |
| CV 17 + CV 18 | Lang adres (eerst CV17 schrijven, daarna CV18) |
| CV 8 = 8 | Terugzetten naar fabrieksinstellingen (reset) |

### CV-tabel van de uitgangen

Elk van de vier uitgangen heeft een blok van 10 CV's, beginnend bij CV 30:

| Uitgang | Begin van CV-blok |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Binnen elk blok:

| Offset | CV (voorbeeld Aux 0) | Naam | Beschrijving |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = uitgang volgt het rijrichtingsbit van de loc |
| +1 | CV 31 | `inverted` | 1 = uitgang is logisch geïnverteerd |
| +2 | CV 32 | `number` | DCC-functienummer dat deze uitgang schakelt (0 = geen) |
| +3 | CV 33 | `bleeps` | Aantal bleeps (in ontwikkeling) |
| +4 | CV 34 | `bleepOn` | Aan-tijd van een bleep (in ontwikkeling) |
| +5 | CV 35 | `bleepOff` | Uit-tijd van een bleep (in ontwikkeling) |
| +6 | CV 36 | `pulseTime` | Pulsduur in eenheden van 100 ms (0 = uitgeschakeld) |
| +7 | CV 37 | `slaved` | Andere uitgang volgen: vul het pinnummer van de bronuitgang in (0 = uitgeschakeld) |
| +8 | CV 38 | *(gereserveerd)* | — |
| +9 | CV 39 | `state` | Laatst bekende uitgangstoestand — wordt automatisch bijgewerkt, niet beschrijven |

### Logica van het uitgangsgedrag

Elke uitgang doorloopt bij elke lusdoorgang de volgende prioriteitsketen:

1. **Gekoppeld** (`CV+7 ≠ 0`) — de uitgang volgt de pin die in deze CV is opgegeven, eventueel geïnverteerd.
2. **Puls** (`CV+6 ≠ 0`) — de uitgang geeft bij elke opgaande flank van de schakelende functie één puls van `pulseTime × 100 ms`.
3. **Continu** — de uitgang volgt de functietoestand, eventueel geïnverteerd via `CV+1`.

De vlag `onDirChange` (`CV+0`) combineert de uitgang via XOR met de actuele rijrichting van de loc. Zo kan dezelfde functie afhankelijk van de rijrichting verschillende uitgangen schakelen — en precies zo werkt de standaard rijrichtingsafhankelijke frontverlichting voor/achter.
