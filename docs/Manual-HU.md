> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; 🇭🇺 HU &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# OS-Flux Kézikönyv

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Bevezetés

Az OS-Flux egy kompakt, ATtiny85 mikrovezérlőre épülő DCC dekóder. Négy LED-/funkciókimenete van, és közvetlenül beépíthető egy fényjelző házába vagy egy mozdonyba.

A feltöltött firmware-től függően két különböző szerepet tölthet be:

| Firmware | Szerep |
|---|---|
| **Flux-Signal** | Helyhez kötött jelződekóder — legfeljebb 4 jelző-LED-et vezérel DCC kiterjesztett jelzőparancsokkal |
| **Flux-Mobile** | Járműbe szánt funkciódekóder — legfeljebb 4 funkciókimenetet (AUX) vezérel a mozdonyban DCC funkcióparancsokkal |

A firmware-t a programozás előtt válaszd ki. A két hex fájlt külön terjesztjük.

---

## A panel áttekintése

A panelnek **két letörhető füle** van.

![](snapaway.png)
*A két letörhető fül*

### Felső fül — programozó csatlakozó

Ez az ICSP csatlakozó, amelyen keresztül a firmware az ATtiny85-be kerül. Csak a programozáshoz kell. **Utána törd le** — üzem közben semmi szerepe, és nélküle tisztább a beépítés.

A teljes programozási útmutató itt található:
[Firmware feltöltése egy OS dekóderre](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Alsó fül — címbeállító gomb + JST csatlakozó

Ezen a fülön található:

- Egy **nyomógomb** a DCC cím beállításához
- Egy **JST csatlakozó** a DCC buszra való egyszerű csatlakoztatáshoz

Jelződekóderként való beépítésnél **ezt a fület hagyd meg**. A JST csatlakozóval a jelző egyszerűen rácsatlakoztatható a DCC buszra és leválasztható róla, így a kivehető jelzők gond nélkül megoldhatók.

Járműben (mozdonyban) való használatnál az alsó fül is letörhető, ha útban van.

---

## Forrasztási pontok a hátoldalon

![](rearsidesolderpads.png)
*Forrasztási pontok a hátoldalon*

A panel hátoldalán a négy kimenet forrasztási pontjai is elérhetők. Ezek elektromosan azonosak az előlapi forrasztási pontokkal — azt az oldalt használd, amelyik a beépítéshez kényelmesebb.

---

## Programozás

1. Töltsd le a **Flux-Signal.hex** vagy a **Flux-Mobile.hex** fájlt a kiadások (releases) oldaláról.
2. Csatlakoztasd a programozó fület egy ArduinoISP-t futtató Arduinóhoz. A teljes bekötés és útmutató:
   [Firmware feltöltése egy OS dekóderre](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Töltsd fel a hex fájlt.
4. **Törd le a programozó fület.**

---

## Flux-Signal — jelződekóder

### Működés

A Flux-Signal dekóder négy LED-kimenetet vezérel. A dekóder nem tudja és nem is kell tudnia, milyen színű az egyes LED — ez kizárólag attól függ, hogyan kötöd be a jelzőt. A bitpozíciók egyszerűen a négy fizikai kimeneti forrasztási pontnak felelnek meg:

| Bitpozíció | Kimeneti pont |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

A dekóder a beállított címére küldött **DCC kiterjesztett jelzőparancsokat** (extended accessory, `notifyDccSigOutputState`) figyeli. A parancsban lévő 8 bites állapotérték közvetlenül meghatározza, mely LED-ek világítanak és melyek villognak:

| Állapotbájt bitjei | Funkció |
|---|---|
| Bitek 3:0 (alsó félbájt) | Jelzéskép-maszk — mely kimenetek vannak bekapcsolva |
| Bitek 7:4 (felső félbájt) | Villogásmaszk — ezek közül mely kimenetek villognak folyamatos fény helyett |

Ha a jelzéskép-maszkban egy bitet 1-re állítasz, az adott kimenet bekapcsol. Ha ugyanezt a bitet a villogásmaszkban is beállítod, a kimenet folyamatos fény helyett villogni fog. Azok a kimenetek, amelyeknek a jelzéskép-bitje 0, a villogásmaszktól függetlenül ki vannak kapcsolva.

A villogás periódusideje **333 ms**.

### Az állapotbájt összeállítása

Az állapotértéket a legegyszerűbben a **Windows Számológép Programozó módjában, bináris kijelzéssel** állíthatod össze. Válts BIN-re, és írd be közvetlenül a 8 bitet — a felső négy bit a villogásmaszk, az alsó négy a jelzéskép-maszk. A számológép azonnal megmutatja a küldendő HEX értéket.

![](calculator.png)
*Windows Számológép — Programozó mód, bináris nézet. Felső félbájt = villogásmaszk, alsó félbájt = jelzéskép-maszk.*

A fenti példa a `0010 1011` értéket mutatja (hex `0x2B`):

| Félbájt | Bitek (4 3 2 1) | Jelentés |
|---|---|---|
| Felső `0010` | 1. bit beállítva | LED 1 villog |
| Alsó `1011` | 0., 1., 3. bit beállítva | LED 0 be, LED 1 be, LED 2 ki, LED 3 be |

Eredmény: LED 0 folyamatosan világít — LED 1 villog — LED 2 nem világít — LED 3 folyamatosan világít.

Amíg a dekóder egy új cím betanulására vár, mind a négy LED villog, jelezve ezt az üzemmódot.

### Alapértelmezett beállítások

| Beállítás | Alapértelmezés |
|---|---|
| DCC cím | 1 |
| Parancsmód | DCC kiterjesztett (`dccExt = 1`) |

### A DCC cím beállítása

Tartsd lenyomva az **alsó fülön lévő gombot** **1 másodpercig**. Az összes LED villogni kezd — a dekóder most címre vár.

Küldj tetszőleges DCC parancsot arra a címre, amelyet hozzá szeretnél rendelni:

- Ha **DCC kiterjesztett jelzőparancsot** küldesz — a dekóder eltárolja a címet, és kiterjesztett módban marad (`dccExt = 1`). Ez a normál üzem.
- Ha **hagyományos DCC váltóállító parancsot** küldesz — a dekóder eltárolja a címet, és váltó módba kapcsol (`dccExt = 0`). Ebben a módban a dekóder már nem reagál a kiterjesztett jelzőparancsokra. Akkor hasznos, ha a központod nem tud kiterjesztett jelzőparancsot küldeni.

Az új cím és mód azonnal elmentődik az EEPROM-ba.

---

## Flux-Mobile — járműbe szánt funkciódekóder

### Működés

A Flux-Mobile firmware **multifunkciós (mozdony-) DCC dekóderként** jelentkezik be. Négy funkciókimenete (AUX) van, amelyek mindegyike hozzárendelhető egy DCC funkciószámhoz, és CV-programozással sokféle viselkedésre beállítható.

Az alapértelmezett cím **3** (rövid cím).

### Alapértelmezett kimeneti konfiguráció

Új panelen a négy kimenet hagyományos, menetirány szerinti első/hátsó világításként, valamint két funkcióval kapcsolt kimenetként működik:

| Kimenet | Pin | Alapértelmezett viselkedés |
|---|---|---|
| Aux 0 | 3 | Első világítás — előremenetben világít, logikailag invertált (alacsony szinten aktív LED) |
| Aux 1 | 4 | Hátsó világítás — hátramenetben világít |
| Aux 2 | 1 | F1 kapcsolja |
| Aux 3 | 0 | F2 kapcsolja |

### DCC cím

Szabványos CV-programozással állítható be a DCC programozó vágányon:

| CV | Funkció |
|---|---|
| CV 1 | Rövid cím (1–127) |
| CV 17 + CV 18 | Hosszú cím (előbb a CV17-et írd, utána a CV18-at) |
| CV 8 = 8 | Gyári alapbeállítás visszaállítása |

### Kimeneti CV-táblázat

Mind a négy kimenetnek van egy 10 CV-ből álló blokkja, CV 30-tól kezdődően:

| Kimenet | CV-blokk kezdete |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Egy blokkon belül:

| Eltolás | CV (Aux 0 példa) | Név | Leírás |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = a kimenet követi a mozdony menetirány-bitjét |
| +1 | CV 31 | `inverted` | 1 = a kimenet logikailag invertált |
| +2 | CV 32 | `number` | A kimenetet kapcsoló DCC funkció száma (0 = nincs) |
| +3 | CV 33 | `bleeps` | Rövid felvillanások száma (fejlesztés alatt) |
| +4 | CV 34 | `bleepOn` | Felvillanás bekapcsolt ideje (fejlesztés alatt) |
| +5 | CV 35 | `bleepOff` | Felvillanás kikapcsolt ideje (fejlesztés alatt) |
| +6 | CV 36 | `pulseTime` | Impulzus hossza 100 ms-os egységekben (0 = kikapcsolva) |
| +7 | CV 37 | `slaved` | Másik kimenet tükrözése: a forráskimenet pinszáma (0 = kikapcsolva) |
| +8 | CV 38 | *(fenntartva)* | — |
| +9 | CV 39 | `state` | A kimenet utolsó ismert állapota — automatikusan frissül, ne írd |

### A kimenetek működési logikája

Minden kimenet minden ciklusban a következő prioritási sorrendet követi:

1. **Tükrözés** (`CV+7 ≠ 0`) — a kimenet az ebben a CV-ben megadott pint tükrözi, igény szerint invertálva.
2. **Impulzus** (`CV+6 ≠ 0`) — a vezérlő funkció minden felfutó élénél a kimenet egyetlen, `pulseTime × 100 ms` hosszú impulzust ad.
3. **Folyamatos** — a kimenet a funkció állapotát követi, igény szerint a `CV+1`-gyel invertálva.

Az `onDirChange` jelző (`CV+0`) a kimenetet XOR-kapcsolatba hozza a mozdony aktuális menetirányával, így ugyanaz a funkció a menetiránytól függően különböző kimeneteket kapcsolhat — így működik az alapértelmezett első/hátsó világítás is.
