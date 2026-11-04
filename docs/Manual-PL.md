> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; 🇵🇱 PL &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# Instrukcja OS-Flux

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Wprowadzenie

OS-Flux to kompaktowy dekoder DCC zbudowany na mikrokontrolerze ATtiny85. Ma cztery wyjścia LED/funkcyjne i mieści się bezpośrednio w obudowie semafora lub w lokomotywie.

W zależności od wgranego oprogramowania (firmware) pełni jedną z dwóch ról:

| Firmware | Rola |
|---|---|
| **Flux-Signal** | Stacjonarny dekoder semaforów — steruje maksymalnie 4 diodami LED semafora za pomocą rozszerzonych poleceń DCC dla semaforów |
| **Flux-Mobile** | Dekoder funkcyjny — steruje maksymalnie 4 wyjściami funkcyjnymi (AUX) w lokomotywie za pomocą poleceń funkcji DCC |

Wybierz firmware przed programowaniem. Oba pliki hex są dystrybuowane osobno.

---

## Przegląd płytki

Płytka ma **dwie odłamywane części**.

![](snapaway.png)
*Dwie odłamywane części płytki*

### Górna część — złącze programujące

To złącze ICSP służy do wgrywania firmware do ATtiny85. Jest potrzebne wyłącznie podczas programowania. **Odłam je po zaprogramowaniu** — w eksploatacji do niczego nie służy, a bez niego montaż jest czystszy.

Pełną instrukcję programowania znajdziesz tutaj:
[Wgrywanie firmware do dekodera OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Dolna część — przycisk adresu + złącze JST

Na tej części znajdują się:

- **Przycisk** do ustawiania adresu DCC
- **Złącze JST** do wygodnego podłączenia do magistrali DCC

**Pozostaw tę część** przy montażu jako dekoder semafora. Złącze JST pozwala podłączać i odłączać semafor od magistrali DCC, dzięki czemu semafory wyjmowane z makiety to żaden problem.

Przy zastosowaniu w pojeździe (lokomotywie) dolną część również można odłamać, jeśli przeszkadza.

---

## Pady lutownicze na spodzie

![](rearsidesolderpads.png)
*Pady lutownicze na spodzie płytki*

Na spodzie płytki znajdują się pady lutownicze czterech wyjść. Są one elektrycznie połączone z padami na wierzchu — użyj tej strony, która lepiej pasuje do Twojego montażu.

---

## Programowanie

1. Pobierz **Flux-Signal.hex** lub **Flux-Mobile.hex** ze strony z wydaniami (releases).
2. Podłącz część programującą do Arduino z wgranym szkicem ArduinoISP. Pełny schemat połączeń i instrukcja:
   [Wgrywanie firmware do dekodera OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Wgraj plik hex.
4. **Odłam część programującą.**

---

## Flux-Signal — dekoder semaforów

### Zasada działania

Dekoder Flux-Signal steruje czterema wyjściami LED. Dekoder nie wie i nie musi wiedzieć, jaki kolor ma dana dioda — zależy to wyłącznie od tego, jak podłączysz semafor. Pozycje bitów odpowiadają po prostu czterem fizycznym padom wyjściowym:

| Pozycja bitu | Pad wyjściowy |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Dekoder nasłuchuje **rozszerzonych poleceń DCC dla semaforów** (extended accessory, `notifyDccSigOutputState`) wysłanych na jego skonfigurowany adres. 8-bitowa wartość stanu w tym poleceniu bezpośrednio określa, które diody świecą, a które migają:

| Bity bajtu stanu | Funkcja |
|---|---|
| Bity 3:0 (młodszy półbajt) | Maska wskazania — które wyjścia są włączone |
| Bity 7:4 (starszy półbajt) | Maska migania — które z tych wyjść migają zamiast świecić światłem ciągłym |

Ustaw bit w masce wskazania, aby włączyć dane wyjście. Ustaw ten sam bit w masce migania, aby wyjście migało zamiast świecić ciągle. Wyjścia, których bit w masce wskazania wynosi 0, są wyłączone niezależnie od maski migania.

Okres migania wynosi **333 ms**.

### Składanie bajtu stanu

Najprościej zbudować wartość stanu za pomocą **Kalkulatora Windows w trybie Programisty z widokiem binarnym**. Przełącz na BIN i wpisz bezpośrednio 8 bitów — cztery starsze bity to maska migania, cztery młodsze to maska wskazania. Kalkulator od razu pokazuje wartość HEX do wysłania.

![](calculator.png)
*Kalkulator Windows — tryb Programisty, widok binarny. Starszy półbajt = maska migania, młodszy półbajt = maska wskazania.*

Powyższy przykład pokazuje `0010 1011` (hex `0x2B`):

| Półbajt | Bity (4 3 2 1) | Znaczenie |
|---|---|---|
| Starszy `0010` | ustawiony bit 1 | LED 1 miga |
| Młodszy `1011` | ustawione bity 0, 1, 3 | LED 0 wł., LED 1 wł., LED 2 wył., LED 3 wł. |

Wynik: LED 0 świeci ciągle — LED 1 miga — LED 2 wyłączona — LED 3 świeci ciągle.

Gdy dekoder czeka na nauczenie się nowego adresu, wszystkie cztery diody migają, sygnalizując ten tryb.

### Ustawienia domyślne

| Ustawienie | Domyślnie |
|---|---|
| Adres DCC | 1 |
| Tryb poleceń | DCC rozszerzony (`dccExt = 1`) |

### Ustawianie adresu DCC

Przytrzymaj **przycisk na dolnej części** przez **1 sekundę**. Wszystkie diody zaczynają migać — dekoder czeka teraz na adres.

Wyślij dowolne polecenie DCC na adres, który chcesz przypisać:

- Jeśli wyślesz **rozszerzone polecenie DCC dla semaforów** — dekoder zapamięta ten adres i pozostanie w trybie rozszerzonym (`dccExt = 1`). To normalny tryb pracy.
- Jeśli wyślesz **zwykłe polecenie zwrotnicy DCC** (polecenie akcesoriów) — dekoder zapamięta ten adres i przełączy się w tryb zwrotnicowy (`dccExt = 0`). W tym trybie dekoder nie reaguje już na rozszerzone polecenia dla semaforów. Przydatne, jeśli Twoja centrala nie potrafi wysyłać poleceń rozszerzonych.

Nowy adres i tryb są natychmiast zapisywane w pamięci EEPROM.

---

## Flux-Mobile — dekoder funkcyjny

### Zasada działania

Firmware Flux-Mobile rejestruje się jako **wielofunkcyjny dekoder DCC (lokomotywowy)**. Ma cztery wyjścia funkcyjne (AUX), z których każde można powiązać z numerem funkcji DCC i skonfigurować na wiele sposobów przez programowanie CV.

Adres domyślny to **3** (adres krótki).

### Domyślna konfiguracja wyjść

W nowej płytce cztery wyjścia działają jak klasyczne światła czołowe przód/tył zależne od kierunku jazdy oraz dwa wyjścia sterowane funkcjami:

| Wyjście | Pin | Domyślne działanie |
|---|---|---|
| Aux 0 | 3 | Światła przednie — świecą podczas jazdy do przodu, logicznie odwrócone (LED aktywna stanem niskim) |
| Aux 1 | 4 | Światła tylne — świecą podczas jazdy do tyłu |
| Aux 2 | 1 | Sterowane funkcją F1 |
| Aux 3 | 0 | Sterowane funkcją F2 |

### Adres DCC

Ustawiany standardowym programowaniem CV na torze programowym DCC:

| CV | Funkcja |
|---|---|
| CV 1 | Adres krótki (1–127) |
| CV 17 + CV 18 | Adres długi (najpierw zapisz CV17, potem CV18) |
| CV 8 = 8 | Reset do ustawień fabrycznych |

### Tabela CV wyjść

Każde z czterech wyjść ma blok 10 CV, zaczynając od CV 30:

| Wyjście | Początek bloku CV |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

W obrębie każdego bloku:

| Przesunięcie | CV (przykład Aux 0) | Nazwa | Opis |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = wyjście zależy od bitu kierunku jazdy lokomotywy |
| +1 | CV 31 | `inverted` | 1 = wyjście jest logicznie odwrócone |
| +2 | CV 32 | `number` | Numer funkcji DCC sterującej tym wyjściem (0 = brak) |
| +3 | CV 33 | `bleeps` | Liczba krótkich błysków (w opracowaniu) |
| +4 | CV 34 | `bleepOn` | Czas włączenia błysku (w opracowaniu) |
| +5 | CV 35 | `bleepOff` | Czas wyłączenia błysku (w opracowaniu) |
| +6 | CV 36 | `pulseTime` | Czas trwania impulsu w jednostkach 100 ms (0 = wyłączone) |
| +7 | CV 37 | `slaved` | Powielanie innego wyjścia: wpisz numer pinu wyjścia źródłowego (0 = wyłączone) |
| +8 | CV 38 | *(zarezerwowane)* | — |
| +9 | CV 39 | `state` | Ostatni znany stan wyjścia — aktualizowany automatycznie, nie zapisuj |

### Logika działania wyjść

Każde wyjście w każdym przebiegu pętli sprawdza kolejno następujące priorytety:

1. **Powielanie** (`CV+7 ≠ 0`) — wyjście powiela pin wskazany w tym CV, opcjonalnie w odwróconej logice.
2. **Impuls** (`CV+6 ≠ 0`) — przy każdym zboczu narastającym funkcji sterującej wyjście generuje pojedynczy impuls o długości `pulseTime × 100 ms`.
3. **Praca ciągła** — wyjście podąża za stanem funkcji, opcjonalnie odwrócone przez `CV+1`.

Flaga `onDirChange` (`CV+0`) wykonuje operację XOR stanu wyjścia z aktualnym kierunkiem jazdy lokomotywy. Dzięki temu ta sama funkcja może sterować różnymi wyjściami w zależności od kierunku jazdy — właśnie tak działają domyślne światła przednie i tylne.
