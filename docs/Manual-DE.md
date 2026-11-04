> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; 🇩🇪 DE &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# OS-Flux Handbuch

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Einleitung

Der OS-Flux ist ein kompakter DCC-Decoder auf Basis eines ATtiny85-Mikrocontrollers. Er hat vier LED-/Funktionsausgänge und passt direkt in ein Signalgehäuse oder in eine Lok.

Je nach aufgespielter Firmware übernimmt er eine von zwei Rollen:

| Firmware | Rolle |
|---|---|
| **Flux-Signal** | Stationärer Signaldecoder — steuert bis zu 4 LEDs für die Signalbegriffe über erweiterte DCC-Zubehörbefehle (DCCext) |
| **Flux-Mobile** | Funktionsdecoder für Fahrzeuge — steuert bis zu 4 Funktionsausgänge (AUX) in einer Lok über DCC-Funktionsbefehle |

Wähle die Firmware vor dem Programmieren aus. Die beiden Hex-Dateien werden getrennt bereitgestellt.

---

## Platinenübersicht

Die Platine hat **zwei abbrechbare Stege**.

![](snapaway.png)
*Die beiden abbrechbaren Stege*

### Oberer Steg — Programmieranschluss

Dies ist der ICSP-Anschluss, über den die Firmware auf den ATtiny85 geflasht wird. Er wird nur beim Programmieren benötigt. **Brich ihn danach ab** — im Betrieb hat er keine Funktion, und ohne ihn ist der Einbau sauberer.

Die vollständige Programmieranleitung findest du hier:
[Flashen einer OS-Decoder-Firmware](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-DE.md)

### Unterer Steg — Adresstaster + JST-Stecker

Dieser Steg trägt:

- einen **Taster** zum Einstellen der DCC-Adresse
- einen **JST-Stecker** für den einfachen Anschluss an den DCC-Bus

**Behalte diesen Steg** beim Einbau als Signaldecoder. Über den JST-Stecker lässt sich das Signal vom DCC-Bus abziehen und wieder anstecken — so sind abnehmbare Signale ganz einfach möglich.

Beim Einsatz in Fahrzeugen (Lok) kann auch der untere Steg abgebrochen werden, wenn er im Weg ist.

---

## Lötpads auf der Rückseite

![](rearsidesolderpads.png)
*Lötpads auf der Rückseite*

Auf der Rückseite der Platine liegen Lötpads für die vier Ausgänge. Sie sind elektrisch mit den Pads auf der Vorderseite identisch — verwende die Seite, die für deinen Einbau am besten passt.

---

## Programmieren

1. Wähle auf der Releases-Seite **Flux-Signal.hex** oder **Flux-Mobile.hex** aus.
2. Verbinde den Programmiersteg mit einem Arduino, auf dem ArduinoISP läuft. Vollständige Verdrahtung und Anleitung:
   [Flashen einer OS-Decoder-Firmware](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-DE.md)
3. Flashe die Hex-Datei.
4. **Brich den Programmiersteg ab.**

---

## Flux-Signal — Signaldecoder

### Funktionsweise

Der Flux-Signal-Decoder steuert vier LED-Ausgänge. Welche Farbe die einzelnen LEDs haben, weiß der Decoder nicht und spielt für ihn auch keine Rolle — das hängt ganz davon ab, wie du dein Signal verdrahtest. Die Bitpositionen entsprechen einfach den vier physischen Ausgangspads:

| Bitposition | Ausgangspad |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Der Decoder reagiert auf **erweiterte DCC-Zubehörbefehle (Signalbegriffe)** (`notifyDccSigOutputState`), die an seine eingestellte Adresse gerichtet sind. Der 8-Bit-Wert in diesem Befehl legt direkt fest, welche LEDs leuchten und welche blinken:

| Bits des Statusbytes | Funktion |
|---|---|
| Bits 3:0 (unteres Halbbyte) | Signalbild-Maske — welche Ausgänge eingeschaltet sind |
| Bits 7:4 (oberes Halbbyte) | Blink-Maske — welche dieser Ausgänge blinken statt dauerhaft zu leuchten |

Setze ein Bit in der Signalbild-Maske, um den zugehörigen Ausgang einzuschalten. Setze dasselbe Bit zusätzlich in der Blink-Maske, damit er blinkt statt dauerhaft zu leuchten. Ausgänge, deren Bit in der Signalbild-Maske 0 ist, bleiben aus — unabhängig von der Blink-Maske.

Die Blinkperiode beträgt **333 ms**.

### Das Statusbyte zusammenstellen

Am einfachsten stellst du einen Statuswert mit dem **Windows-Rechner im Modus „Programmierer“ mit Binäranzeige** zusammen. Schalte auf BIN um und gib die 8 Bits direkt ein — die oberen vier Bits sind die Blink-Maske, die unteren vier die Signalbild-Maske. Der Rechner zeigt sofort den zu sendenden HEX-Wert an.

![](calculator.png)
*Windows-Rechner — Modus „Programmierer“, Binäransicht. Oberes Halbbyte = Blink-Maske, unteres Halbbyte = Signalbild-Maske.*

Das Beispiel oben zeigt `0010 1011` (hex `0x2B`):

| Halbbyte | Bits (4 3 2 1) | Bedeutung |
|---|---|---|
| Oben `0010` | Bit 1 gesetzt | LED 1 blinkt |
| Unten `1011` | Bits 0, 1, 3 gesetzt | LED 0 an, LED 1 an, LED 2 aus, LED 3 an |

Ergebnis: LED 0 leuchtet dauerhaft — LED 1 blinkt — LED 2 aus — LED 3 leuchtet dauerhaft.

Solange der Decoder darauf wartet, eine neue Adresse zu lernen, blinken alle vier LEDs, um diesen Modus anzuzeigen.

### Werkseinstellungen

| Einstellung | Standardwert |
|---|---|
| DCC-Adresse | 1 |
| Befehlsmodus | DCC erweitert (`dccExt = 1`) |

### DCC-Adresse einstellen

Halte den **Taster auf dem unteren Steg** **1 Sekunde** lang gedrückt. Alle LEDs beginnen zu blinken — der Decoder wartet jetzt auf eine Adresse.

Sende einen beliebigen DCC-Befehl an die Adresse, die du vergeben möchtest:

- Sendest du einen **erweiterten DCC-Zubehörbefehl (Signalbegriff)**, speichert der Decoder diese Adresse und bleibt im erweiterten Modus (`dccExt = 1`). Das ist der normale Betrieb.
- Sendest du einen **DCC-Weichenbefehl (Basic-Zubehörbefehl)**, speichert der Decoder diese Adresse und wechselt in den Weichenmodus (`dccExt = 0`). In diesem Modus reagiert der Decoder nicht mehr auf erweiterte Zubehörbefehle. Praktisch, wenn deine Zentrale keine erweiterten Zubehörbefehle senden kann.

Die neue Adresse und der Modus werden sofort im EEPROM gespeichert.

---

## Flux-Mobile — Funktionsdecoder für Fahrzeuge

### Funktionsweise

Die Flux-Mobile-Firmware meldet sich als **Multifunktionsdecoder (Lokdecoder) für DCC**. Sie hat vier Funktionsausgänge (AUX), die jeweils mit einer DCC-Funktionsnummer verknüpft und über CV-Programmierung mit verschiedenen Verhaltensweisen konfiguriert werden können.

Die Standardadresse ist **3** (kurze Adresse).

### Standardbelegung der Ausgänge

Bei einer neuen Platine verhalten sich die vier Ausgänge wie übliches Spitzenlicht vorne/hinten plus zwei über Funktionen geschaltete Zusatzausgänge:

| Ausgang | Pin | Standardverhalten |
|---|---|---|
| Aux 0 | 3 | Spitzenlicht vorne — an bei Vorwärtsfahrt, logisch invertiert (LED aktiv-low) |
| Aux 1 | 4 | Spitzenlicht hinten — an bei Rückwärtsfahrt |
| Aux 2 | 1 | Geschaltet über F1 |
| Aux 3 | 0 | Geschaltet über F2 |

### DCC-Adresse

Einstellung über normale CV-Programmierung auf dem DCC-Programmiergleis:

| CV | Funktion |
|---|---|
| CV 1 | Kurze Adresse (1–127) |
| CV 17 + CV 18 | Lange Adresse (zuerst CV17, dann CV18 schreiben) |
| CV 8 = 8 | Reset auf Werkseinstellungen |

### CV-Tabelle der Ausgänge

Jeder der vier Ausgänge hat einen Block von 10 CVs, beginnend bei CV 30:

| Ausgang | Beginn des CV-Blocks |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Innerhalb jedes Blocks:

| Offset | CV (Beispiel Aux 0) | Name | Beschreibung |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = Ausgang folgt dem Fahrtrichtungsbit der Lok |
| +1 | CV 31 | `inverted` | 1 = Ausgang ist logisch invertiert |
| +2 | CV 32 | `number` | DCC-Funktionsnummer, die diesen Ausgang schaltet (0 = keine) |
| +3 | CV 33 | `bleeps` | Anzahl der Bleeps (in Entwicklung) |
| +4 | CV 34 | `bleepOn` | Einschaltdauer eines Bleeps (in Entwicklung) |
| +5 | CV 35 | `bleepOff` | Ausschaltdauer eines Bleeps (in Entwicklung) |
| +6 | CV 36 | `pulseTime` | Impulsdauer in Einheiten von 100 ms (0 = deaktiviert) |
| +7 | CV 37 | `slaved` | Anderen Ausgang spiegeln: Pin-Nummer des Quellausgangs eintragen (0 = deaktiviert) |
| +8 | CV 38 | *(reserviert)* | — |
| +9 | CV 39 | `state` | Letzter bekannter Ausgangszustand — wird automatisch aktualisiert, nicht beschreiben |

### Logik des Ausgangsverhaltens

Jeder Ausgang durchläuft in jedem Schleifendurchlauf folgende Prioritätskette:

1. **Gekoppelt** (`CV+7 ≠ 0`) — der Ausgang spiegelt den in dieser CV angegebenen Pin, optional invertiert.
2. **Impuls** (`CV+6 ≠ 0`) — der Ausgang gibt bei jeder steigenden Flanke der schaltenden Funktion einen einzelnen Impuls von `pulseTime × 100 ms` aus.
3. **Dauerhaft** — der Ausgang folgt dem Funktionszustand, optional invertiert über `CV+1`.

Das Flag `onDirChange` (`CV+0`) verknüpft den Ausgang per XOR mit der aktuellen Fahrtrichtung der Lok. So kann dieselbe Funktion je nach Fahrtrichtung unterschiedliche Ausgänge schalten — genau so funktioniert das standardmäßige fahrtrichtungsabhängige Spitzenlicht vorne/hinten.
