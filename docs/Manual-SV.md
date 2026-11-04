> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; 🇸🇪 SV &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# OS-Flux Manual

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introduktion

OS-Flux är en kompakt DCC-dekoder uppbyggd kring en ATtiny85-mikrokontroller. Den har fyra LED-/funktionsutgångar och kan byggas in direkt i en signal eller ett lok.

Den finns i två olika roller, beroende på vilken firmware som flashas:

| Firmware | Roll |
|---|---|
| **Flux-Signal** | Stationär signaldekoder — styr upp till 4 lysdioder i en signalbild via utökade DCC-signalkommandon (DCC extended) |
| **Flux-Mobile** | Mobil funktionsdekoder för lok — styr upp till 4 AUX-utgångar i ett lok via DCC-funktionskommandon |

Välj firmware innan du programmerar. De två hex-filerna distribueras separat.

---

## Översikt över kortet

Kortet har **två avbrytbara flikar**.

![](snapaway.png)
*De två avbrytbara flikarna*

### Övre flik — programmeringskontakt

Detta är ICSP-kontakten som används för att flasha firmware till ATtiny85. Den behövs bara vid programmeringen. **Bryt av den efteråt** — den fyller ingen funktion på anläggningen och kortet blir snyggare utan den.

Se den fullständiga programmeringsguiden här:
[Så flashar du firmware på en OS-dekoder (engelska)](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Nedre flik — adressknapp + JST-kontakt

Den här fliken har:

- En **tryckknapp** för att ställa in DCC-adressen
- En **JST-kontakt** för enkel anslutning till DCC-bussen

**Behåll den här fliken** när dekodern används som signaldekoder. Med JST-kontakten kan signalen kopplas in i och ur DCC-bussen, vilket gör det enkelt att bygga löstagbara signaler.

Vid mobil användning (i lok) kan även den nedre fliken brytas av om den är i vägen.

---

## Lödpunkter på baksidan

![](rearsidesolderpads.png)
*Lödpunkter på baksidan*

På kortets baksida finns lödpunkter för de fyra utgångarna. Det är samma elektriska punkter som lödpunkterna på framsidan — använd den sida som passar din installation bäst.

---

## Programmering

1. Välj **Flux-Signal.hex** eller **Flux-Mobile.hex** på releases-sidan.
2. Anslut programmeringsfliken till en Arduino som kör ArduinoISP. Fullständigt kopplingsschema och instruktioner:
   [Så flashar du firmware på en OS-dekoder (engelska)](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Flasha hex-filen.
4. **Bryt av programmeringsfliken.**

---

## Flux-Signal — signaldekoder

### Så fungerar den

Flux-Signal-dekodern styr fyra LED-utgångar. Dekodern vet inte, och bryr sig inte om, vilken färg varje lysdiod har — det beror helt på hur du kopplar din signal. Bitpositionerna motsvarar helt enkelt de fyra fysiska utgångarna:

| Bitposition | Utgång |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Dekodern lyssnar efter **utökade DCC-signalkommandon** (DCC extended accessory, `notifyDccSigOutputState`) som skickas till dess inställda adress. Det 8-bitars tillståndsvärdet i kommandot styr direkt vilka lysdioder som är tända och vilka som blinkar:

| Bitar i tillståndsbyten | Funktion |
|---|---|
| Bit 3:0 (nedre nibble) | Signalbildsmask — vilka utgångar som är tända |
| Bit 7:4 (övre nibble) | Blinkmask — vilka av dessa utgångar som blinkar i stället för att lysa med fast sken |

Sätt en bit i signalbildsmasken för att tända motsvarande utgång. Sätt samma bit i blinkmasken för att få utgången att blinka i stället för att lysa med fast sken. Utgångar vars bit i signalbildsmasken är 0 är släckta, oavsett blinkmasken.

Blinkperioden är **333 ms**.

### Sätta ihop tillståndsbyten

Det enklaste sättet att sätta ihop ett tillståndsvärde är att använda **Kalkylatorn i Windows i läget Programmerare med binär visning**. Växla till BIN och skriv in de 8 bitarna direkt — de fyra övre bitarna är blinkmasken, de fyra nedre är signalbildsmasken. Kalkylatorn visar direkt HEX-värdet som ska skickas.

![](calculator.png)
*Kalkylatorn i Windows — läget Programmerare, binär visning. Övre nibble = blinkmask, nedre nibble = signalbildsmask.*

Exemplet ovan visar `0010 1011` (hex `0x2B`):

| Nibble | Bitar (4 3 2 1) | Betydelse |
|---|---|---|
| Övre `0010` | bit 1 satt | LED 1 blinkar |
| Nedre `1011` | bit 0, 1, 3 satta | LED 0 tänd, LED 1 tänd, LED 2 släckt, LED 3 tänd |

Resultat: LED 0 fast sken — LED 1 blinkar — LED 2 släckt — LED 3 fast sken.

Medan dekodern väntar på att lära sig en ny adress blinkar alla fyra lysdioderna för att visa detta läge.

### Fabriksinställningar

| Inställning | Standardvärde |
|---|---|
| DCC-adress | 1 |
| Kommandoläge | DCC extended (`dccExt = 1`) |

### Ställa in DCC-adressen

Håll **knappen på den nedre fliken** intryckt i **1 sekund**. Alla lysdioder börjar blinka — dekodern väntar nu på en adress.

Skicka valfritt DCC-kommando till den adress du vill tilldela:

- Skickar du ett **utökat DCC-signalkommando** — sparar dekodern adressen och stannar i extended-läge (`dccExt = 1`). Detta är normal drift.
- Skickar du ett **vanligt DCC-växelkommando** — sparar dekodern adressen och växlar till växelläge (`dccExt = 0`). I det här läget reagerar dekodern inte längre på utökade signalkommandon. Användbart om din kommandostation inte kan skicka utökade signalkommandon.

Den nya adressen och läget sparas direkt i EEPROM.

---

## Flux-Mobile — mobil funktionsdekoder

### Så fungerar den

Flux-Mobile-firmwaren registrerar sig som en **DCC-lokdekoder (multifunktionsdekoder)**. Den har fyra AUX-utgångar som var och en kan kopplas till ett DCC-funktionsnummer och ställas in med olika beteenden via CV-programmering.

Standardadressen är **3** (kort adress).

### Standardinställning av utgångarna

På ett nytt kort fungerar de fyra utgångarna som vanliga strålkastare fram och bak plus två AUX-utgångar som styrs av funktionsknappar:

| Utgång | Pin | Standardbeteende |
|---|---|---|
| Aux 0 | 3 | Främre strålkastare — tänd vid körning framåt, logiskt inverterad (LED aktiv låg) |
| Aux 1 | 4 | Bakre strålkastare — tänd vid körning bakåt |
| Aux 2 | 1 | Styrs av F1 |
| Aux 3 | 0 | Styrs av F2 |

### DCC-adress

Ställs in med vanlig CV-programmering på programmeringsspåret:

| CV | Funktion |
|---|---|
| CV 1 | Kort adress (1–127) |
| CV 17 + CV 18 | Lång adress (skriv CV17 först, sedan CV18) |
| CV 8 = 8 | Återställning till fabriksinställningar |

### CV-tabell för utgångarna

Var och en av de fyra utgångarna har ett block med 10 CV:er som börjar på CV 30:

| Utgång | Första CV i blocket |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Inom varje block:

| Offset | CV (exempel Aux 0) | Namn | Beskrivning |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = utgången följer lokets riktningsbit |
| +1 | CV 31 | `inverted` | 1 = utgången är logiskt inverterad |
| +2 | CV 32 | `number` | DCC-funktionsnummer som styr denna utgång (0 = ingen) |
| +3 | CV 33 | `bleeps` | Antal blinkningar (under utveckling) |
| +4 | CV 34 | `bleepOn` | Tändtid per blinkning (under utveckling) |
| +5 | CV 35 | `bleepOff` | Släcktid per blinkning (under utveckling) |
| +6 | CV 36 | `pulseTime` | Pulslängd i enheter om 100 ms (0 = avaktiverad) |
| +7 | CV 37 | `slaved` | Spegla en annan utgång: ange källutgångens pinnummer (0 = avaktiverad) |
| +8 | CV 38 | *(reserverad)* | — |
| +9 | CV 39 | `state` | Senast kända utgångstillstånd — uppdateras automatiskt, skriv inte hit |

### Utgångarnas logik

Varje utgång går igenom följande prioritetsordning i varje programslinga:

1. **Slav** (`CV+7 ≠ 0`) — utgången speglar det stift som anges i denna CV, eventuellt inverterat.
2. **Puls** (`CV+6 ≠ 0`) — utgången ger en enda puls på `pulseTime × 100 ms` vid varje stigande flank hos den styrande funktionen.
3. **Fast** — utgången följer funktionens tillstånd, eventuellt inverterat via `CV+1`.

Flaggan `onDirChange` (`CV+0`) XOR:ar utgången med lokets aktuella körriktning, så att samma funktion kan styra olika utgångar beroende på körriktningen — det är så standardinställningen med strålkastare fram och bak fungerar.
