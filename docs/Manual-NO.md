> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; 🇳🇴 NO &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# OS-Flux Manual

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introduksjon

OS-Flux er en kompakt DCC-dekoder bygget rundt en ATtiny85-mikrokontroller. Den har fire LED-/funksjonsutganger og kan bygges rett inn i et signal eller et lokomotiv.

Den leveres i to ulike roller, avhengig av hvilken firmware som flashes:

| Firmware | Rolle |
|---|---|
| **Flux-Signal** | Stasjonær signaldekoder — styrer opptil 4 lysdioder i et signalbilde via utvidede DCC-signalkommandoer (DCC extended) |
| **Flux-Mobile** | Mobil funksjonsdekoder — styrer opptil 4 AUX-utganger i et lokomotiv via DCC-funksjonskommandoer |

Velg firmware før du programmerer. De to hex-filene distribueres hver for seg.

---

## Oversikt over kortet

Kortet har **to avbrekkbare fliker**.

![](snapaway.png)
*De to avbrekkbare flikene*

### Øvre flik — programmeringskontakt

Dette er ICSP-kontakten som brukes til å flashe firmware inn i ATtiny85. Den trengs bare under programmeringen. **Knekk den av etterpå** — den har ingen funksjon på anlegget, og kortet blir ryddigere uten.

Se den fullstendige programmeringsveiledningen her:
[Slik flasher du firmware på en OS-dekoder (engelsk)](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Nedre flik — adresseknapp + JST-kontakt

Denne fliken har:

- En **trykknapp** for å stille inn DCC-adressen
- En **JST-kontakt** for enkel tilkobling til DCC-bussen

**Behold denne fliken** når dekoderen brukes som signaldekoder. Med JST-kontakten kan signalet kobles til og fra DCC-bussen, slik at det blir enkelt å lage avtakbare signaler.

Ved mobil bruk (i lokomotiver) kan også den nedre fliken knekkes av hvis den er i veien.

---

## Loddeland på baksiden

![](rearsidesolderpads.png)
*Loddeland på baksiden*

På baksiden av kortet finnes loddeland for de fire utgangene. Dette er de samme elektriske punktene som loddelandene på forsiden — bruk den siden som passer best for monteringen din.

---

## Programmering

1. Velg **Flux-Signal.hex** eller **Flux-Mobile.hex** fra releases-siden.
2. Koble programmeringsfliken til en Arduino som kjører ArduinoISP. Fullstendig koblingsskjema og veiledning:
   [Slik flasher du firmware på en OS-dekoder (engelsk)](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Flash hex-filen.
4. **Knekk av programmeringsfliken.**

---

## Flux-Signal — signaldekoder

### Slik fungerer den

Flux-Signal-dekoderen styrer fire LED-utganger. Dekoderen vet ikke, og bryr seg heller ikke om, hvilken farge hver lysdiode har — det avhenger helt av hvordan du kobler signalet ditt. Bitposisjonene tilsvarer rett og slett de fire fysiske utgangene:

| Bitposisjon | Utgang |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Dekoderen lytter etter **utvidede DCC-signalkommandoer** (DCC extended accessory, `notifyDccSigOutputState`) som er sendt til den innstilte adressen. Den 8-biters tilstandsverdien i kommandoen bestemmer direkte hvilke lysdioder som lyser og hvilke som blinker:

| Biter i tilstandsbyten | Funksjon |
|---|---|
| Bit 3:0 (nedre nibble) | Signalbildemaske — hvilke utganger som er på |
| Bit 7:4 (øvre nibble) | Blinkemaske — hvilke av disse utgangene som blinker i stedet for å lyse fast |

Sett en bit i signalbildemasken for å slå på den aktuelle utgangen. Sett den samme biten i blinkemasken for å få utgangen til å blinke i stedet for å lyse fast. Utganger der biten i signalbildemasken er 0, er av, uansett blinkemasken.

Blinkeperioden er **333 ms**.

### Sette sammen tilstandsbyten

Den enkleste måten å sette sammen en tilstandsverdi på er å bruke **Windows Kalkulator i modusen Programmerer med binær visning**. Bytt til BIN og skriv inn de 8 bitene direkte — de fire øverste bitene er blinkemasken, de fire nederste er signalbildemasken. Kalkulatoren viser straks HEX-verdien som skal sendes.

![](calculator.png)
*Windows Kalkulator — modusen Programmerer, binær visning. Øvre nibble = blinkemaske, nedre nibble = signalbildemaske.*

Eksempelet over viser `0010 1011` (hex `0x2B`):

| Nibble | Biter (4 3 2 1) | Betydning |
|---|---|---|
| Øvre `0010` | bit 1 satt | LED 1 blinker |
| Nedre `1011` | bit 0, 1, 3 satt | LED 0 på, LED 1 på, LED 2 av, LED 3 på |

Resultat: LED 0 lyser fast — LED 1 blinker — LED 2 av — LED 3 lyser fast.

Mens dekoderen venter på å lære en ny adresse, blinker alle fire lysdiodene for å vise denne modusen.

### Fabrikkinnstillinger

| Innstilling | Standardverdi |
|---|---|
| DCC-adresse | 1 |
| Kommandomodus | DCC extended (`dccExt = 1`) |

### Stille inn DCC-adressen

Hold **knappen på den nedre fliken** inne i **1 sekund**. Alle lysdiodene begynner å blinke — dekoderen venter nå på en adresse.

Send en hvilken som helst DCC-kommando til adressen du vil tildele:

- Sender du en **utvidet DCC-signalkommando** — lagrer dekoderen adressen og blir værende i extended-modus (`dccExt = 1`). Dette er normal drift.
- Sender du en **vanlig DCC-sporvekselkommando** — lagrer dekoderen adressen og bytter til sporvekselmodus (`dccExt = 0`). I denne modusen reagerer dekoderen ikke lenger på utvidede signalkommandoer. Nyttig hvis sentralen din ikke kan sende utvidede signalkommandoer.

Den nye adressen og modusen lagres umiddelbart i EEPROM.

---

## Flux-Mobile — mobil funksjonsdekoder

### Slik fungerer den

Flux-Mobile-firmwaren melder seg som en **DCC-lokdekoder (multifunksjonsdekoder)**. Den har fire AUX-utganger som hver kan knyttes til et DCC-funksjonsnummer og stilles inn med ulike funksjoner via CV-programmering.

Standardadressen er **3** (kort adresse).

### Standardoppsett av utgangene

På et nytt kort oppfører de fire utgangene seg som vanlige frontlys og baklys pluss to AUX-utganger som styres av funksjonstaster:

| Utgang | Pin | Standardfunksjon |
|---|---|---|
| Aux 0 | 3 | Frontlys — på ved kjøring forover, logisk invertert (LED aktiv lav) |
| Aux 1 | 4 | Baklys — på ved kjøring bakover |
| Aux 2 | 1 | Styres av F1 |
| Aux 3 | 0 | Styres av F2 |

### DCC-adresse

Stilles inn med vanlig CV-programmering på programmeringssporet:

| CV | Funksjon |
|---|---|
| CV 1 | Kort adresse (1–127) |
| CV 17 + CV 18 | Lang adresse (skriv CV17 først, deretter CV18) |
| CV 8 = 8 | Tilbakestilling til fabrikkinnstillinger |

### CV-tabell for utgangene

Hver av de fire utgangene har en blokk på 10 CV-er som starter på CV 30:

| Utgang | Første CV i blokken |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Innenfor hver blokk:

| Offset | CV (eksempel Aux 0) | Navn | Beskrivelse |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = utgangen følger lokets retningsbit |
| +1 | CV 31 | `inverted` | 1 = utgangen er logisk invertert |
| +2 | CV 32 | `number` | DCC-funksjonsnummeret som styrer denne utgangen (0 = ingen) |
| +3 | CV 33 | `bleeps` | Antall blink (under utvikling) |
| +4 | CV 34 | `bleepOn` | Påtid per blink (under utvikling) |
| +5 | CV 35 | `bleepOff` | Avtid per blink (under utvikling) |
| +6 | CV 36 | `pulseTime` | Pulsvarighet i enheter på 100 ms (0 = deaktivert) |
| +7 | CV 37 | `slaved` | Speil en annen utgang: sett til pinnummeret til kildeutgangen (0 = deaktivert) |
| +8 | CV 38 | *(reservert)* | — |
| +9 | CV 39 | `state` | Sist kjente utgangstilstand — oppdateres automatisk, skal ikke skrives |

### Utgangenes logikk

Hver utgang går gjennom følgende prioritetsrekkefølge i hver programløkke:

1. **Slave** (`CV+7 ≠ 0`) — utgangen speiler pinnen som er angitt i denne CV-en, eventuelt invertert.
2. **Puls** (`CV+6 ≠ 0`) — utgangen gir én enkelt puls på `pulseTime × 100 ms` på hver stigende flanke av den styrende funksjonen.
3. **Fast** — utgangen følger funksjonstilstanden, eventuelt invertert via `CV+1`.

`onDirChange`-flagget (`CV+0`) XOR-er utgangen med lokets gjeldende kjøreretning, slik at samme funksjon kan styre ulike utganger avhengig av kjøreretningen — det er slik standardoppsettet med frontlys og baklys fungerer.
