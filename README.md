# H7_DMA

Ziel: DMA Uebertragung.

## Memory Layout
[H7 Memory Layout Besonderheiten](https://community.st.com/s/article/FAQ-DMA-is-not-working-on-STM32H7-devices)

Neue Datei anlegen, wie [DMA_Buffer.ld](DMA_Buffer.ld).

```
SECTIONS
{
    .dma_buffer : /* Space before ':' is critical */
    {
    *(.dma_buffer)
    } >RAM_D2
}
```

Die Sections sind bereits in der [STM32H743ZITx_FLASH.ld](STM32H743ZITx_FLASH.ld) definiert.
Die neue .ld file muss noch zum linken eingetragen werden.

In der bereits kopierten **STM32MakeCPP.make**:
```
LDSCRIPT = STM32H743ZITx_FLASH.ld
```
zu
```
LDSCRIPT = STM32H743ZITx_FLASH.ld  DMA_Buffer.ld
```
aendern.

In die **main.h**:
```
/* USER CODE BEGIN Private defines */

#if defined(__ICCARM__)
#define DMA_BUFFER _Pragma("location=\".dma_buffer\"")
#else
#define DMA_BUFFER __attribute__((section(".dma_buffer")))
#endif
```

Neue Variablen koennen nun in der .cpp unter  angelegt werden:

```
/* USER CODE BEGIN PV */

DMA_BUFFER char _char_buffer[200];
```
Diese koennen keine lokalen Variablen sein, und koennen beim compilen nicht direkt beschrieben werden. Diese sind quasi nur der vordefinierte Speicher.

```
HAL_UART_Transmit_DMA(&huart3, (uint8_t *)_char_buffer, strlen(_char_buffer));
```
Damit strlen funktioniert muss der _char_buffer auf \r\n enden.
Beispiel siehe **main.cpp**.

## CubeMX

Hier muss DMA fuer USART3 aktiviert werden, Memory To Peripheral, Normal Mode. USART3 Global Interrupt MUSS ebenfalls aktiviert werden.