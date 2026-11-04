> 🌐 &nbsp; [🇬🇧 EN](Manual-EN.md) &nbsp;|&nbsp; [🇩🇪 DE](Manual-DE.md) &nbsp;|&nbsp; [🇫🇷 FR](Manual-FR.md) &nbsp;|&nbsp; [🇳🇱 NL](Manual-NL.md) &nbsp;|&nbsp; 🇪🇸 ES &nbsp;|&nbsp; [🇮🇹 IT](Manual-IT.md) &nbsp;|&nbsp; [🇵🇱 PL](Manual-PL.md) &nbsp;|&nbsp; [🇨🇿 CS](Manual-CS.md) &nbsp;|&nbsp; [🇩🇰 DA](Manual-DA.md) &nbsp;|&nbsp; [🇳🇴 NO](Manual-NO.md) &nbsp;|&nbsp; [🇸🇪 SV](Manual-SV.md) &nbsp;|&nbsp; [🇭🇺 HU](Manual-HU.md) &nbsp;|&nbsp; [🇵🇹 PT](Manual-PT.md)

# Manual del OS-Flux

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introducción

El OS-Flux es un decodificador DCC compacto basado en un microcontrolador ATtiny85. Dispone de cuatro salidas para LED/funciones y cabe directamente dentro del cuerpo de una señal o en una locomotora.

Según el firmware que se grabe, cumple una de dos funciones distintas:

| Firmware | Función |
|---|---|
| **Flux-Signal** | Decodificador de señales fijo — controla hasta 4 luces de señal (LED) mediante comandos DCC extendidos para señales |
| **Flux-Mobile** | Decodificador de funciones para locomotoras — controla hasta 4 salidas auxiliares en una locomotora mediante los paquetes de funciones DCC |

Elija el firmware antes de programar. Los dos archivos hex se distribuyen por separado.

---

## Descripción de la placa

La placa tiene **dos pestañas separables**.

![](snapaway.png)
*Las dos pestañas separables*

### Pestaña superior — conector de programación

Es el conector ICSP que se usa para grabar el firmware en el ATtiny85. Solo se necesita durante la programación. **Sepárela después** — una vez instalado el decodificador no tiene ninguna utilidad y el montaje queda más limpio sin ella.

Consulte la guía de programación completa aquí:
[Grabar el firmware en un decodificador OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-ES.md)

### Pestaña inferior — pulsador de dirección + conector JST

Esta pestaña lleva:

- Un **pulsador** para ajustar la dirección DCC
- Un **conector JST** para conectar fácilmente al bus DCC

**Conserve esta pestaña** en las instalaciones como decodificador de señales. Con el conector JST puede conectar y desconectar la señal del bus DCC, lo que facilita tener señales desmontables.

Para el uso móvil (en una locomotora) también puede separar la pestaña inferior si estorba.

---

## Pads de soldadura traseros

![](rearsidesolderpads.png)
*Pads de soldadura de la cara trasera*

En la cara trasera de la placa hay pads de soldadura para las cuatro salidas. Son los mismos puntos eléctricos que los pads de la cara delantera — utilice la cara que mejor convenga a su instalación.

---

## Programación

1. Elija **Flux-Signal.hex** o **Flux-Mobile.hex** en la página de versiones (releases).
2. Conecte la pestaña de programación a un Arduino con ArduinoISP. Cableado e instrucciones completos:
   [Grabar el firmware en un decodificador OS](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-ES.md)
3. Grabe el archivo hex.
4. **Separe la pestaña de programación.**

---

## Flux-Signal — Decodificador de señales

### Funcionamiento

El decodificador Flux-Signal controla cuatro salidas de LED. Al decodificador no le importa de qué color es cada LED — eso depende únicamente de cómo cablee su señal. Las posiciones de bit corresponden simplemente a los cuatro pads de salida físicos:

| Posición de bit | Pad de salida |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

El decodificador responde a los **comandos DCC extendidos para señales** (`notifyDccSigOutputState`) dirigidos a su dirección configurada. El valor de estado de 8 bits de ese comando determina directamente qué LED están encendidos y cuáles parpadean:

| Bits del byte de estado | Función |
|---|---|
| Bits 3:0 (nibble bajo) | Máscara de aspecto — qué salidas están encendidas |
| Bits 7:4 (nibble alto) | Máscara de parpadeo — cuáles de esas salidas parpadean en lugar de quedar fijas |

Active un bit en la máscara de aspecto para encender esa salida. Active el mismo bit en la máscara de parpadeo para que parpadee en lugar de quedar fija. Las salidas cuyo bit de aspecto es 0 están apagadas, independientemente de la máscara de parpadeo.

El periodo de parpadeo es de **333 ms**.

### Componer el byte de estado

La forma más sencilla de obtener un valor de estado es usar la **Calculadora de Windows en modo Programador con visualización binaria**. Cambie a BIN y escriba directamente los 8 bits — los cuatro bits altos son la máscara de parpadeo, los cuatro bajos la máscara de aspecto. La calculadora muestra al instante el valor HEX que hay que enviar.

![](calculator.png)
*Calculadora de Windows — modo Programador, vista binaria. Nibble alto = máscara de parpadeo, nibble bajo = máscara de aspecto.*

El ejemplo anterior muestra `0010 1011` (hex `0x2B`):

| Nibble | Bits (4 3 2 1) | Significado |
|---|---|---|
| Alto `0010` | bit 1 activo | LED 1 parpadea |
| Bajo `1011` | bits 0, 1, 3 activos | LED 0 encendido, LED 1 encendido, LED 2 apagado, LED 3 encendido |

Resultado: LED 0 encendido fijo — LED 1 parpadeando — LED 2 apagado — LED 3 encendido fijo.

Mientras el decodificador espera aprender una nueva dirección, los cuatro LED parpadean para indicar este modo.

### Valores por defecto

| Ajuste | Valor por defecto |
|---|---|
| Dirección DCC | 1 |
| Modo de comando | DCC extendido (`dccExt = 1`) |

### Ajustar la dirección DCC

Mantenga pulsado el **pulsador de la pestaña inferior** durante **1 segundo**. Todos los LED empiezan a parpadear — el decodificador espera ahora una dirección.

Envíe cualquier comando DCC a la dirección que quiera asignar:

- Si envía un **comando DCC extendido para señales**, el decodificador guarda esa dirección y permanece en modo extendido (`dccExt = 1`). Este es el funcionamiento normal.
- Si envía un **comando de accesorio DCC normal (comando de desvío)**, el decodificador guarda esa dirección y pasa al modo desvío (`dccExt = 0`). En este modo el decodificador ya no responde a los comandos extendidos para señales. Útil si su central no puede enviar comandos extendidos para señales.

La nueva dirección y el modo se guardan inmediatamente en la EEPROM.

---

## Flux-Mobile — Decodificador de funciones móvil

### Funcionamiento

El firmware Flux-Mobile se registra como **decodificador DCC multifunción (de locomotora)**. Tiene cuatro salidas auxiliares; cada una puede asociarse a un número de función DCC y configurarse con distintos comportamientos mediante programación de CV.

La dirección por defecto es **3** (dirección corta).

### Configuración de salidas por defecto

En una placa nueva, las cuatro salidas funcionan como luces frontales y traseras convencionales más dos salidas auxiliares controladas por funciones:

| Salida | Pin | Comportamiento por defecto |
|---|---|---|
| Aux 0 | 3 | Luz frontal — encendida en marcha hacia delante, lógica invertida (LED activo a nivel bajo) |
| Aux 1 | 4 | Luz trasera — encendida en marcha hacia atrás |
| Aux 2 | 1 | Controlada por F1 |
| Aux 3 | 0 | Controlada por F2 |

### Dirección DCC

Se ajusta mediante la programación de CV estándar en la vía de programación DCC:

| CV | Función |
|---|---|
| CV 1 | Dirección corta (1–127) |
| CV 17 + CV 18 | Dirección larga (escriba primero CV17 y después CV18) |
| CV 8 = 8 | Restablecer valores de fábrica |

### Tabla de CV de las salidas

Cada una de las cuatro salidas tiene un bloque de 10 CV a partir de la CV 30:

| Salida | Inicio del bloque de CV |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Dentro de cada bloque:

| Desplazamiento | CV (ejemplo Aux 0) | Nombre | Descripción |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = la salida sigue el bit de sentido de marcha de la locomotora |
| +1 | CV 31 | `inverted` | 1 = la salida tiene la lógica invertida |
| +2 | CV 32 | `number` | Número de función DCC que controla esta salida (0 = ninguna) |
| +3 | CV 33 | `bleeps` | Número de pitidos (en desarrollo) |
| +4 | CV 34 | `bleepOn` | Tiempo de encendido del pitido (en desarrollo) |
| +5 | CV 35 | `bleepOff` | Tiempo de apagado del pitido (en desarrollo) |
| +6 | CV 36 | `pulseTime` | Duración del impulso en unidades de 100 ms (0 = desactivado) |
| +7 | CV 37 | `slaved` | Copiar otra salida: indique el número de pin de la salida de origen (0 = desactivado) |
| +8 | CV 38 | *(reservada)* | — |
| +9 | CV 39 | `state` | Último estado conocido de la salida — se actualiza automáticamente, no escribir |

### Lógica de comportamiento de las salidas

En cada ciclo, cada salida recorre la siguiente cadena de prioridades:

1. **Esclava** (`CV+7 ≠ 0`) — la salida copia el pin indicado en esta CV, opcionalmente invertido.
2. **Por impulso** (`CV+6 ≠ 0`) — la salida genera un único impulso de `pulseTime × 100 ms` en cada flanco de subida de la función que la controla.
3. **Fija** — la salida sigue el estado de la función, opcionalmente invertida mediante `CV+1`.

El indicador `onDirChange` (`CV+0`) combina la salida mediante XOR con el sentido de marcha actual de la locomotora, de modo que una misma función puede controlar salidas distintas según el sentido de marcha — así es como funcionan las luces frontales y traseras por defecto.
