> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; [🇪🇸 ES](Manual-ES.md) &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; 🇵🇹 PT

# Manual do OS-Flux

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introdução

O OS-Flux é um decoder DCC compacto baseado num microcontrolador ATtiny85. Tem quatro saídas para LED/funções e cabe directamente no corpo de um sinal ou numa locomotiva.

Consoante o firmware gravado, desempenha uma de duas funções distintas:

| Firmware | Função |
|---|---|
| **Flux-Signal** | Decoder de sinais fixo — comanda até 4 luzes de sinal (LED) através de comandos DCC estendidos para sinais |
| **Flux-Mobile** | Decoder de funções para locomotivas — comanda até 4 saídas auxiliares numa locomotiva através dos pacotes de funções DCC |

Escolha o firmware antes de programar. Os dois ficheiros hex são distribuídos separadamente.

---

## Visão Geral da Placa

A placa tem **duas abas destacáveis**.

![](snapaway.png)
*As duas abas destacáveis*

### Aba superior — conector de programação

É o conector ICSP utilizado para gravar o firmware no ATtiny85. Só é necessário durante a programação. **Destaque-a depois** — depois de instalado o decoder já não tem utilidade e a montagem fica mais limpa sem ela.

Consulte o guia de programação completo aqui:
[Gravar o firmware num decoder OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Aba inferior — botão de endereço + conector JST

Esta aba contém:

- Um **botão** para definir o endereço DCC
- Um **conector JST** para ligar facilmente ao bus DCC

**Mantenha esta aba** nas instalações como decoder de sinais. O conector JST permite ligar e desligar o sinal do bus DCC, o que facilita a utilização de sinais amovíveis.

Para uso móvel (numa locomotiva), a aba inferior também pode ser destacada se estorvar.

---

## Pads de soldadura traseiros

![](rearsidesolderpads.png)
*Pads de soldadura do lado traseiro*

O lado traseiro da placa tem pads de soldadura para as quatro saídas. São os mesmos pontos eléctricos que os pads do lado da frente — utilize o lado que for mais conveniente para a sua instalação.

---

## Programação

1. Escolha **Flux-Signal.hex** ou **Flux-Mobile.hex** na página de versões (releases).
2. Ligue a aba de programação a um Arduino com ArduinoISP. Ligações e instruções completas:
   [Gravar o firmware num decoder OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Grave o ficheiro hex.
4. **Destaque a aba de programação.**

---

## Flux-Signal — Decoder de Sinais

### Funcionamento

O decoder Flux-Signal comanda quatro saídas LED. O decoder não sabe nem quer saber de que cor é cada LED — isso depende inteiramente da forma como liga o seu sinal. As posições dos bits correspondem simplesmente aos quatro pads de saída físicos:

| Posição do bit | Pad de saída |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

O decoder responde aos **comandos DCC estendidos para sinais** (`notifyDccSigOutputState`) dirigidos ao seu endereço configurado. O valor de estado de 8 bits desse comando define directamente que LED estão acesos e quais piscam:

| Bits do byte de estado | Função |
|---|---|
| Bits 3:0 (nibble baixo) | Máscara de aspecto — que saídas estão acesas |
| Bits 7:4 (nibble alto) | Máscara de piscar — quais dessas saídas piscam em vez de ficarem fixas |

Active um bit na máscara de aspecto para acender essa saída. Active o mesmo bit na máscara de piscar para a fazer piscar em vez de ficar fixa. As saídas cujo bit de aspecto é 0 ficam apagadas, independentemente da máscara de piscar.

O período de intermitência é de **333 ms**.

### Compor o byte de estado

A forma mais simples de obter um valor de estado é usar a **Calculadora do Windows no modo Programador com visualização binária**. Mude para BIN e escreva directamente os 8 bits — os quatro bits altos são a máscara de piscar, os quatro baixos a máscara de aspecto. A calculadora mostra de imediato o valor HEX a enviar.

![](calculator.png)
*Calculadora do Windows — modo Programador, vista binária. Nibble alto = máscara de piscar, nibble baixo = máscara de aspecto.*

O exemplo acima mostra `0010 1011` (hex `0x2B`):

| Nibble | Bits (4 3 2 1) | Significado |
|---|---|---|
| Alto `0010` | bit 1 activo | LED 1 pisca |
| Baixo `1011` | bits 0, 1, 3 activos | LED 0 aceso, LED 1 aceso, LED 2 apagado, LED 3 aceso |

Resultado: LED 0 aceso fixo — LED 1 a piscar — LED 2 apagado — LED 3 aceso fixo.

Enquanto o decoder aguarda a aprendizagem de um novo endereço, os quatro LED piscam para indicar este modo.

### Configuração predefinida

| Parâmetro | Predefinição |
|---|---|
| Endereço DCC | 1 |
| Modo de comando | DCC estendido (`dccExt = 1`) |

### Definir o endereço DCC

Mantenha premido o **botão da aba inferior** durante **1 segundo**. Todos os LED começam a piscar — o decoder aguarda agora um endereço.

Envie um comando DCC qualquer para o endereço que pretende atribuir:

- Se enviar um **comando DCC estendido para sinais**, o decoder guarda esse endereço e mantém-se no modo estendido (`dccExt = 1`). É o funcionamento normal.
- Se enviar um **comando de acessório DCC normal (comando de agulha)**, o decoder guarda esse endereço e passa para o modo agulha (`dccExt = 0`). Neste modo o decoder deixa de responder aos comandos estendidos para sinais. Útil se a sua central não conseguir enviar comandos estendidos para sinais.

O novo endereço e o modo são guardados imediatamente na EEPROM.

---

## Flux-Mobile — Decoder de Funções Móvel

### Funcionamento

O firmware Flux-Mobile apresenta-se como **decoder DCC multifunções (de locomotiva)**. Tem quatro saídas auxiliares; cada uma pode ser associada a um número de função DCC e configurada com vários comportamentos através da programação de CV.

O endereço predefinido é **3** (endereço curto).

### Configuração predefinida das saídas

Numa placa nova, as quatro saídas funcionam como as habituais luzes direccionais da frente e de trás, mais duas saídas auxiliares comandadas por funções:

| Saída | Pino | Comportamento predefinido |
|---|---|---|
| Aux 0 | 3 | Luzes da frente — acesas em marcha para a frente, lógica invertida (LED activo a nível baixo) |
| Aux 1 | 4 | Luzes de trás — acesas em marcha atrás |
| Aux 2 | 1 | Comandada por F1 |
| Aux 3 | 0 | Comandada por F2 |

### Endereço DCC

Define-se através da programação normal de CV na via de programação DCC:

| CV | Função |
|---|---|
| CV 1 | Endereço curto (1–127) |
| CV 17 + CV 18 | Endereço longo (escreva primeiro a CV17 e depois a CV18) |
| CV 8 = 8 | Reposição dos valores de fábrica |

### Tabela de CV das saídas

Cada uma das quatro saídas tem um bloco de 10 CV a começar na CV 30:

| Saída | Início do bloco de CV |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Dentro de cada bloco:

| Deslocamento | CV (exemplo Aux 0) | Nome | Descrição |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = a saída segue o bit de sentido de marcha da locomotiva |
| +1 | CV 31 | `inverted` | 1 = a saída tem a lógica invertida |
| +2 | CV 32 | `number` | Número da função DCC que comanda esta saída (0 = nenhuma) |
| +3 | CV 33 | `bleeps` | Número de bips (em desenvolvimento) |
| +4 | CV 34 | `bleepOn` | Tempo ligado do bip (em desenvolvimento) |
| +5 | CV 35 | `bleepOff` | Tempo desligado do bip (em desenvolvimento) |
| +6 | CV 36 | `pulseTime` | Duração do impulso em unidades de 100 ms (0 = desactivado) |
| +7 | CV 37 | `slaved` | Copiar outra saída: indique o número do pino da saída de origem (0 = desactivado) |
| +8 | CV 38 | *(reservada)* | — |
| +9 | CV 39 | `state` | Último estado conhecido da saída — actualizado automaticamente, não escrever |

### Lógica de comportamento das saídas

Em cada ciclo, cada saída percorre a seguinte cadeia de prioridades:

1. **Escrava** (`CV+7 ≠ 0`) — a saída copia o pino indicado nesta CV, opcionalmente invertido.
2. **Por impulso** (`CV+6 ≠ 0`) — a saída gera um único impulso de `pulseTime × 100 ms` em cada flanco ascendente da função que a comanda.
3. **Fixa** — a saída segue o estado da função, opcionalmente invertida através da `CV+1`.

O indicador `onDirChange` (`CV+0`) combina a saída por XOR com o sentido de marcha actual da locomotiva, permitindo que a mesma função comande saídas diferentes consoante o sentido de marcha — é assim que funcionam as luzes da frente e de trás predefinidas.
