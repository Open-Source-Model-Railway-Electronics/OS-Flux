> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; 🇮🇹 IT &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# Manuale OS-Flux

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introduzione

L'OS-Flux è un decoder DCC compatto basato su un microcontrollore ATtiny85. Dispone di quattro uscite LED/funzioni e si monta direttamente nel corpo di un segnale o in una locomotiva.

A seconda del firmware caricato, svolge uno di due ruoli distinti:

| Firmware | Ruolo |
|---|---|
| **Flux-Signal** | Decoder per segnali fisso — comanda fino a 4 luci di segnale (LED) tramite comandi DCC estesi per segnali |
| **Flux-Mobile** | Decoder funzioni per locomotive — comanda fino a 4 uscite ausiliarie su una locomotiva tramite i pacchetti funzione DCC |

Scegliere il firmware prima della programmazione. I due file hex sono distribuiti separatamente.

---

## Panoramica della scheda

La scheda ha **due linguette staccabili**.

![](snapaway.png)
*Le due linguette staccabili*

### Linguetta superiore — connettore di programmazione

È il connettore ICSP usato per caricare il firmware nell'ATtiny85. Serve solo durante la programmazione. **Staccarla in seguito** — una volta installato il decoder non serve più e il montaggio risulta più pulito senza.

La guida completa alla programmazione è disponibile qui:
[Caricare il firmware in un decoder OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-IT.md)

### Linguetta inferiore — pulsante indirizzo + connettore JST

Questa linguetta porta:

- Un **pulsante** per impostare l'indirizzo DCC
- Un **connettore JST** per collegarsi facilmente al bus DCC

**Conservare questa linguetta** nelle installazioni come decoder per segnali. Il connettore JST permette di collegare e scollegare il segnale dal bus DCC, rendendo semplice l'uso di segnali rimovibili.

Per l'uso mobile (in locomotiva) anche la linguetta inferiore può essere staccata, se d'ingombro.

---

## Piazzole di saldatura posteriori

![](rearsidesolderpads.png)
*Piazzole di saldatura sul lato posteriore*

Sul retro della scheda sono presenti piazzole di saldatura per le quattro uscite. Sono gli stessi nodi elettrici delle piazzole sul lato anteriore — usare il lato più comodo per la propria installazione.

---

## Programmazione

1. Scegliere **Flux-Signal.hex** o **Flux-Mobile.hex** dalla pagina delle release.
2. Collegare la linguetta di programmazione a un Arduino con ArduinoISP. Collegamenti e istruzioni completi:
   [Caricare il firmware in un decoder OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-IT.md)
3. Caricare il file hex.
4. **Staccare la linguetta di programmazione.**

---

## Flux-Signal — Decoder per segnali

### Funzionamento

Il decoder Flux-Signal comanda quattro uscite LED. Al decoder non interessa di che colore sia ciascun LED — dipende interamente da come si cabla il segnale. Le posizioni dei bit corrispondono semplicemente alle quattro piazzole di uscita fisiche:

| Posizione bit | Piazzola di uscita |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

Il decoder risponde ai **comandi DCC estesi per segnali** (`notifyDccSigOutputState`) inviati al suo indirizzo configurato. Il valore di stato a 8 bit contenuto nel comando stabilisce direttamente quali LED sono accesi e quali lampeggiano:

| Bit del byte di stato | Funzione |
|---|---|
| Bit 3:0 (nibble basso) | Maschera aspetto — quali uscite sono accese |
| Bit 7:4 (nibble alto) | Maschera lampeggio — quali di queste uscite lampeggiano invece di restare fisse |

Impostare un bit nella maschera aspetto per accendere quell'uscita. Impostare lo stesso bit nella maschera lampeggio per farla lampeggiare invece di restare fissa. Le uscite con bit di aspetto a 0 sono spente, indipendentemente dalla maschera lampeggio.

Il periodo di lampeggio è di **333 ms**.

### Comporre il byte di stato

Il modo più semplice per ricavare un valore di stato è usare la **Calcolatrice di Windows in modalità Programmatore con visualizzazione binaria**. Passare a BIN e digitare direttamente gli 8 bit — i quattro bit alti sono la maschera lampeggio, i quattro bassi la maschera aspetto. La calcolatrice mostra subito il valore HEX da inviare.

![](calculator.png)
*Calcolatrice di Windows — modalità Programmatore, vista binaria. Nibble alto = maschera lampeggio, nibble basso = maschera aspetto.*

L'esempio sopra mostra `0010 1011` (hex `0x2B`):

| Nibble | Bit (4 3 2 1) | Significato |
|---|---|---|
| Alto `0010` | bit 1 impostato | LED 1 lampeggia |
| Basso `1011` | bit 0, 1, 3 impostati | LED 0 acceso, LED 1 acceso, LED 2 spento, LED 3 acceso |

Risultato: LED 0 acceso fisso — LED 1 lampeggiante — LED 2 spento — LED 3 acceso fisso.

Mentre il decoder attende di apprendere un nuovo indirizzo, tutti e quattro i LED lampeggiano per segnalare questa modalità.

### Impostazioni predefinite

| Impostazione | Valore predefinito |
|---|---|
| Indirizzo DCC | 1 |
| Modalità di comando | DCC esteso (`dccExt = 1`) |

### Impostare l'indirizzo DCC

Tenere premuto il **pulsante sulla linguetta inferiore** per **1 secondo**. Tutti i LED iniziano a lampeggiare — il decoder è ora in attesa di un indirizzo.

Inviare un qualsiasi comando DCC all'indirizzo da assegnare:

- Se si invia un **comando DCC esteso per segnali**, il decoder memorizza quell'indirizzo e resta in modalità estesa (`dccExt = 1`). È il funzionamento normale.
- Se si invia un **normale comando DCC per accessori (comando scambio)**, il decoder memorizza quell'indirizzo e passa alla modalità scambio (`dccExt = 0`). In questa modalità il decoder non risponde più ai comandi estesi per segnali. Utile se la propria centrale non è in grado di inviare comandi estesi per segnali.

Il nuovo indirizzo e la modalità vengono salvati subito nella EEPROM.

---

## Flux-Mobile — Decoder funzioni mobile

### Funzionamento

Il firmware Flux-Mobile si presenta come **decoder DCC multifunzione (per locomotive)**. Ha quattro uscite ausiliarie, ognuna delle quali può essere associata a un numero di funzione DCC e configurata con diversi comportamenti tramite la programmazione delle CV.

L'indirizzo predefinito è **3** (indirizzo corto).

### Configurazione predefinita delle uscite

Su una scheda nuova le quattro uscite funzionano come normali fari anteriori/posteriori più due uscite ausiliarie comandate da funzioni:

| Uscita | Pin | Comportamento predefinito |
|---|---|---|
| Aux 0 | 3 | Fari anteriori — accesi in marcia avanti, logica invertita (LED attivo basso) |
| Aux 1 | 4 | Fari posteriori — accesi in marcia indietro |
| Aux 2 | 1 | Comandata da F1 |
| Aux 3 | 0 | Comandata da F2 |

### Indirizzo DCC

Si imposta con la normale programmazione delle CV sul binario di programmazione DCC:

| CV | Funzione |
|---|---|
| CV 1 | Indirizzo corto (1–127) |
| CV 17 + CV 18 | Indirizzo lungo (scrivere prima CV17, poi CV18) |
| CV 8 = 8 | Reset alle impostazioni di fabbrica |

### Tabella CV delle uscite

Ciascuna delle quattro uscite ha un blocco di 10 CV a partire dalla CV 30:

| Uscita | Inizio blocco CV |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

All'interno di ogni blocco:

| Offset | CV (esempio Aux 0) | Nome | Descrizione |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = l'uscita segue il bit del senso di marcia della locomotiva |
| +1 | CV 31 | `inverted` | 1 = l'uscita ha la logica invertita |
| +2 | CV 32 | `number` | Numero di funzione DCC che comanda questa uscita (0 = nessuna) |
| +3 | CV 33 | `bleeps` | Numero di bip (in sviluppo) |
| +4 | CV 34 | `bleepOn` | Tempo di accensione del bip (in sviluppo) |
| +5 | CV 35 | `bleepOff` | Tempo di spegnimento del bip (in sviluppo) |
| +6 | CV 36 | `pulseTime` | Durata dell'impulso in unità di 100 ms (0 = disattivato) |
| +7 | CV 37 | `slaved` | Replica un'altra uscita: impostare il numero di pin dell'uscita sorgente (0 = disattivato) |
| +8 | CV 38 | *(riservata)* | — |
| +9 | CV 39 | `state` | Ultimo stato noto dell'uscita — aggiornato automaticamente, non scrivere |

### Logica di comportamento delle uscite

A ogni ciclo, ciascuna uscita percorre la seguente catena di priorità:

1. **Asservita** (`CV+7 ≠ 0`) — l'uscita replica il pin indicato in questa CV, eventualmente invertito.
2. **A impulso** (`CV+6 ≠ 0`) — l'uscita emette un singolo impulso di `pulseTime × 100 ms` a ogni fronte di salita della funzione che la comanda.
3. **Fissa** — l'uscita segue lo stato della funzione, eventualmente invertita tramite `CV+1`.

Il flag `onDirChange` (`CV+0`) combina l'uscita in XOR con il senso di marcia attuale della locomotiva, così la stessa funzione può comandare uscite diverse a seconda del senso di marcia — è così che funzionano i fari anteriori/posteriori predefiniti.
